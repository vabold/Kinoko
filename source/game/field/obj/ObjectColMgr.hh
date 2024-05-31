#pragma once

#include "game/field/CourseColMgr.hh"
#include "game/field/KColData.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

namespace Field {

class ObjectColMgr {
public:
    ObjectColMgr(const char *filename);
    ~ObjectColMgr();

    void setMtx(EGG::Matrix34f &val);
    void setKCLScale(f32 val);

    const EGG::Vector3f &getKclBboxLowWorld() const;
    const EGG::Vector3f &getKclBboxHighWorld() const;

    static void *LoadFile(const char *filename);

private:
    KColData *m_data;
    EGG::Matrix34f m_mtx;
    EGG::Matrix34f m_mtxInv;
    f32 m_kclScale;
    EGG::Vector3f _68;
};

} // namespace Field
