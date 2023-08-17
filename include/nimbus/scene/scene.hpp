#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/component.hpp"

#include "entt/entity/registry.hpp"

namespace nimbus
{

class Entity;  // forward declare, can't include header for circular reason

class Scene : public refCounted
{
   public:
    Scene(const std::string& name = "Untitled");
    ~Scene();

    Entity addEntity(const std::string& name = std::string());
    void   removeEntity(Entity entity);
    void   sortEntities();

    void onStart();
    void onUpdate(float deltaTime);
    void onDraw();
    void onStop();

    void onResize(uint32_t width, uint32_t height);

   private:
    entt::registry m_registry;
    float          m_aspectRatio;
    std::string    m_name;
    uint32_t       m_genesisIndex = 0;

    friend class Entity;
    friend class SceneSerializer;

    // felix only
    friend class FelixLayer;
    friend class SceneHeirarchyPanel;
    friend class EditCameraMenuPanel;
    friend class ViewportPanel;

    void _render(Camera* p_camera);
    void _renderSceneSpecific(Camera* p_camera);
    void _onDrawEditor(Camera* p_editorCamera);

    // private addEntity for scene deserialization where these are known
    Entity _addEntity(const std::string& name,
                      const std::string& guidStr,
                      uint32_t           sequenceIndex);
};

}  // namespace nimbus