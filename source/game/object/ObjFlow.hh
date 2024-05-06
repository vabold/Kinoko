#include <Common.hh>

namespace Object {

class ObjFlow {
public:
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

    ObjFlow(const char *filename);
    virtual ~ObjFlow() = default;

    static void *LoadFile(const char *filename);
private:
};

} // namespace Object