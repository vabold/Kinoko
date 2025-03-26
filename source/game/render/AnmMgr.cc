#include "AnmMgr.hh"

namespace Render {

/// @addr{0x8055597C}
void AnmMgr::linkAnims(size_t idx, const Abstract::g3d::ResFile *resFile, const char *name,
        AnmType anmType) {
    // For now, we only care about Chr
    switch (anmType) {
    case AnmType::Chr:
        m_anmList.emplace_back(resFile->resAnmChr(name), anmType, idx);
        break;
    default:
        break;
    }
}

/// @addr{0x805573CC}
void AnmMgr::playAnim(f32 /*frame*/, f32 /*rate*/, size_t idx) {
    ASSERT(idx < m_anmList.size());
    AnmNodeChr *&activeChrAnim = m_activeAnims[static_cast<size_t>(AnmType::Chr)];
    activeChrAnim = &(*std::next(m_anmList.begin(), idx));
}

} // namespace Render
