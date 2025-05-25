#include "DrawMdl.hh"

namespace Render {

/// @addr{0x8055DDEC}
void DrawMdl::linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
        AnmType anmType) {
    if (!m_anmMgr) {
        m_anmMgr = new AnmMgr(this);
    }

    m_anmMgr->linkAnims(idx, resFile, name, anmType);
}

} // namespace Render
