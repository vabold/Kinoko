#pragma once

#include <Common.hh>

class HostSystem {
public:
    void initControllers();

    void create();
    void init();
    void calc();

    s8 controllerCount() const;
    s8 ghostCount() const;
    Character character(s8 controllerId) const;
    Vehicle vehicle(s8 controllerId) const;
    Course course() const;

    static HostSystem *CreateInstance(s8 controllerCount, s8 ghostCount);
    static void DestroyInstance();
    static HostSystem *Instance();

private:
    HostSystem(s8 controllerCount, s8 ghostCount);
    ~HostSystem();

    Vehicle *m_vehicles;
    Character *m_characters;
    Course m_course;
    s8 m_controllerCount;
    s8 m_ghostCount;

    static HostSystem *s_instance;
};
