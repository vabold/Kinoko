#pragma once

#include "source/host/SceneId.hh"

#include <source/egg/core/SceneCreator.hh>

#include <Common.hh>

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
