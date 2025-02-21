#include "ObjectCollisionBase.hh"

#include <egg/math/Math.hh>

#include <cmath>

namespace Field {

ObjectCollisionBase::ObjectCollisionBase() = default;

ObjectCollisionBase::~ObjectCollisionBase() = default;

/// @addr{0x80834348}
bool ObjectCollisionBase::check(ObjectCollisionBase &rhs, EGG::Vector3f &distance) {
    constexpr f32 INITIAL_MAX_VALUE = std::sqrt(std::numeric_limits<f32>::max());
    STATIC_ASSERT(std::bit_cast<u32>(INITIAL_MAX_VALUE) == 0x5f7fffff);

    f32 rad = getBoundingRadius() + rhs.getBoundingRadius();
    f32 sqDist = rad * rad;
    f32 max = INITIAL_MAX_VALUE;
    f32 lastRadius = 0.0f;

    EGG::Vector3f D = EGG::Vector3f::zero;
    EGG::Vector3f v0;
    EGG::Vector3f v1;
    GJKState state;

    do {
        for (state.m_idx = 0, state.m_mask = 1; state.m_flags & state.m_mask; state.m_mask *= 2) {
            ++state.m_idx;
        }

        state.m_support1[state.m_idx] = getSupport(-D);
        state.m_support2[state.m_idx] = rhs.getSupport(D);

        EGG::Vector3f A = state.m_support1[state.m_idx] - state.m_support2[state.m_idx];
        f32 max2 = max * max;

        f32 dot = D.dot(A);
        if (dot > 0.0f && dot * dot > sqDist * max2) {
            return false;
        }

        lastRadius = std::max(lastRadius, dot / max);

        if (inSimplex(state, A) || (max2 - dot) < max2 * 0.000001f) {
            getNearestPoint(state, state.m_flags, v0, v1);

            v0 -= D * (getBoundingRadius() / max);
            v1 += D * (rhs.getBoundingRadius() / max);

            distance = v1 - v0;

            m_00 = v0;
            rhs.m_00 = v1;

            return true;
        }

        state.m_s[state.m_idx] = A;
        state.m_00c = state.m_flags | state.m_mask;

        if (!getNearestSimplex(state, D)) {
            getNearestPoint(state, state.m_flags, v0, v1);

            v0 -= D * (getBoundingRadius() / max);
            v1 += D * (rhs.getBoundingRadius() / max);

            distance = v1 - v0;

            m_00 = v0;
            rhs.m_00 = v1;

            return true;
        }

        max = D.length();

        if (max2 - max * max <= std::numeric_limits<f32>::epsilon() * max2) {
            FUN_808350e4(state, D);
            getNearestPoint(state, state.m_flags, v0, v1);
            f32 len = D.length();
            v0 -= D * (getBoundingRadius() / len);
            v1 += D * (rhs.getBoundingRadius() / len);

            distance = v1 - v0;

            m_00 = v1;
            rhs.m_00 = v1;

            return true;
        }

    } while (state.m_flags < 0xf && (max > std::numeric_limits<f32>::epsilon()));

    return false;
}

/// @addr{0x8083504C}
bool ObjectCollisionBase::enclosesOrigin(const GJKState &state, u32 idx) const {
    u32 mask = 1;
    for (u8 i = 0; i < 4; ++i, mask *= 2) {
        if ((idx & mask) && state.m_scales[idx][i] <= 0.0f) {
            return false;
        }
    }

    return true;
}

/// @addr{0x808350E4}
void ObjectCollisionBase::FUN_808350e4(GJKState &state, EGG::Vector3f &v) const {
    f32 min = std::numeric_limits<f32>::max();

    for (u32 mask = state.m_00c; mask != 0; --mask) {
        if (mask != (mask & state.m_00c) || !enclosesOrigin(state, mask)) {
            continue;
        }

        f32 sqLen = 0.0f;
        EGG::Vector3f tmp = EGG::Vector3f::zero;
        getNearestPoint(state, mask, tmp);

        sqLen = tmp.squaredLength();
        if (sqLen < min) {
            state.m_flags = mask;
            v = tmp;
            min = sqLen;
        }
    }
}

/// @addr{0x80835304}
bool ObjectCollisionBase::getNearestSimplex(GJKState &state, EGG::Vector3f &v) const {
    calcSimplex(state);

    for (u32 i = state.m_flags; i != 0; --i) {
        if (i != (i & state.m_flags) || !FUN_808357e4(state, i | state.m_mask)) {
            continue;
        }

        state.m_flags = i | state.m_mask;
        getNearestPoint(state, state.m_flags, v);
        return true;
    }

    if (FUN_808357e4(state, state.m_mask)) {
        state.m_flags = state.m_mask;
        v = state.m_s[state.m_idx];

        return true;
    }

    return false;
}

/// @addr{0x80835650}
void ObjectCollisionBase::getNearestPoint(GJKState &state, u32 idx, EGG::Vector3f &v0,
        EGG::Vector3f &v1) const {
    v0.setZero();
    v1.setZero();
    f32 sum = 0.0f;

    for (u32 i = 0, mask = 1; i < 4; ++i, mask *= 2) {
        if ((idx & mask) == 0) {
            continue;
        }

        sum += state.m_scales[idx][i];
        v0 += state.m_support1[i] * state.m_scales[idx][i];
        v1 += state.m_support2[i] * state.m_scales[idx][i];
    }

    v0 *= 1.0f / sum;
    v1 *= 1.0f / sum;
}

/// @addr{0x808357E4} @addr{0x808359A4}
bool ObjectCollisionBase::FUN_808357e4(const GJKState &state, u32 idx) const {
    for (u32 i = 0, mask = 1; i < 4; ++i, mask *= 2) {
        if ((state.m_00c & mask) == 0) {
            continue;
        }

        if (idx & mask) {
            if (state.m_scales[idx][i] <= 0.0f) {
                return false;
            }
        } else if (state.m_scales[idx | mask][i] > 0.0f) {
            return false;
        }
    }

    return true;
}

/// @addr{0x808358CC}
bool ObjectCollisionBase::inSimplex(const GJKState &state, const EGG::Vector3f &v) const {
    for (u32 i = 0, mask = 1; i < 4; ++i, mask *= 2) {
        if ((state.m_00c & mask) && state.m_s[i] == v) {
            return true;
        }
    }

    return false;
}

/// @addr{0x80835F34}
void ObjectCollisionBase::getNearestPoint(const GJKState &state, u32 idx, EGG::Vector3f &v) const {
    v.setZero();

    f32 sum = 0.0f;

    for (u32 i = 0, mask = 1; i < 4; ++i, mask *= 2) {
        if ((idx & mask) == 0) {
            continue;
        }

        sum += state.m_scales[idx][i];
        v += state.m_s[i] * state.m_scales[idx][i];
    }

    v *= 1.0f / sum;
}

/// @addr{0x80835A8C}
void ObjectCollisionBase::calcSimplex(GJKState &state) const {
    const u32 idx = state.m_idx;

    for (u32 i = 0, mask = 1; i < 4; ++i, mask *= 2) {
        if ((state.m_flags & mask) == 0) {
            continue;
        }

        f32 result = state.m_s[i].dot(state.m_s[idx]);
        s_dotProductCache[idx][i] = result;
        s_dotProductCache[i][idx] = result;
    }

    state.m_scales[state.m_mask][idx] = 1.0f;
    s_dotProductCache[idx][idx] = state.m_s[idx].squaredLength();

    for (u32 i = 0, iMask = 1; i < 4; ++i, iMask *= 2) {
        if ((state.m_flags & iMask) == 0) {
            continue;
        }

        u32 iStateMask = iMask | state.m_mask;

        state.m_scales[iStateMask][i] = s_dotProductCache[idx][idx] - s_dotProductCache[idx][i];
        state.m_scales[iStateMask][idx] = s_dotProductCache[i][i] - s_dotProductCache[i][idx];

        for (u32 j = 0, jMask = 1; j < i; ++j, jMask *= 2) {
            if ((state.m_flags & jMask) == 0) {
                continue;
            }

            state.m_scales[jMask | iStateMask][j] = state.m_scales[iStateMask][i] *
                            (s_dotProductCache[i][i] - s_dotProductCache[i][j]) +
                    state.m_scales[iStateMask][idx] *
                            (s_dotProductCache[idx][i] - s_dotProductCache[idx][j]);
            state.m_scales[jMask | iStateMask][i] = state.m_scales[jMask | state.m_mask][j] *
                            (s_dotProductCache[j][j] - s_dotProductCache[i][j]) +
                    state.m_scales[jMask | state.m_mask][idx] *
                            (s_dotProductCache[idx][j] - s_dotProductCache[idx][i]);
            state.m_scales[jMask | iStateMask][idx] = state.m_scales[jMask | iMask][j] *
                            (s_dotProductCache[j][j] - s_dotProductCache[j][idx]) +
                    state.m_scales[jMask | iMask][i] *
                            (s_dotProductCache[i][j] - s_dotProductCache[i][idx]);
        }
    }

    if (state.m_00c != 0xf) {
        return;
    }

    f32 _1_1 = s_dotProductCache[0][0] - s_dotProductCache[0][1];
    f32 _2_2 = s_dotProductCache[0][0] - s_dotProductCache[0][2];
    f32 _3_2 = s_dotProductCache[0][0] - s_dotProductCache[0][3];
    f32 _0_2 = s_dotProductCache[1][1] - s_dotProductCache[1][0];
    f32 _0_3 = s_dotProductCache[2][1] - s_dotProductCache[2][0];
    f32 _1_2 = s_dotProductCache[3][0] - s_dotProductCache[3][1];
    f32 _1_3 = s_dotProductCache[2][0] - s_dotProductCache[2][1];
    f32 _2_1 = s_dotProductCache[3][0] - s_dotProductCache[3][2];
    f32 _0_1 = s_dotProductCache[3][1] - s_dotProductCache[3][0];
    f32 _2_3 = s_dotProductCache[1][0] - s_dotProductCache[1][2];
    f32 _3_1 = s_dotProductCache[2][0] - s_dotProductCache[2][3];
    f32 _3_3 = s_dotProductCache[1][0] - s_dotProductCache[1][3];

    state.m_scales[15][0] = _0_1 * state.m_scales[14][3] +
            (_0_2 * state.m_scales[14][1] + _0_3 * state.m_scales[14][2]);
    state.m_scales[15][1] = _1_2 * state.m_scales[13][3] +
            (_1_1 * state.m_scales[13][0] + _1_3 * state.m_scales[13][2]);
    state.m_scales[15][2] = _2_1 * state.m_scales[11][3] +
            (_2_2 * state.m_scales[11][0] + _2_3 * state.m_scales[11][1]);
    state.m_scales[15][3] = _3_1 * state.m_scales[7][2] +
            (_3_2 * state.m_scales[7][0] + _3_3 * state.m_scales[7][1]);
}

std::array<std::array<f32, 4>, 4> ObjectCollisionBase::s_dotProductCache = {{}};

} // namespace Field
