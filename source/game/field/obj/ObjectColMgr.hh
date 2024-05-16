#pragma once

#include "game/field/CourseColMgr.hh"
#include "game/field/KColData.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

namespace Field {

class ObjectColMgr {
public:
    static void *LoadFile(const char *filename);
    ObjectColMgr(const char *filename);
    ~ObjectColMgr();

    EGG::Vector3f getKclBboxLowWorld();
    EGG::Vector3f getKclBboxHighWorld();
private:
    KColData *m_data;
    EGG::Matrix34f m_mtx;
    EGG::Matrix34f m_mtxInv;
    f32 m_kclScale;
    EGG::Vector3f _68;
};

} // namespace Field