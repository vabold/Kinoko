#pragma once

#include "game/render/AnmMgr.hh"

namespace Render {

class DrawMdl {
public:
    DrawMdl() : m_anmMgr(nullptr) {}

    /// @addr{0x8055DDEC}
    void linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
            AnmType anmType) {
        if (!m_anmMgr) {
            m_anmMgr = new AnmMgr(this);
        }

        m_anmMgr->linkAnims(idx, resFile, name, anmType);
    }

    [[nodiscard]] AnmMgr *anmMgr() {
        return m_anmMgr;
    }

private:
    AnmMgr *m_anmMgr;
};

} // namespace Render
