#pragma once

#include "SDL.h"

#include <string>

namespace nimbus
{
class Event
{
   public:
    Event() = default;

    ~Event() = default;

    SDL_Event* getEvent()
    {
        return &m_event;
    }

    uint32_t getEventType() const
    {
        return m_event.type;
    }

    bool wasHandled() const
    {
        return m_wasHandled;
    }

    void markAsHandled()
    {
        m_wasHandled = true;
    }

    void clear()
    {
        m_wasHandled = false;
    }

    const std::string toString() const
    {
        switch (m_event.type) 
        {
            case SDL_FIRSTEVENT:              return "FIRSTEVENT";
            case SDL_QUIT:                    return "QUIT";
            case SDL_APP_TERMINATING:         return "APP_TERMINATING";
            case SDL_APP_LOWMEMORY:           return "APP_LOWMEMORY";
            case SDL_APP_WILLENTERBACKGROUND: return "APP_WILLENTERBACKGROUND";
            case SDL_APP_DIDENTERBACKGROUND:  return "APP_DIDENTERBACKGROUND";
            case SDL_APP_WILLENTERFOREGROUND: return "APP_WILLENTERFOREGROUND";
            case SDL_APP_DIDENTERFOREGROUND:  return "APP_DIDENTERFOREGROUND";
            case SDL_LOCALECHANGED:           return "LOCALECHANGED";
            case SDL_DISPLAYEVENT:            return "DISPLAYEVENT";
            case SDL_WINDOWEVENT:             return "WINDOWEVENT";
            case SDL_SYSWMEVENT:              return "SYSWMEVENT";
            case SDL_KEYDOWN:                 return "KEYDOWN";
            case SDL_KEYUP:                   return "KEYUP";
            case SDL_TEXTEDITING:             return "TEXTEDITING";
            case SDL_TEXTINPUT:               return "TEXTINPUT";
            case SDL_KEYMAPCHANGED:           return "KEYMAPCHANGED";
            case SDL_MOUSEMOTION:             return "MOUSEMOTION";
            case SDL_MOUSEBUTTONDOWN:         return "MOUSEBUTTONDOWN";
            case SDL_MOUSEBUTTONUP:           return "MOUSEBUTTONUP";
            case SDL_MOUSEWHEEL:              return "MOUSEWHEEL";
            case SDL_JOYAXISMOTION:           return "JOYAXISMOTION";
            case SDL_JOYBALLMOTION:           return "JOYBALLMOTION";
            case SDL_JOYHATMOTION:            return "JOYHATMOTION";
            case SDL_JOYBUTTONDOWN:           return "JOYBUTTONDOWN";
            case SDL_JOYBUTTONUP:             return "JOYBUTTONUP";
            case SDL_JOYDEVICEADDED:          return "JOYDEVICEADDED";
            case SDL_JOYDEVICEREMOVED:        return "JOYDEVICEREMOVED";
            case SDL_CONTROLLERAXISMOTION:    return "CONTROLLERAXISMOTION";
            case SDL_CONTROLLERBUTTONDOWN:    return "CONTROLLERBUTTONDOWN";
            case SDL_CONTROLLERBUTTONUP:      return "CONTROLLERBUTTONUP";
            case SDL_CONTROLLERDEVICEADDED:   return "CONTROLLERDEVICEADDED";
            case SDL_CONTROLLERDEVICEREMOVED: return "CONTROLLERDEVICEREMOVED";
            case SDL_CONTROLLERDEVICEREMAPPED:return "CONTROLLERDEVICEREMAPPED";
            case SDL_FINGERDOWN:              return "FINGERDOWN";
            case SDL_FINGERUP:                return "FINGERUP";
            case SDL_FINGERMOTION:            return "FINGERMOTION";
            case SDL_DOLLARGESTURE:           return "DOLLARGESTURE";
            case SDL_DOLLARRECORD:            return "DOLLARRECORD";
            case SDL_MULTIGESTURE:            return "MULTIGESTURE";
            case SDL_CLIPBOARDUPDATE:         return "CLIPBOARDUPDATE";
            case SDL_DROPFILE:                return "DROPFILE";
            case SDL_AUDIODEVICEADDED:        return "AUDIODEVICEADDED";
            case SDL_AUDIODEVICEREMOVED:      return "AUDIODEVICEREMOVED";
            case SDL_SENSORUPDATE:            return "SENSORUPDATE";
            case SDL_RENDER_TARGETS_RESET:    return "RENDER_TARGETS_RESET";
            case SDL_RENDER_DEVICE_RESET:     return "RENDER_DEVICE_RESET";
            case SDL_USEREVENT:               return "USEREVENT";
            case SDL_LASTEVENT:               return "LASTEVENT";
            default:
                return "Unknown event: " + std::to_string(m_event.type);
        }
    }

   private:
    SDL_Event m_event;
    bool      m_wasHandled = false;
};
}  // namespace nimbus