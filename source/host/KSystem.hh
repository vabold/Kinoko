#include <Common.hh>

class KSystem : EGG::Disposer {
public:
    virtual ~KSystem() {}

    virtual void init() = 0;
    virtual void calc() = 0;
    virtual bool run() = 0;
    virtual void parseOptions(int argc, char **argv) = 0;

protected:
    static KSystem *s_instance;
};
