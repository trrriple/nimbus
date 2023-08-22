#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/event.hpp"

// because we ripped this from SDL, make sure it actually matches because
// SDL is going to be provided the data
NB_CORE_COMPILETIME_ASSERT((sizeof(SDL_Event) == sizeof(nimbus::Event::EventDetails)),
                           "Critical Error: EventDetails size mismatch!");

namespace nimbus
{

void Event::clear()
{
    m_wasHandled = false;

    // some events need to have memory freed
    switch (m_details.type)
    {
        case (Type::dropFile):
        {
            SDL_free(m_details.drop.file);
            break;
        }
        default:
            break;
    }
}

const std::string Event::toString() const
{
    switch (m_details.type)
    {
        case Type::firstEvent:
            return "firstEvent";
        case Type::quit:
            return "quit";
        case Type::appTerminating:
            return "appTerminating";
        case Type::appLowMemory:
            return "appLowMemory";
        case Type::appWillEnterBackground:
            return "appWillEnterBackground";
        case Type::appDidEnterBackground:
            return "appDidEnterBackground";
        case Type::appWillEnterForeground:
            return "appWillEnterForeground";
        case Type::appDidEnterForeground:
            return "appDidEnterForeground";
        case Type::localeChanged:
            return "localeChanged";
        case Type::display:
            return "display";
        case Type::window:
            return "window";
        case Type::syswm:
            return "syswm";
        case Type::keyDown:
            return "keyDown";
        case Type::keyUp:
            return "keyUp";
        case Type::textEditing:
            return "textEditing";
        case Type::textInput:
            return "textInput";
        case Type::keymapChanged:
            return "keymapChanged";
        case Type::mouseMotion:
            return "mouseMotion";
        case Type::mouseButtonDown:
            return "mouseButtonDown";
        case Type::mouseButtonUp:
            return "mouseButtonUp";
        case Type::mouseWheel:
            return "mouseWheel";
        case Type::joyAxisMotion:
            return "joyAxisMotion";
        case Type::joyBallMotion:
            return "joyBallMotion";
        case Type::joyHatMotion:
            return "joyHatMotion";
        case Type::joyButtonDown:
            return "joyButtonDown";
        case Type::joyButtonUp:
            return "joyButtonUp";
        case Type::joyDeviceAdded:
            return "joyDeviceAdded";
        case Type::joyDeviceRemoved:
            return "joyDeviceRemoved";
        case Type::controllerAxisMotion:
            return "controllerAxisMotion";
        case Type::controllerButtonDown:
            return "controllerButtonDown";
        case Type::controllerButtonUp:
            return "controllerButtonUp";
        case Type::controllerDeviceAdded:
            return "controllerDeviceAdded";
        case Type::controllerDeviceRemoved:
            return "controllerDeviceRemoved";
        case Type::controllerDeviceRemapped:
            return "controllerDeviceRemapped";
        case Type::fingerDown:
            return "fingerDown";
        case Type::fingerUp:
            return "fingerUp";
        case Type::fingerMotion:
            return "fingerMotion";
        case Type::dollarGesture:
            return "dollarGesture";
        case Type::dollarRecord:
            return "dollarRecord";
        case Type::multiGesture:
            return "multiGesture";
        case Type::clipboardUpdate:
            return "clipboardUpdate";
        case Type::dropFile:
            return "dropFile";
        case Type::audioDeviceAdded:
            return "audioDeviceAdded";
        case Type::audioDeviceRemoved:
            return "audioDeviceRemoved";
        case Type::sensorUpdate:
            return "sensorUpdate";
        case Type::renderTargetsReset:
            return "renderTargetsReset";
        case Type::renderDeviceReset:
            return "renderDeviceReset";
        case Type::userEvent:
            return "userEvent";
        case Type::lastEvent:
            return "lastEvent";
        default:
            return "Unknown event: " + std::to_string((i32_t)m_details.type);
    }
}

}  // namespace nimbus