#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartJump.hh"
#include "game/kart/KartMove.hh"

#include "game/system/RaceManager.hh"

namespace Kart {

struct StartBoostEntry {
    f32 range;
    s16 frames;
};

/// @addr{0x808B64F8}
/// @memberof KartState
static constexpr std::array<StartBoostEntry, 6> START_BOOST_ENTRIES = {{
        {0.85f, 0},
        {0.88f, 10},
        {0.905f, 20},
        {0.925f, 30},
        {0.94f, 45},
        {0.95f, 70},
}};

/// @addr{0x805943B4}
KartState::KartState() {
    clearBitfield0();
    clearBitfield1();
    clearBitfield2();
    clearBitfield3();

    m_bAutoDrift = inputs()->driftIsAuto();

    m_airtime = 0;
    m_cannonPointId = 0;
    m_startBoostIdx = 0;
}

/// @addr{0x8059455C}
void KartState::init() {
    reset();
}

/// @addr{0x80594594}
void KartState::reset() {
    clearBitfield3();
    clearBitfield2();
    clearBitfield1();
    clearBitfield0();

    m_airtime = 0;
    m_top.setZero();
    m_hwgTimer = 0;
    m_boostRampType = -1;
    m_jumpPadVariant = -1;
    m_halfPipeInvisibilityTimer = 0;
    m_startBoostCharge = 0.0f;
    m_stickX = 0.0f;
    m_wallBonkTimer = 0;
    m_trickableTimer = 0;
}

/// @stage 1+
/// @brief Each frame, read input and save related bit flags. Also handles start boosts.
/// @addr{0x8059487C}
void KartState::calcInput() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        if (!m_bInAction && !m_bBeforeRespawn && !m_bCannonStart && !m_bInCannon &&
                !m_bOverZipper) {
            const auto &currentState = inputs()->currentState();
            const auto &lastState = inputs()->lastState();
            m_stickX = currentState.stick.x;
            m_stickY = currentState.stick.y;

            if (!m_bRejectRoadTrigger) {
                if (m_stickX < 0.0f) {
                    m_bStickLeft = true;
                } else if (m_stickX > 0.0f) {
                    m_bStickRight = true;
                }
            }

            if (!m_bBurnout) {
                m_bAccelerate = currentState.accelerate();
                m_bAccelerateStart = m_bAccelerate && !lastState.accelerate();
                m_bBrake = currentState.brake();

                if (!m_bAutoDrift) {
                    m_bDriftInput = currentState.drift();
                    m_bHopStart = m_bDriftInput && !lastState.drift();
                }
            }
        }

        calcHandleStartBoost();
    } else {
        if (!raceMgr->isStageReached(System::RaceManager::Stage::Countdown)) {
            return;
        }

        const auto &currentState = inputs()->currentState();
        m_stickX = currentState.stick.x;
        m_bChargeStartBoost = currentState.accelerate();

        calcStartBoost();
    }
}

/// @stage All
/// @brief Every frame, resets the input state and saves collision-related bit flags.
/// @addr{0x8059474C}
void KartState::calc() {
    resetFlags();

    collide()->calcBeforeRespawn();

    calcCollisions();
    collide()->calcBoundingRadius();
}

/// @addr{0x80594704}
void KartState::resetFlags() {
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bGroundStart = false;
    m_bStickLeft = false;
    m_bWallCollisionStart = false;
    m_bAirStart = false;
    m_bStickRight = false;

    m_bZipperInvisibleWall = false;

    m_bCollidingOffroad = false;
    m_bJumpPadDisableYsusForce = false;

    m_stickY = 0.0f;
    m_stickX = 0.0f;
}

/// @stage All
/// @brief Each frame, checks for collision and saves relevant bit flags.
/// @addr{0x80594BD4}
/// @details Iterates each tire to check for collision. If any tire is colliding with the floor,
/// the "Any Wheel Collision" bit is set. If all tires are colliding with the floor, the
/// "All Wheels Collision" bit is set. Tracks airtime and computes the appropriate
/// top vector, given the floor normals of all colliding floor KCLs.
void KartState::calcCollisions() {
    bool wasTouchingGround = m_bTouchingGround;
    bool wasWallCollision = m_bWallCollision || m_bWall3Collision;

    m_bWall3Collision = false;
    m_bWallCollision = false;
    m_bVehicleBodyFloorCollision = false;
    m_bAnyWheelCollision = false;
    m_bAllWheelsCollision = false;
    m_bTouchingGround = false;

    if (m_hwgTimer > 0) {
        if (--m_hwgTimer == 0) {
            m_bUNK2 = false;
            m_bSomethingWallCollision = false;
        }
    }

    m_top.setZero();
    bool softWallCollision = false;

    if (collide()->someSoftWallTimer() > 0) {
        if (collide()->someNonSoftWallTimer() == 0) {
            softWallCollision = true;
        } else {
            f32 softSusp = collide()->suspBottomHeightSoftWall() /
                    static_cast<f32>(collide()->someSoftWallTimer());
            f32 nonSusp = collide()->suspBottomHeightNonSoftWall() /
                    static_cast<f32>(collide()->someNonSoftWallTimer());

            if (softSusp - nonSusp >= 40.0f) {
                m_bSoftWallDrift = false;
            } else {
                softWallCollision = true;
            }
        }
    }

    u16 wheelCollisions = 0;
    u16 softWallCount = 0;
    EGG::Vector3f wallNrm = EGG::Vector3f::zero;
    bool trickable = false;

    for (u16 tireIdx = 0; tireIdx < tireCount(); ++tireIdx) {
        const auto &colData = collisionData(tireIdx);
        if (hasFloorCollision(tirePhysics(tireIdx))) {
            m_top += colData.floorNrm;
            trickable = trickable || colData.bTrickable;
            ++wheelCollisions;
        }

        if (softWallCollision && colData.bSoftWall) {
            ++softWallCount;
            wallNrm += colData.noBounceWallNrm;
        }
    }

    if (wheelCollisions > 0) {
        m_bAnyWheelCollision = true;
        if (wheelCollisions == tireCount()) {
            m_bAllWheelsCollision = true;
        }
    }

    CollisionData &colData = collisionData();
    if (colData.bFloor) {
        m_bVehicleBodyFloorCollision = true;
        m_top += colData.floorNrm;
        trickable = trickable || colData.bTrickable;

        if (m_bOverZipper && !m_bHalfpipeMidair) {
            halfPipe()->end(true);
        }
    }

    bool hitboxGroupSoftWallCollision = false;
    if (softWallCollision && colData.bSoftWall) {
        hitboxGroupSoftWallCollision = true;
        ++softWallCount;
        wallNrm += colData.wallNrm;
    }

    bool bVar3 = colData.bInvisibleWallOnly && m_halfPipeInvisibilityTimer > 0;
    m_halfPipeInvisibilityTimer = std::max(0, m_halfPipeInvisibilityTimer - 1);

    m_wallBonkTimer = std::max(0, m_wallBonkTimer - 1);

    bool hwg = false;

    if ((colData.bWall || colData.bWall3) && !bVar3) {
        if (colData.bWall) {
            m_bWallCollision = true;
        }

        if (colData.bWall3) {
            m_bWall3Collision = true;
        }

        if (!wasWallCollision) {
            m_bWallCollisionStart = true;

            if (wallKclType() == COL_TYPE_SPECIAL_WALL && wallKclVariant() == 0) {
                if (!state()->isTriggerRespawn() && !state()->isInRespawn() &&
                        !state()->isAfterRespawn() && !state()->isBeforeRespawn() &&
                        !state()->isInAction() && !state()->isCannonStart() &&
                        !state()->isInCannon()) {
                    action()->start(Kart::Action::UNK_1);
                }
            }
        }

        m_wallBonkTimer = 2;

        if (m_hwgTimer == 0 && colData.movement.y > 1.0f) {
            EGG::Vector3f movement = colData.movement;
            movement.normalise();

            if (movement.dot(EGG::Vector3f::ey) > 0.8f &&
                    colData.wallNrm.dot(EGG::Vector3f::ey) > 0.85f &&
                    (movement.x * colData.wallNrm.x + movement.z * colData.wallNrm.z < 0.0f ||
                            collide()->colPerpendicularity() >= 1.0f)) {
                colData.wallNrm.y = 0.0f;
                colData.wallNrm.normalise();
                wallNrm = colData.wallNrm;

                if (wallNrm.length() < 0.05f) {
                    wallNrm = movement;
                    wallNrm.y = 0.0f;
                }

                hwg = true;
            }
        }
    }

    if (colData.bInvisibleWall && m_bHalfPipeRamp &&
            collide()->surfaceFlags().offBit(KartCollide::eSurfaceFlags::StopHalfPipeState)) {
        m_bZipperInvisibleWall = true;
    }

    if (softWallCount > 0 || hwg) {
        m_bUNK2 = true;
        m_softWallSpeed = wallNrm;
        m_softWallSpeed.normalise();
        if (softWallCount > 0 && !m_bHop) {
            m_bSoftWallDrift = true;
        }

        if (hwg) {
            m_bHWG = true;
        }

        if (hitboxGroupSoftWallCollision || hwg || isBike()) {
            m_bSomethingWallCollision = true;
            m_hwgTimer = 10;

            if (hwg) {
                m_hwgTimer *= 2;
            }
        }
    }

    m_bAirtimeOver20 = false;
    m_trickableTimer = std::max(0, m_trickableTimer - 1);

    if (wheelCollisions < 1 && !colData.bFloor) {
        if (wasTouchingGround) {
            m_bAirStart = true;
        }

        if (++m_airtime > 20) {
            m_bAirtimeOver20 = true;
        }
    } else {
        m_top.normalise();

        m_bTouchingGround = true;
        m_bAfterCannon = false;

        if (!m_bInAction) {
            m_bActionMidZipper = false;
            m_bEndHalfPipe = false;
        }

        if (m_bOverZipper) {
            halfPipe()->end(true);
        }

        if (trickable) {
            m_trickableTimer = 3;
        }

        m_bTrickable = m_trickableTimer > 0;

        if (!m_bJumpPad) {
            m_bJumpPadMushroomCollision = false;
        }

        if (!wasTouchingGround) {
            m_bGroundStart = true;
        }

        if (m_bInATrick && jump()->cooldown() == 0) {
            move()->landTrick();
            dynamics()->setForceUpright(true);
            jump()->end();
        }

        m_airtime = 0;
    }
}

/// @brief STAGE 1 - Each frame, calculates the start boost charge.
/// @addr{0x80595918}
/// @details If the player is holding accelerate, the start boost charge increases using exponential
/// decay. If the player is not holding accelerate, the start boost charge decays by 4% each frame.
void KartState::calcStartBoost() {
    constexpr f32 START_BOOST_DELTA_ONE = 0.02f;
    constexpr f32 START_BOOST_DELTA_TWO = 0.002f;
    constexpr f32 START_BOOST_FALLOFF = 0.96f;

    if (m_bChargeStartBoost) {
        m_startBoostCharge += START_BOOST_DELTA_ONE -
                (START_BOOST_DELTA_ONE - START_BOOST_DELTA_TWO) * m_startBoostCharge;
    } else {
        m_startBoostCharge *= START_BOOST_FALLOFF;
    }

    m_startBoostCharge = std::max(0.0f, std::min(1.0f, m_startBoostCharge));
}

/// @stage 1
/// @brief On countdown end, calculates and applies our start boost charge.
/// @addr{0x805959D4}
void KartState::calcHandleStartBoost() {
    if (System::RaceManager::Instance()->getCountdownTimer() != 0) {
        return;
    }

    if (m_bAccelerate) {
        if (m_startBoostCharge > START_BOOST_ENTRIES.back().range) {
            m_startBoostIdx = std::numeric_limits<size_t>::max();
        } else if (m_startBoostCharge > START_BOOST_ENTRIES.front().range) {
            // Ranges are exclusive on the lower bound and inclusive on the upper bound
            for (size_t i = 1; i < START_BOOST_ENTRIES.size(); ++i) {
                if (m_startBoostCharge > START_BOOST_ENTRIES[i - 1].range &&
                        m_startBoostCharge <= START_BOOST_ENTRIES[i].range) {
                    m_startBoostIdx = i;
                    break;
                }
            }
        }
    }

    if (m_startBoostIdx <= 0) {
        return;
    }

    handleStartBoost(m_startBoostIdx);
    m_bChargeStartBoost = false;
}

/// @brief Applies the relevant start boost duration.
/// @addr{0x80595AF8}
/// @param idx The index into the start boost entries array.
void KartState::handleStartBoost(size_t idx) {
    if (m_startBoostIdx == std::numeric_limits<size_t>::max()) {
        move()->burnout().start();
    } else {
        move()->applyStartBoost(START_BOOST_ENTRIES[idx].frames);
    }
}

/// @brief Resets certain bitfields pertaining to ejections (reject road, half pipe zippers, etc.)
/// @addr{0x805958F0}
void KartState::resetEjection() {
    m_bHalfPipeRamp = false;
    m_bRejectRoad = false;
}

/// @brief Helper function to clear all bit flags at 0x4-0x7 in KartState.
void KartState::clearBitfield0() {
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bDriftManual = false;
    m_bBeforeRespawn = false;
    m_bWall3Collision = false;
    m_bWallCollision = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bGroundStart = false;
    m_bVehicleBodyFloorCollision = false;
    m_bAnyWheelCollision = false;
    m_bAllWheelsCollision = false;
    m_bStickLeft = false;
    m_bWallCollisionStart = false;
    m_bAirtimeOver20 = false;
    m_bStickyRoad = false;
    m_bTouchingGround = false;
    m_bHop = false;
    m_bBoost = false;
    m_bAirStart = false;
    m_bStickRight = false;
    m_bMushroomBoost = false;
    m_bDriftAuto = false;
    m_bSlipdriftCharge = false;
    m_bWheelie = false;
    m_bJumpPad = false;
    m_bRampBoost = false;
}

/// @brief Helper function to clear all bit flags at 0x8-0xB in KartState.
void KartState::clearBitfield1() {
    m_bInAction = false;
    m_bTriggerRespawn = false;
    m_bCannonStart = false;
    m_bInCannon = false;
    m_bTrickStart = false;
    m_bInATrick = false;
    m_bBoostOffroadInvincibility = false;
    m_bHalfPipeRamp = false;
    m_bOverZipper = false;
    m_bJumpPadMushroomCollision = false;
    m_bZipperInvisibleWall = false;
    m_bDisableBackwardsAccel = false;
    m_bZipperBoost = false;
    m_bZipperStick = false;
    m_bZipperTrick = false;
    m_bRespawnKillY = false;
    m_bBurnout = false;
    m_bTrickRot = false;
    m_bJumpPadMushroomVelYInc = false;
    m_bChargingSsmt = false;
    m_bRejectRoad = false;
    m_bRejectRoadTrigger = false;
    m_bTrickable = false;
}

/// @brief Helper function to clear all bit flags at 0xC-0xF in KartState.
void KartState::clearBitfield2() {
    m_bWheelieRot = false;
    m_bSkipWheelCalc = false;
    m_bJumpPadMushroomTrigger = false;
    m_bNoSparkInvisibleWall = false;
    m_bCollidingOffroad = false;
    m_bInRespawn = false;
    m_bAfterRespawn = false;
    m_bCrushed = false;
    m_bJumpPadFixedSpeed = false;
    m_bJumpPadDisableYsusForce = false;
    m_bHalfpipeMidair = false;
}

/// @brief Helper function to clear all bit flags at 0x10-0x13 in KartState.
void KartState::clearBitfield3() {
    m_bUNK2 = false;
    m_bSomethingWallCollision = false;
    m_bSoftWallDrift = false;
    m_bHWG = false;
    m_bAfterCannon = false;
    m_bActionMidZipper = false;
    m_bChargeStartBoost = false;
    m_bEndHalfPipe = false;
}

} // namespace Kart
