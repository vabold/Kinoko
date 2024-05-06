#include <Common.hh>

namespace Object {

// this class is called ObjectHolder in ghidra, but that's cringe
class GeoObjManager {
public:
    static GeoObjManager *CreateInstance();
    static void DestroyInstance();
    static GeoObjManager *Instance();
    
    GeoObjManager();
    virtual ~GeoObjManager();
private:
    static GeoObjManager *s_instance;
};

} // namespace Object
