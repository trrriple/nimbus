#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/event.hpp"



// because we ripped this from SDL, make sure it actually matches because
// SDL is going to be provided the data
NM_CORE_COMPILETIME_ASSERT((sizeof(SDL_Event)
                            == sizeof(nimbus::Event::EventDetails)),
                           "Critical Error: EventDetails size mismatch!");

namespace nimbus
{

void Event::clear() noexcept
{
    m_wasHandled = false;

    // some events need to have memory freed
    switch (m_details.type)
    {
        case (Type::DROPFILE):
        {
            SDL_free(m_details.drop.file);
            break;
        }
        default:
            break;
    }
}

const std::string Event::toString() const noexcept
{
    switch (m_details.type)
    {
        case Type::FIRSTEVENT:
            return "FIRSTEVENT";
        case Type::QUIT:
            return "QUIT";
        case Type::APP_TERMINATING:
            return "APP_TERMINATING";
        case Type::APP_LOWMEMORY:
            return "APP_LOWMEMORY";
        case Type::APP_WILLENTERBACKGROUND:
            return "APP_WILLENTERBACKGROUND";
        case Type::APP_DIDENTERBACKGROUND:
            return "APP_DIDENTERBACKGROUND";
        case Type::APP_WILLENTERFOREGROUND:
            return "APP_WILLENTERFOREGROUND";
        case Type::APP_DIDENTERFOREGROUND:
            return "APP_DIDENTERFOREGROUND";
        case Type::LOCALECHANGED:
            return "LOCALECHANGED";
        case Type::DISPLAY:
            return "DISPLAY";
        case Type::WINDOW:
            return "WINDOW";
        case Type::SYSWM:
            return "SYSWM";
        case Type::KEYDOWN:
            return "KEYDOWN";
        case Type::KEYUP:
            return "KEYUP";
        case Type::TEXTEDITING:
            return "TEXTEDITING";
        case Type::TEXTINPUT:
            return "TEXTINPUT";
        case Type::KEYMAPCHANGED:
            return "KEYMAPCHANGED";
        case Type::MOUSEMOTION:
            return "MOUSEMOTION";
        case Type::MOUSEBUTTONDOWN:
            return "MOUSEBUTTONDOWN";
        case Type::MOUSEBUTTONUP:
            return "MOUSEBUTTONUP";
        case Type::MOUSEWHEEL:
            return "MOUSEWHEEL";
        case Type::JOYAXISMOTION:
            return "JOYAXISMOTION";
        case Type::JOYBALLMOTION:
            return "JOYBALLMOTION";
        case Type::JOYHATMOTION:
            return "JOYHATMOTION";
        case Type::JOYBUTTONDOWN:
            return "JOYBUTTONDOWN";
        case Type::JOYBUTTONUP:
            return "JOYBUTTONUP";
        case Type::JOYDEVICEADDED:
            return "JOYDEVICEADDED";
        case Type::JOYDEVICEREMOVED:
            return "JOYDEVICEREMOVED";
        case Type::CONTROLLERAXISMOTION:
            return "CONTROLLERAXISMOTION";
        case Type::CONTROLLERBUTTONDOWN:
            return "CONTROLLERBUTTONDOWN";
        case Type::CONTROLLERBUTTONUP:
            return "CONTROLLERBUTTONUP";
        case Type::CONTROLLERDEVICEADDED:
            return "CONTROLLERDEVICEADDED";
        case Type::CONTROLLERDEVICEREMOVED:
            return "CONTROLLERDEVICEREMOVED";
        case Type::CONTROLLERDEVICEREMAPPED:
            return "CONTROLLERDEVICEREMAPPED";
        case Type::FINGERDOWN:
            return "FINGERDOWN";
        case Type::FINGERUP:
            return "FINGERUP";
        case Type::FINGERMOTION:
            return "FINGERMOTION";
        case Type::DOLLARGESTURE:
            return "DOLLARGESTURE";
        case Type::DOLLARRECORD:
            return "DOLLARRECORD";
        case Type::MULTIGESTURE:
            return "MULTIGESTURE";
        case Type::CLIPBOARDUPDATE:
            return "CLIPBOARDUPDATE";
        case Type::DROPFILE:
            return "DROPFILE";
        case Type::AUDIODEVICEADDED:
            return "AUDIODEVICEADDED";
        case Type::AUDIODEVICEREMOVED:
            return "AUDIODEVICEREMOVED";
        case Type::SENSORUPDATE:
            return "SENSORUPDATE";
        case Type::RENDER_TARGETS_RESET:
            return "RENDER_TARGETS_RESET";
        case Type::RENDER_DEVICE_RESET:
            return "RENDER_DEVICE_RESET";
        case Type::USEREVENT:
            return "USEREVENT";
        case Type::LASTEVENT:
            return "LASTEVENT";
        default:
            return "Unknown event: " + std::to_string((int32_t)m_details.type);
    }
}

}  // namespace nimbus