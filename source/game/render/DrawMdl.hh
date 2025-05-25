#pragma once

#include "game/render/AnmMgr.hh"

namespace Render {

class DrawMdl {
public:
    DrawMdl() : m_anmMgr(nullptr) {}

    void linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
            AnmType anmType);

    [[nodiscard]] AnmMgr *anmMgr() {
        return m_anmMgr;
    }

private:
    AnmMgr *m_anmMgr;
};

} // namespace Render
