#pragma once
#include <Common.hh>

namespace Field {

struct ObjAttrs {
    u16 id;
    const char name[32];
    const char resources[64];
    u16 soundMode;
    s16 clippingMode;
    s16 backwardsDrawDistance;
    s16 forwardsDrawDistance;
    s16 collisionMode;
    s16 collisionParam1;
    s16 collisionParam2;
    s16 collisionParam3;
    s16 _72;
};
struct _ObjFlow {
    u16 m_count;
    ObjAttrs *m_attrs;
    u16 *m_slots;
};
class ObjFlow : public _ObjFlow {
public:
    ObjFlow(const char *filename);
    virtual ~ObjFlow() = default;

    static void *LoadFile(const char *filename);

    u16 getIdFromName(const char* name) const;
private:
};

} // namespace Field