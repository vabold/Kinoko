#pragma once

#include "game/system/ResourceManager.hh"

#include <abstract/g3d/ResFile.hh>

namespace Render {

class DrawMdl;

enum class AnmType : s32 {
    Empty = -1,
    Chr = 0,
    Clr = 1,
    Srt = 2,
    Pat = 3,
    Shp = 4,
    Max = 5,
};

class AnmNodeChr {
public:
    AnmNodeChr(Abstract::g3d::AnmObjChrRes anmObjChrRes, AnmType anmType, size_t idx)
        : m_anmObjChrRes(anmObjChrRes), m_anmType(anmType), m_idx(idx) {}

    [[nodiscard]] f32 frameCount() const {
        return static_cast<f32>(m_anmObjChrRes.frameCount());
    }

private:
    Abstract::g3d::AnmObjChrRes m_anmObjChrRes;
    AnmType m_anmType;
    size_t m_idx;
};

class AnmMgr {
public:
    /// @addr{0x80555750}
    AnmMgr(DrawMdl *drawMdl) : m_parent(drawMdl) {}

    void linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
            AnmType anmType);
    void playAnim(f32 frame, f32 rate, size_t idx);

    /// @addr{0x80557340}
    [[nodiscard]] const AnmNodeChr *activeAnim(AnmType anmType) const {
        return m_activeAnims[static_cast<size_t>(anmType)];
    }

private:
    DrawMdl *m_parent;
    std::list<AnmNodeChr> m_anmList;
    std::array<AnmNodeChr *, static_cast<size_t>(AnmType::Max) - 1> m_activeAnims;
};

} // namespace Render
