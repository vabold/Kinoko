#include "ObjectColMgr.hh"

#include "game/system/ResourceManager.hh"

namespace Field {

void *ObjectColMgr::LoadFile(const char *filename) {
    auto *resMgr = System::ResourceManager::Instance();
    return resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
}

ObjectColMgr::ObjectColMgr(const char *filename) : m_kclScale(1.0f) {
    // In the base game, this file is loaded in each GeoObjectKCL and passed into
    // this function. It's simpler to just keep it here.
    void *file = LoadFile(filename);
    m_data = new KColData(file);
    m_mtx = EGG::Matrix34f::ident;
    m_mtxInv = EGG::Matrix34f::ident;
    _68 = EGG::Vector3f::zero;
}

ObjectColMgr::~ObjectColMgr() {
    delete m_data;
}

EGG::Vector3f ObjectColMgr::getKclBboxLowWorld() {
    EGG::Vector3f out = m_data->bbox().min * m_kclScale;
    return m_mtx.ps_multVector(out);
}
EGG::Vector3f ObjectColMgr::getKclBboxHighWorld() {
    EGG::Vector3f out = m_data->bbox().max * m_kclScale;
    return m_mtx.ps_multVector(out);
}

} // namespace Field