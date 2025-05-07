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
    m_status.makeAllZero();

    m_status.changeBit(inputs()->driftIsAuto(), eStatus::AutoDrift);

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
    bool isAutoDrift = m_status.onBit(eStatus::AutoDrift);
    m_status.makeAllZero().changeBit(isAutoDrift, eStatus::AutoDrift);

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
        if (m_status.offBit(eStatus::InAction, eStatus::BeforeRespawn, eStatus::CannonStart,
                    eStatus::InCannon, eStatus::OverZipper)) {
            const auto &currentState = inputs()->currentState();
            const auto &lastState = inputs()->lastState();
            m_stickX = currentState.stick.x;
            m_stickY = currentState.stick.y;

            if (m_status.offBit(eStatus::RejectRoadTrigger)) {
                if (m_stickX < 0.0f) {
                    m_status.setBit(eStatus::StickLeft);
                } else if (m_stickX > 0.0f) {
                    m_status.setBit(eStatus::StickRight);
                }
            }

            if (m_status.offBit(eStatus::Burnout)) {
                m_status.changeBit(currentState.accelerate(), eStatus::Accelerate)
                        .changeBit(currentState.accelerate() && !lastState.accelerate(),
                                eStatus::AccelerateStart)
                        .changeBit(currentState.brake(), eStatus::Brake);

                if (m_status.offBit(eStatus::AutoDrift)) {
                    m_status.changeBit(currentState.drift(), eStatus::DriftInput)
                            .changeBit(currentState.drift() && !lastState.drift(),
                                    eStatus::HopStart);
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
        m_status.changeBit(currentState.accelerate(), eStatus::ChargeStartBoost);

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
    m_status.resetBit(eStatus::Accelerate, eStatus::Brake, eStatus::DriftInput, eStatus::HopStart,
            eStatus::AccelerateStart, eStatus::GroundStart, eStatus::StickLeft,
            eStatus::WallCollisionStart, eStatus::AirStart, eStatus::StickRight,
            eStatus::ZipperInvisibleWall, eStatus::JumpPadDisableYsusForce);

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
    bool wasTouchingGround = m_status.onBit(eStatus::TouchingGround);
    bool wasWallCollision = m_status.onBit(eStatus::WallCollision, eStatus::Wall3Collision);

    m_status.resetBit(eStatus::Wall3Collision, eStatus::WallCollision,
            eStatus::VehicleBodyFloorCollision, eStatus::AnyWheelCollision,
            eStatus::AllWheelsCollision, eStatus::TouchingGround);

    if (m_hwgTimer > 0) {
        if (--m_hwgTimer == 0) {
            m_status.resetBit(eStatus::UNK2, eStatus::SomethingWallCollision);
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
                m_status.resetBit(eStatus::SoftWallDrift);
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
        m_status.setBit(eStatus::AnyWheelCollision);
        if (wheelCollisions == tireCount()) {
            m_status.setBit(eStatus::AllWheelsCollision);
        }
    }

    CollisionData &colData = collisionData();
    if (colData.bFloor) {
        m_status.setBit(eStatus::VehicleBodyFloorCollision);
        m_top += colData.floorNrm;
        trickable = trickable || colData.bTrickable;

        if (m_status.onBit(eStatus::OverZipper)) {
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
            status().setBit(eStatus::WallCollision);
        }

        if (colData.bWall3) {
            m_status.setBit(eStatus::Wall3Collision);
        }

        if (!wasWallCollision) {
            m_status.setBit(eStatus::WallCollisionStart);
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

    if (colData.bInvisibleWall && m_status.onBit(eStatus::HalfPipeRamp) &&
            collide()->surfaceFlags().offBit(KartCollide::eSurfaceFlags::StopHalfPipeState)) {
        m_status.setBit(eStatus::ZipperInvisibleWall);
    }

    if (softWallCount > 0 || hwg) {
        m_status.setBit(eStatus::UNK2);
        m_softWallSpeed = wallNrm;
        m_softWallSpeed.normalise();
        if (softWallCount > 0 && m_status.offBit(eStatus::Hop)) {
            m_status.setBit(eStatus::SoftWallDrift);
        }

        if (hwg) {
            m_status.setBit(eStatus::HWG);
        }

        if (hitboxGroupSoftWallCollision || hwg || isBike()) {
            m_status.setBit(eStatus::SomethingWallCollision);
            m_hwgTimer = 10;

            if (hwg) {
                m_hwgTimer *= 2;
            }
        }
    }

    m_status.resetBit(eStatus::AirtimeOver20);
    m_trickableTimer = std::max(0, m_trickableTimer - 1);

    if (wheelCollisions < 1 && !colData.bFloor) {
        if (wasTouchingGround) {
            m_status.setBit(eStatus::AirStart);
        }

        if (++m_airtime > 20) {
            m_status.setBit(eStatus::AirtimeOver20);
        }
    } else {
        m_top.normalise();

        m_status.setBit(eStatus::TouchingGround).resetBit(eStatus::AfterCannon);

        if (m_status.offBit(eStatus::InAction)) {
            m_status.resetBit(eStatus::EndHalfPipe);
        }

        if (m_status.onBit(eStatus::OverZipper)) {
            halfPipe()->end(true);
        }

        if (trickable) {
            m_trickableTimer = 3;
        }

        m_status.changeBit(m_trickableTimer > 0, eStatus::Trickable);

        if (!wasTouchingGround) {
            m_status.setBit(eStatus::GroundStart);
        }

        if (m_status.onBit(eStatus::InATrick) && jump()->cooldown() == 0) {
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

    if (m_status.onBit(eStatus::ChargeStartBoost)) {
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

    if (m_status.onBit(eStatus::Accelerate)) {
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
    m_status.resetBit(eStatus::ChargeStartBoost);
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
    m_status.resetBit(eStatus::HalfPipeRamp, eStatus::RejectRoad);
}

} // namespace Kart
