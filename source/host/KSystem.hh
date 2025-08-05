#pragma once

#include <Common.hh>

class SavestateManager;

/// @brief Base interface for a Kinoko system.
class KSystem : EGG::Disposer {
    friend class SavestateManager;

public:
    virtual ~KSystem() {}

    /// @brief Initializes the system.
    virtual void init() = 0;

    /// @brief Executes a frame.
    virtual void calc() = 0;

    /// @brief Executes a run.
    /// @return Whether the run was successful or not.
    virtual bool run() = 0;

    /// @brief Parses non-generic command line options.
    /// @details It is recommended to not pass the executable, mode flag, and mode argument.
    /// @param argc The number of arguments.
    /// @param argv The arguments.
    virtual void parseOptions(int argc, char **argv) = 0;

protected:
    static KSystem *s_instance;
};
