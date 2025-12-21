#pragma once

#include "game/field/obj/ObjectFireSnake.hh"

namespace Field {

class ObjectFireSnakeV final : public ObjectFireSnake {
public:
    ObjectFireSnakeV(const System::MapdataGeoObj &params);
    ~ObjectFireSnakeV() override;

    void init() override;
    void calc() override;

private:
    void calcSub();

    /// @addr{0x806C30F0}
    void enterDespawned() {
        ObjectFireSnake::enterDespawned();
    }

    void enterFalling();

    /// @addr{0x806C33B4}
    void enterHighBounce() {}

    void enterRest() {
        ObjectFireSnake::enterRest();
    }

    void enterBounce() {
        ObjectFireSnake::enterBounce();
    }

    void enterDespawning() {
        ObjectFireSnake::enterDespawning();
    }

    /// @addr{0x806C30F4}
    void calcDespawned() {}

    void calcFalling();
    void calcHighBounce();

    void calcRest() {
        ObjectFireSnake::calcRest();
    }

    void calcBounce() {
        ObjectFireSnake::calcBounce();
    }

    void calcDespawning() {
        ObjectFireSnake::calcDespawning();
    }

    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterDespawned,
                    &ObjectFireSnakeV::calcDespawned>(0)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterFalling,
                    &ObjectFireSnakeV::calcFalling>(1)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterHighBounce,
                    &ObjectFireSnakeV::calcHighBounce>(2)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterRest,
                    &ObjectFireSnakeV::calcRest>(3)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterBounce,
                    &ObjectFireSnakeV::calcBounce>(4)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterDespawning,
                    &ObjectFireSnakeV::calcDespawning>(5)},
    }};

    const s16 m_cycleDuration;
    const f32 m_distFromPipe;
    f32 m_fallSpeed;

    static constexpr f32 RADIUS = 130.0f;
    static constexpr f32 GRAVITY = 3.0f;

    /// How many frames the snake falls before starting to check against floor collision
    static constexpr u32 COL_CHECK_DELAY_FRAMES = 10;
};

} // namespace Field
