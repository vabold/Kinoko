#pragma once

#include "host/SceneId.hh"

#include <egg/core/SceneCreator.hh>

#include <Common.hh>

/// @brief Represents the host application.
namespace Host {

class SceneCreatorDynamic final : public EGG::SceneCreator {
public:
    EGG::Scene *create(int sceneId) const override;
    void destroy(int sceneId) const override;

private:
    EGG::Scene *create(SceneId sceneId) const;
    void destroy(SceneId sceneId) const;
};

} // namespace Host
