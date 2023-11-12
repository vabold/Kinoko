#pragma once

#include <Common.hh>

namespace EGG {

class SceneManager;

class Scene {
public:
    /*------------*
        Lifecycle
     *------------*/

    Scene();
    virtual ~Scene();

    /*----------*
        Virtual
     *----------*/

    virtual void calc() {}
    virtual void enter() {}
    virtual void exit() {}
    virtual void reinit() {}
    virtual void incoming_childDestroy() {}
    virtual void outgoing_childCreate() {}

    /*-----------*
        Getters
     *-----------*/

    Scene *parent() const {
        return m_parent;
    }

    Scene *child() const {
        return m_child;
    }

    int id() const {
        return m_id;
    }

    SceneManager *sceneMgr() const {
        return m_sceneMgr;
    }

    /*-----------*
        Setters
     *-----------*/

    void setParent(Scene *parent) {
        m_parent = parent;
    }

    void setChild(Scene *child) {
        m_child = child;
    }

    void setId(int id) {
        m_id = id;
    }

    void setSceneMgr(SceneManager *sceneMgr) {
        m_sceneMgr = sceneMgr;
    }

protected:
    /*-----------*
        Members
     *-----------*/

    Scene *m_parent;
    Scene *m_child;
    int m_id;
    SceneManager *m_sceneMgr;
};

} // namespace EGG
