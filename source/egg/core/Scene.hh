#pragma once

#include <Common.hh>

namespace EGG {

class SceneManager;

/// @brief Base class for all scenes.
/// @details A scene compartmentalizes certain elements of the game,
/// such as the menu, the race, the credits, etc.
/// This allows for easy cleanup when transitioning to a different scene,
/// as scenes destroy instances of classes used only during the scene lifetime.
/// While this is not an abstract class, you'll never see an instance of this base class.
/// @nosubgrouping
class Scene : Disposer {
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

    /// @beginSetters
    void setParent(Scene *parent);
    void setChild(Scene *child);
    void setId(int id);
    void setSceneMgr(SceneManager *sceneMgr);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] Heap *heap() const;
    [[nodiscard]] Scene *parent() const;
    [[nodiscard]] Scene *child() const;
    [[nodiscard]] int id() const;
    [[nodiscard]] SceneManager *sceneMgr() const;
    /// @endGetters

protected:
    /*-----------*
        Members
     *-----------*/

    Heap *m_heap;
    Scene *m_parent;
    Scene *m_child;
    int m_id;
    SceneManager *m_sceneMgr;
};

} // namespace EGG
