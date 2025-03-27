#pragma once

#include "game/system/ResourceManager.hh"

#include <abstract/g3d/ResFile.hh>

#include <list>

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

class AnmNode {
public:
    AnmNode(Abstract::g3d::AnmObjChrRes anmObjChrRes, AnmType anmType, size_t idx)
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

    /// @addr{0x8055597C}
    void linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
            AnmType anmType) {
        // For now, we only care about Chr
        if (anmType == AnmType::Chr) {
            m_anmList.emplace_back(resFile->resAnmChr(name), anmType, idx);
        }
    }

    /// @addr{0x805573CC}
    void playAnim(f32 frame, f32 rate, size_t idx) {
        ASSERT(idx < m_anmList.size());
        AnmNode *&activeChrAnim = m_activeAnims[static_cast<size_t>(AnmType::Chr)];
        activeChrAnim = &(*std::next(m_anmList.begin(), idx));
    }

    /// @addr{0x80557340}
    [[nodiscard]] const AnmNode *activeAnim(AnmType anmType) const {
        return m_activeAnims[static_cast<size_t>(anmType)];
    }

private:
    DrawMdl *m_parent;
    std::list<AnmNode> m_anmList;
    std::array<AnmNode *, static_cast<size_t>(AnmType::Max) - 1> m_activeAnims;
};

} // namespace Render
