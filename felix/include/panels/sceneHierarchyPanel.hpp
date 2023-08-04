#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"
#include "IconsFontAwesome6.h"

namespace nimbus
{

class SceneHeirarchyPanel
{
   public:
    SceneHeirarchyPanel(ref<Scene>& p_scene)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_sceneContext = p_scene;


    }
    ~SceneHeirarchyPanel()
    {
    }

    void onDraw()
    {
        ImGui::Begin("Heirarchy");
        
        for (auto [entityHandle] :
             mp_sceneContext->m_registry.storage<entt::entity>().each())
        {
            Entity entity = {entityHandle, mp_sceneContext.get()};
            _drawNode(&entity);
        }

        ImGui::End(); // Heirarchy
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ref<Scene>   mp_sceneContext;

    void _drawNode(Entity* p_entity)
    {

        auto name = p_entity->getComponent<NameCmp>().name;

        ImGui::Text(ICON_FA_CUBE  " %s", name.c_str());
    }

};

}  // namespace nimbus