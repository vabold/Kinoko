#include "game/field/obj/ObjectKCL.hh"

namespace Field {

class ObjectTownBridge final : public ObjectKCL {
public:
    enum class State {
        Raising = 0,
        Raised = 1,
        Lowering = 2,
        Lowered = 3,
    };

    ObjectTownBridge(const System::MapdataGeoObj &params);
    ~ObjectTownBridge() override;

    void calc() override;

    /// @addr{0x8080ACD8}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void createCollision() override;

    /// @addr{0x8080A8D0}
    [[nodiscard]] f32 colRadiusAdditionalLength() const override {
        return 500.0f;
    }

private:
    [[nodiscard]] f32 calcBridgeAngle(u32 t) const;
    [[nodiscard]] State calcState(u32 t) const;

    bool m_rotateUpwards; ///< Normally 1, otherwise the bridge will open downwards.
    f32 m_angVel;         ///< Speed of the bridge's movement.
    u32 m_pivotFrames;    ///< # of frames the bridge pivots up or down.
    u32 m_raisedFrames;   ///< # of frames the bridge remains raised.
    u32 m_loweredFrames;  ///< # of frames the bridge remains lowered.
    u32 m_fullAnimFrames; ///< The full duration of a bridge raise/lower loop.
    State m_state;
    ObjColMgr *m_raisedColMgr;
    ObjColMgr *m_midColMgr;
    ObjColMgr *m_flatColMgr;
};

} // namespace Field
