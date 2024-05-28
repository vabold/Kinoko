#pragma once
#include <Common.hh>

namespace Field {

class ObjAttrs {
public:
    u16 id() const {
        return parse<u16>(m_id);
    }
    const char* name() const {
        return m_name;
    }
    const char* resources() const {
        return m_resources;
    }
    u16 soundMode() const {
        return parse<u16>(m_soundMode);
    }
    s16 clippingMode() const {
        return parse<s16>(m_clippingMode);
    }
    s16 backwardsDrawDistance() const {
        return parse<s16>(m_backwardsDrawDistance);
    }
    s16 forwardsDrawDistance() const {
        return parse<s16>(m_forwardsDrawDistance);
    }
    s16 collisionMode() const {
        return parse<s16>(m_collisionMode);
    }
    s16 collisionParam1() const {
        return parse<s16>(m_collisionParam1);
    }
    s16 collisionParam2() const {
        return parse<s16>(m_collisionParam2);
    }
    s16 collisionParam3() const {
        return parse<s16>(m_collisionParam3);
    }
    s16 _72() const {
        return parse<s16>(m__72);
    }
private:
    u16 m_id;
    const char m_name[32];
    const char m_resources[64];
    u16 m_soundMode;
    s16 m_clippingMode;
    s16 m_backwardsDrawDistance;
    s16 m_forwardsDrawDistance;
    s16 m_collisionMode;
    s16 m_collisionParam1;
    s16 m_collisionParam2;
    s16 m_collisionParam3;
    s16 m__72;
};
class ObjFlow {
public:
    ObjFlow(const char *filename);
    virtual ~ObjFlow() = default;

    static void *LoadFile(const char *filename);

    u16 count() const;
    ObjAttrs* attrs(u16 idx) const;
    u16 slots(u16 idx) const;

    u16 getIdFromName(const char* name) const;
private:
    u16 m_count;
    ObjAttrs *m_rawAttrs;
    u16 *m_rawSlots;
};

} // namespace Field