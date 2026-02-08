#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

/// @brief Smallest convex shape that encloses a given set of points.
class ObjectCollisionConvexHull : public ObjectCollisionBase {
public:
    ObjectCollisionConvexHull(const std::span<const EGG::Vector3f> &points);
    ~ObjectCollisionConvexHull() override;

    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale) override;
    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) override;
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override;

    /// @addr{0x807F957C}
    [[nodiscard]] f32 getBoundingRadius() const override {
        return m_worldRadius;
    }

    /// @addr{0x8081E254}
    [[nodiscard]] f32 initRadius() const {
        return m_initRadius;
    }

    /// @addr{0x8080C414}
    virtual void setBoundingRadius(f32 val) {
        m_worldRadius = val;
    }

protected:
    ObjectCollisionConvexHull(size_t count);

    std::span<EGG::Vector3f> m_points;

private:
    const f32 m_initRadius;
    std::span<EGG::Vector3f> m_worldPoints;
    f32 m_worldRadius;
};

} // namespace Field
