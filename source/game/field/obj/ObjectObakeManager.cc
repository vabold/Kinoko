#include "ObjectObakeManager.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Kinoko::Field {

/// @addr{0x8080B0D8}
ObjectObakeManager::ObjectObakeManager(const System::MapdataGeoObj &params)
    : ObjectDrivable(params), m_blockCache({}), m_blocks(MAX_BLOCKS), m_fallingBlockCount(0) {
    static constexpr f32 BLOCK_WIDTH = 195.00002f;
    static constexpr f32 BLOCK_HEIGHT = 130.0f;

    m_colBox = EGG::egg_new<ObjectCollisionBox>(BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH,
            EGG::Vector3f::zero);
    m_colSphere = EGG::egg_new<ObjectCollisionSphere>(1.0f, EGG::Vector3f::zero);

    addBlock(params);
}

/// @addr{0x8080BEA4}
ObjectObakeManager::~ObjectObakeManager() {
    EGG::egg_delete(m_colBox);
    EGG::egg_delete(m_colSphere);

    for (auto *&block : m_blocks) {
        EGG::egg_delete(block);
    }
}

/// @addr{0x8080BB28}
void ObjectObakeManager::calc() {
    u32 frame = System::RaceManager::Instance()->timer();

    // Kinoko-specific: Since m_blocks is sorted by fall frame, only check the blocks that are next
    // to fall. Blocks never return to FallState::Rest, and each block's calc only affects that
    // block, so this starts the same blocks falling on the same frames as the base game.
    for (; m_fallingBlockCount < m_blocks.size(); ++m_fallingBlockCount) {
        auto *block = m_blocks[m_fallingBlockCount];
        u32 fallFrame = block->fallFrame();

        if (fallFrame == 0 || fallFrame > frame) {
            break;
        }

        // Block is starting to fall
        block->setFallState(ObjectObakeBlock::FallState::Falling);
        block->calc();
    }

    for (size_t i = 0; i < m_fallingBlockCount; ++i) {
        m_blocks[i]->calc();
    }
}

/// @addr{0x8080BE44}
bool ObjectObakeManager::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE54}
bool ObjectObakeManager::checkPointPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE64}
bool ObjectObakeManager::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE74}
bool ObjectObakeManager::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE34}
bool ObjectObakeManager::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 /*timeOffset*/) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE38}
bool ObjectObakeManager::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 /*timeOffset*/) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE3C}
bool ObjectObakeManager::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 /*timeOffset*/) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE40}
bool ObjectObakeManager::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 /*timeOffset*/) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BDF4}
bool ObjectObakeManager::checkPointCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE04}
bool ObjectObakeManager::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE14}
bool ObjectObakeManager::checkPointCachedFull(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BE24}
bool ObjectObakeManager::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BDE4}
bool ObjectObakeManager::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 /*timeOffset*/) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BDE8}
bool ObjectObakeManager::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 /*timeOffset*/) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BDEC}
bool ObjectObakeManager::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 /*timeOffset*/) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080BDF0}
bool ObjectObakeManager::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 /*timeOffset*/) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080B244}
void ObjectObakeManager::addBlock(const System::MapdataGeoObj &params) {
    // Kinoko-specific: Insert the block so that m_blocks stays sorted by fall frame, with blocks
    // that never fall (fall frame 0) last. Blocks with the same fall frame stay in insertion order.
    auto fallsBefore = [](const ObjectObakeBlock *lhs, const ObjectObakeBlock *rhs) {
        u32 lhsFallFrame = lhs->fallFrame();
        u32 rhsFallFrame = rhs->fallFrame();
        return lhsFallFrame != 0 && (rhsFallFrame == 0 || lhsFallFrame < rhsFallFrame);
    };

    auto *block = EGG::egg_new<ObjectObakeBlock>(params);
    m_blocks.push_back(block);
    auto *last = m_blocks.end() - 1;
    std::rotate(std::upper_bound(m_blocks.begin(), last, block, fallsBefore), last, m_blocks.end());

    auto [spatialX, spatialZ] = SpatialIndex(block->pos());
    m_blockCache[spatialZ][spatialX] = block;
}

/// @addr{0x8080BEE4}
bool ObjectObakeManager::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    bool collision = false;
    auto [spatialX, spatialZ] = SpatialIndex(pos);

    EGG::Matrix34f t;
    t.makeT(pos);
    m_colSphere->transform(t, EGG::Vector3f(radius, radius, radius), EGG::Vector3f::zero);

    for (s32 i = spatialZ - 1; i <= spatialZ + 1; ++i) {
        for (s32 j = spatialX - 1; j <= spatialX + 1; ++j) {
            // Make sure we're in bounds of the cache
            if (j < 0 || static_cast<size_t>(j) >= CACHE_SIZE_X || i < 0 ||
                    static_cast<size_t>(i) >= CACHE_SIZE_Z) {
                continue;
            }

            auto *block = m_blockCache[i][j];
            if (!block) {
                continue;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL)) {
                t.makeT(block->pos());
                m_colBox->setBoundingRadius(SPECIAL_WALL_BOUNDING_RADIUS);
                m_colBox->transform(t, SPECIAL_WALL_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.0f > distNrm.y || distNrm.y > 0.9f) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist);
                        }

                        if (maskOut) {
                            *maskOut |= KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL);
                        }
                    }
                }

                collision |= collided;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_ROAD)) {
                t.makeT(block->pos());
                m_colBox->transform(t, ROAD_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.9f >= distNrm.y) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist);
                        }

                        if (maskOut) {
                            *maskOut |= KCL_TYPE_BIT(COL_TYPE_ROAD);
                        }
                    }
                }

                collision |= collided;
            }
        }
    }

    return collision;
}

/// @addr{0x8080C41C}
bool ObjectObakeManager::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    bool collision = false;
    auto [spatialX, spatialZ] = SpatialIndex(pos);

    EGG::Matrix34f t;
    t.makeT(pos);

    m_colSphere->transform(t, EGG::Vector3f(radius, radius, radius), EGG::Vector3f::zero);

    for (s32 i = spatialZ - 1; i <= spatialZ + 1; ++i) {
        for (s32 j = spatialX - 1; j <= spatialX + 1; ++j) {
            // Make sure we're in bounds of the cache
            if (j < 0 || static_cast<size_t>(j) >= CACHE_SIZE_X || i < 0 ||
                    static_cast<size_t>(i) >= CACHE_SIZE_Z) {
                continue;
            }

            auto *block = m_blockCache[i][j];
            if (!block) {
                continue;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL)) {
                t.makeT(block->pos());
                m_colBox->setBoundingRadius(SPECIAL_WALL_BOUNDING_RADIUS);
                m_colBox->transform(t, SPECIAL_WALL_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.0f > distNrm.y || distNrm.y > 0.9f) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist);
                        }

                        if (maskOut) {
                            auto *colDir = CollisionDirector::Instance();
                            colDir->pushCollisionEntry(dist.length(), maskOut,
                                    KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL), COL_TYPE_SPECIAL_WALL);
                            colDir->setCurrentCollisionVariant(2);
                        }
                    }
                }

                collision |= collided;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_ROAD)) {
                t.makeT(block->pos());
                m_colBox->transform(t, ROAD_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.9f >= distNrm.y) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist);
                        }

                        if (maskOut) {
                            auto *colDir = CollisionDirector::Instance();
                            colDir->pushCollisionEntry(dist.length(), maskOut,
                                    KCL_TYPE_BIT(COL_TYPE_ROAD), COL_TYPE_ROAD);
                        }
                    }
                }

                collision |= collided;
            }
        }
    }

    return collision;
}

/// @addr{0x8080C980}
bool ObjectObakeManager::checkSphereFullImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfo *info,
        KCLTypeMask *maskOut) {
    bool collision = false;
    auto [spatialX, spatialZ] = SpatialIndex(pos);

    EGG::Matrix34f t;
    t.makeT(pos);
    m_colSphere->transform(t, EGG::Vector3f(radius, radius, radius), EGG::Vector3f::zero);

    for (s32 i = spatialZ - 1; i <= spatialZ + 1; ++i) {
        for (s32 j = spatialX - 1; j <= spatialX + 1; ++j) {
            // Make sure we're in bounds of the cache
            if (j < 0 || static_cast<size_t>(j) >= CACHE_SIZE_X || i < 0 ||
                    static_cast<size_t>(i) >= CACHE_SIZE_Z) {
                continue;
            }

            auto *block = m_blockCache[i][j];
            if (!block) {
                continue;
            }

            // Bonking on top of block
            if (mask & KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL)) {
                t.makeT(block->pos());
                m_colBox->setBoundingRadius(SPECIAL_WALL_BOUNDING_RADIUS);
                m_colBox->transform(t, SPECIAL_WALL_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.0f > distNrm.y || distNrm.y > 0.9f) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist.length(), dist, distNrm, KCL_TYPE_WALL);
                        }

                        if (maskOut) {
                            *maskOut |= KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL);
                        }
                    }
                }

                collision |= collided;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_ROAD)) {
                t.makeT(block->pos());
                m_colBox->transform(t, ROAD_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.9f >= distNrm.y) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist.length(), dist, distNrm, KCL_TYPE_FLOOR);
                        }

                        if (maskOut) {
                            *maskOut |= KCL_TYPE_BIT(COL_TYPE_ROAD);
                        }
                    }
                }

                collision |= collided;
            }
        }
    }

    return collision;
}

/// @addr{0x8080D12C}
bool ObjectObakeManager::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfo *info,
        KCLTypeMask *maskOut) {
    bool collision = false;
    auto [spatialX, spatialZ] = SpatialIndex(pos);

    EGG::Matrix34f t;
    t.makeT(pos);
    m_colSphere->transform(t, EGG::Vector3f(radius, radius, radius), EGG::Vector3f::zero);

    for (s32 i = spatialZ - 1; i <= spatialZ + 1; ++i) {
        for (s32 j = spatialX - 1; j <= spatialX + 1; ++j) {
            // Make sure we're in bounds of the cache
            if (j < 0 || static_cast<size_t>(j) >= CACHE_SIZE_X || i < 0 ||
                    static_cast<size_t>(i) >= CACHE_SIZE_Z) {
                continue;
            }

            auto *block = m_blockCache[i][j];
            if (!block) {
                continue;
            }

            // Bonking on top of block
            if (mask & KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL)) {
                t.makeT(block->pos());
                m_colBox->setBoundingRadius(SPECIAL_WALL_BOUNDING_RADIUS);
                m_colBox->transform(t, SPECIAL_WALL_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.0f > distNrm.y || distNrm.y > 0.9f) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist.length(), dist, distNrm, KCL_TYPE_WALL);
                        }

                        if (maskOut) {
                            auto *colDir = CollisionDirector::Instance();
                            colDir->pushCollisionEntry(dist.length(), maskOut,
                                    KCL_TYPE_BIT(COL_TYPE_SPECIAL_WALL), COL_TYPE_SPECIAL_WALL);
                            colDir->setCurrentCollisionVariant(2);
                        }
                    }
                }

                collision |= collided;
            }

            if (mask & KCL_TYPE_BIT(COL_TYPE_ROAD)) {
                t.makeT(block->pos());
                m_colBox->transform(t, ROAD_SCALE, EGG::Vector3f::zero);

                EGG::Vector3f dist;
                bool collided = m_colSphere->check(*m_colBox, dist);

                if (collided) {
                    EGG::Vector3f distNrm = dist;
                    distNrm.normalise();

                    if (0.9f >= distNrm.y) {
                        collided = false;
                    } else {
                        if (info) {
                            info->update(dist.length(), dist, distNrm, KCL_TYPE_FLOOR);
                        }

                        if (maskOut) {
                            auto *colDir = CollisionDirector::Instance();
                            colDir->pushCollisionEntry(dist.length(), maskOut,
                                    KCL_TYPE_BIT(COL_TYPE_ROAD), COL_TYPE_ROAD);
                        }
                    }
                }

                collision |= collided;
            }
        }
    }

    return collision;
}

/// @brief Helper function to return the spatial index of a given block
std::pair<s32, s32> ObjectObakeManager::SpatialIndex(const EGG::Vector3f &pos) {
    constexpr f32 ORIGIN_OFFSET_X = -30647.498f;
    constexpr f32 ORIGIN_OFFSET_Z = -21092.5f;
    constexpr f32 GRID_WIDTH = 325.0f; // The "width" of each cell in the spatial grid
    constexpr f32 GRID_HALF_WIDTH = 162.5f;

    s32 x = (pos.x - ORIGIN_OFFSET_X + GRID_HALF_WIDTH) / GRID_WIDTH;
    s32 z = (pos.z - ORIGIN_OFFSET_Z + GRID_HALF_WIDTH) / GRID_WIDTH;

    return std::make_pair(x, z);
}

} // namespace Kinoko::Field
