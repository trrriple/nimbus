#pragma once

//////////////////////////////////////////////////////
// Nimbus public API headers
//////////////////////////////////////////////////////
///////////////////////////
// Core
///////////////////////////
#include "nimbus/core/application.hpp"
#include "nimbus/core/common.hpp"
#include "nimbus/core/event.hpp"
#include "nimbus/core/keyCode.hpp"
#include "nimbus/core/mouseButton.hpp"
#include "nimbus/core/layer.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/resourceManager.hpp"
#include "nimbus/core/utility.hpp"

///////////////////////////
// Physics
///////////////////////////
#include "nimbus/physics/physics2D.hpp"

///////////////////////////
// Renderer
///////////////////////////
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/font.hpp"
#include "nimbus/renderer/framebuffer.hpp"
#include "nimbus/renderer/mesh.hpp"
#include "nimbus/renderer/model.hpp"
#include "nimbus/renderer/particleEmitter.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/texture.hpp"

///////////////////////////
// Scene
///////////////////////////
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/entityLogic.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/scene/camera.hpp"
#include "nimbus/scene/sceneSerializer.hpp"

///////////////////////////
// Scripting
///////////////////////////
#include "nimbus/script/scriptEngine.hpp"

///////////////////////
// Gui
//////////////////////
#include "imgui.h"
#include "ImGuizmo.h"
#include "nimbus/guiSubsystem/widgets.hpp"

///////////////////////////
// GLM Stuff
///////////////////////////
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"