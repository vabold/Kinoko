#include "ObjectColMgr.hh"

#include "game/system/ResourceManager.hh"

namespace Field {

void *ObjectColMgr::LoadFile(const char *filename) {
    auto *resMgr = System::ResourceManager::Instance();
    return resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
}

/// @addr{0x807C4CE8}
/// @details In the base game, this file is loaded in each GeoObjectKCL and passed into
/// this function. It's simpler to just keep it here.
ObjectColMgr::ObjectColMgr(const char *filename)
    : m_mtx(EGG::Matrix34f::ident), m_mtxInv(EGG::Matrix34f::ident), m_kclScale(1.0f),
      _68(EGG::Vector3f::zero) {
    void *file = LoadFile(filename);
    m_data = new KColData(file);
}

/// @addr{0x807C4D6C}
ObjectColMgr::~ObjectColMgr() {
    delete m_data;
}

void ObjectColMgr::setMtx(EGG::Matrix34f &val) {
    m_mtx = val;
    m_mtxInv = val.inverseTo33();
}
void ObjectColMgr::setKCLScale(f32 val) {
    m_kclScale = val;
}

/// @addr{0x807C4E4C}
const EGG::Vector3f &ObjectColMgr::getKclBboxLowWorld() const {
    EGG::Vector3f out = m_data->bbox().min * m_kclScale;
    return m_mtx.ps_multVector(out);
}

/// @addr{0x807C4E7C}
const EGG::Vector3f &ObjectColMgr::getKclBboxHighWorld() const {
    EGG::Vector3f out = m_data->bbox().max * m_kclScale;
    return m_mtx.ps_multVector(out);
}

} // namespace Field
