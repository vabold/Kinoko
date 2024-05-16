#pragma once
#include <Common.hh>

namespace Field {

struct GeoHitTableHeader {
    u16 m_objectCount;
    u16 m_hitParameterCount;
};

class GeoHitTable {
public:
    GeoHitTable(const char *filename);
    virtual ~GeoHitTable();

    static void *LoadFile(const char *filename);
private:
    // the game uses a cursor member, I used a ramstream instead
    u16 m_objectCount;
    u16 m_hitParameterCount;
    void** m_hitParametersArray;
    void* m_moreData;
};

} // namespace Field