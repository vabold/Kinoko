#pragma once
#include <Common.hh>

namespace Field {

/// @brief 
struct GeoHitTableHeader {
    u16 m_objectCount;
    u16 m_hitParameterCount;
};

/// @brief Parser and holder for player-object/item-object parameter tables stored in files in Common.szs
/// @details There are 4 files in Common.szs that store interaction parameters for collisions between objects and players or items.
/// GeoHitTableKart.bin : player-object collision effects on players (e.g. colliding with a cactus makes the player spin out)
/// GeoHitTableKartObj.bin : player-object collision effects on objects (e.g. colliding with a crate in DS Delfino Square destroys the crate)
/// GeoHitTableItem.bin : item-object collision effects on items (e.g. a green shell colliding with a pipe in SNES Mario Circuit 3 will bounce off of it as if it were a wall)
/// GeoHitTableItemObj.bin : item-object collision effects on objects (e.g. a green shell colliding with a goomba with squish it and drop a mushroom)
///
/// Note: in the game, there are 4 derived classes for these tables. However, they are all the same size and have no virtual functions besides a default destructor.
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