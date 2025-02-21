#pragma once

#include <egg/math/Matrix.hh>

namespace Field {

/// @brief Houses the state of an object in terms of the Gilbert–Johnson–Keerthi (GJK) algorithm.
struct GJKState {
    GJKState() : m_flags(0), m_idx(0), m_mask(0), m_00c(0), m_scales{{}} {}

    u32 m_flags;
    u32 m_idx;
    u32 m_mask;
    s32 m_00c;
    std::array<EGG::Vector3f, 4> m_s;
    std::array<EGG::Vector3f, 4> m_support1;
    std::array<EGG::Vector3f, 4> m_support2;
    std::array<std::array<f32, 4>, 16> m_scales;
};

/// @brief The base class that all objects' collision inherits from.
/// @details Implementation is done via the GJK distance algorithm.
/// https://en.wikipedia.org/wiki/Gilbert-Johnson-Keerthi_distance_algorithm
class ObjectCollisionBase {
public:
    ObjectCollisionBase();
    virtual ~ObjectCollisionBase();

    virtual void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) = 0;
    virtual const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const = 0;
    virtual f32 getBoundingRadius() const = 0;

    bool check(ObjectCollisionBase &rhs, EGG::Vector3f &distance);

protected:
    EGG::Vector3f m_translation;

private:
    bool enclosesOrigin(const GJKState &state, u32 idx) const;
    void FUN_808350e4(GJKState &state, EGG::Vector3f &v) const;
    bool getNearestSimplex(GJKState &state, EGG::Vector3f &v) const;
    void getNearestPoint(GJKState &state, u32 idx, EGG::Vector3f &v0, EGG::Vector3f &v1) const;
    bool FUN_808357e4(const GJKState &state, u32 idx) const;
    bool inSimplex(const GJKState &state, const EGG::Vector3f &v) const;
    void getNearestPoint(const GJKState &state, u32 idx, EGG::Vector3f &v) const;
    void calcSimplex(GJKState &state) const;

    EGG::Vector3f m_00;

    static std::array<std::array<f32, 4>, 4> s_dotProductCache;
};

} // namespace Field
