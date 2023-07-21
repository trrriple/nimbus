#pragma once

#include <string>

#include "keyCode.hpp"

namespace nimbus
{
class Event
{
   public:
    enum class Type : int32_t
    {
        FIRSTEVENT = 0,  // Unused (do not remove)

        // Application events
        QUIT = 0x100,  // User-requested quit

        // These application events have special meaning on iOS, see
        // README-ios.md for details
        APP_TERMINATING,  // The application is being terminated by the OS
        APP_LOWMEMORY,    // The application is low on memory, free memory if
                          // possible.
        APP_WILLENTERBACKGROUND,  // The application is about to enter the
                                  // background
        APP_DIDENTERBACKGROUND,  // The application did enter the background and
                                 // may not get CPU for some time
        APP_WILLENTERFOREGROUND,  // The application is about to enter the
                                  // foreground
        APP_DIDENTERFOREGROUND,   // The application is now interactive

        LOCALECHANGED,  // The user's locale preferences have changed.

        // Display events
        DISPLAY = 0x150,  // Display state change

        // Window events
        WINDOW = 0x200,  // Window state change
        SYSWM,           // System specific event

        // Keyboard events
        KEYDOWN = 0x300,  // Key pressed
        KEYUP,            // Key released
        TEXTEDITING,      // Keyboard text editing (composition)
        TEXTINPUT,        // Keyboard text input
        KEYMAPCHANGED,  // Keymap changed due to a system event such as an input
                        // language or keyboard layout change.
        TEXTEDITING_EXT,  // Extended keyboard text editing (composition)

        // Mouse events
        MOUSEMOTION = 0x400,  // Mouse moved
        MOUSEBUTTONDOWN,      // Mouse button pressed
        MOUSEBUTTONUP,        // Mouse button released
        MOUSEWHEEL,           // Mouse wheel motion

        // Joystick events
        JOYAXISMOTION = 0x600,  // Joystick axis motion
        JOYBALLMOTION,          // Joystick trackball motion
        JOYHATMOTION,           // Joystick hat position change
        JOYBUTTONDOWN,          // Joystick button pressed
        JOYBUTTONUP,            // Joystick button released
        JOYDEVICEADDED,     // A new joystick has been inserted into the system
        JOYDEVICEREMOVED,   // An opened joystick has been removed
        JOYBATTERYUPDATED,  // Joystick battery level change

        // Game controller events
        CONTROLLERAXISMOTION = 0x650,  // Game controller axis motion
        CONTROLLERBUTTONDOWN,          // Game controller button pressed
        CONTROLLERBUTTONUP,            // Game controller button released
        CONTROLLERDEVICEADDED,  // A new Game controller has been inserted into
                                // the system
        CONTROLLERDEVICEREMOVED,   // An opened Game controller has been removed
        CONTROLLERDEVICEREMAPPED,  // The controller mapping was updated
        CONTROLLERTOUCHPADDOWN,    // Game controller touchpad was touched
        CONTROLLERTOUCHPADMOTION,  // Game controller touchpad finger was moved
        CONTROLLERTOUCHPADUP,      // Game controller touchpad finger was lifted
        CONTROLLERSENSORUPDATE,    // Game controller sensor was updated

        // Touch events
        FINGERDOWN = 0x700,
        FINGERUP,
        FINGERMOTION,

        // Gesture events
        DOLLARGESTURE = 0x800,
        DOLLARRECORD,
        MULTIGESTURE,

        // Clipboard events
        CLIPBOARDUPDATE = 0x900,  // The clipboard or primary selection changed

        // Drag and drop events
        DROPFILE = 0x1000,  // The system requests a file open
        DROPTEXT,           // text/plain drag-and-drop event
        DROPBEGIN,          // A new set of drops is beginning (NULL filename)
        DROPCOMPLETE,  // Current set of drops is now complete (NULL filename)

        // Audio hotplug events
        AUDIODEVICEADDED = 0x1100,  // A new audio device is available
        AUDIODEVICEREMOVED,         // An audio device has been removed.

        // Sensor events
        SENSORUPDATE = 0x1200,  // A sensor was updated

        // Render events
        RENDER_TARGETS_RESET = 0x2000,  // The render targets have been reset
                                        // and their contents need to be updated
        RENDER_DEVICE_RESET,  // The device has been reset and all textures need
                              // to be recreated

        // Internal events
        POLLSENTINEL = 0x7F00,  // Signals the end of an event poll cycle

        // Events ::USEREVENT through ::LASTEVENT are for your use,
        // and should be allocated with registerEvents()
        USEREVENT = 0x8000,

        // This last event is only for bounding internal arrays
        LASTEVENT = 0xFFFF
    };

    /**
     *  \brief Fields shared by every event
     */
    typedef struct CommonEvent
    {
        uint32_t type;
        uint32_t
            timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } CommonEvent;

    /**
     *  \brief Display state change event data (event.display.*)
     */
    typedef struct DisplayEvent
    {
        uint32_t type;  ///< ::DISPLAYEVENT
        uint32_t
            timestamp;     ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t display;  ///< The associated display index
        uint8_t  event;    ///< ::DisplayEventID
        uint8_t  padding1;
        uint8_t  padding2;
        uint8_t  padding3;
        int32_t  data1;  ///< Event dependent data
    } DisplayEvent;

    /**
     *  \brief Window state change event data (event.window.*)
     */
    typedef struct WindowEvent
    {
        uint32_t type;  ///< ::WINDOWEVENT
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The associated window
        uint8_t  event;     ///< ::WindowEventID
        uint8_t  padding1;
        uint8_t  padding2;
        uint8_t  padding3;
        int32_t  data1;  ///< Event dependent data
        int32_t  data2;  ///< Event dependent data
    } WindowEvent;

    /**
     *  \brief Keyboard button event structure (event.key.*)
     */
    typedef struct KeyboardEvent
    {
        uint32_t type;  ///< ::KEYDOWN or ::KEYUP
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with keyboard focus, if any
        uint8_t  state;     ///< ::PRESSED or ::RELEASED
        uint8_t  repeat;    ///< Non-zero if this is a key repeat
        uint8_t  padding2;
        uint8_t  padding3;
        KeySym   keysym;  ///< The key that was pressed or released
    } KeyboardEvent;

#define TEXTEDITINGEVENT_TEXT_SIZE (32)
    /**
     *  \brief Keyboard text editing event structure (event.edit.*)
     */
    typedef struct TextEditingEvent
    {
        uint32_t type;  ///< ::TEXTEDITING
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with keyboard focus, if any
        char     text[TEXTEDITINGEVENT_TEXT_SIZE];  ///< The editing text
        int32_t  start;   ///< The start cursor of selected editing text
        int32_t  length;  ///< The length of selected editing text
    } TextEditingEvent;

    /**
     *  \brief Extended keyboard text editing event structure (event.editExt.*)
     * when text would be truncated if stored in the text buffer
     * SDL_TextEditingEvent
     */
    typedef struct TextEditingExtEvent
    {
        uint32_t type;  ///< ::TEXTEDITING_EXT
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with keyboard focus, if any
        char*    text;      ///< The editing text, which should be freed with
                            ///< SDL_free(), and will not be NULL
        int32_t start;      ///< The start cursor of selected editing text
        int32_t length;     ///< The length of selected editing text
    } TextEditingExtEvent;

#define TEXTINPUTEVENT_TEXT_SIZE (32)
    /**
     *  \brief Keyboard text input event structure (event.text.*)
     */
    typedef struct TextInputEvent
    {
        uint32_t type;  ///< ::TEXTINPUT
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with keyboard focus, if any
        char     text[TEXTINPUTEVENT_TEXT_SIZE];  ///< The input text
    } TextInputEvent;

    /**
     *  \brief Mouse motion event structure (event.motion.*)
     */
    typedef struct MouseMotionEvent
    {
        uint32_t type;  ///< ::MOUSEMOTION
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with mouse focus, if any
        uint32_t which;     ///< The mouse instance id, or TOUCH_MOUSEID
        uint32_t state;     ///< The current button state
        int32_t  x;         ///< X coordinate, relative to window
        int32_t  y;         ///< Y coordinate, relative to window
        int32_t  xrel;      ///< The relative motion in the X direction
        int32_t  yrel;      ///< The relative motion in the Y direction
    } MouseMotionEvent;

    /**
     *  \brief Mouse button event structure (event.button.*)
     */
    typedef struct MouseButtonEvent
    {
        uint32_t type;  ///< ::MOUSEBUTTONDOWN or ::MOUSEBUTTONUP
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with mouse focus, if any
        uint32_t which;     ///< The mouse instance id, or TOUCH_MOUSEID
        uint8_t  button;    ///< The mouse button index
        uint8_t  state;     ///< ::PRESSED or ::RELEASED
        uint8_t  clicks;    ///< 1 for single-click, 2 for double-click, etc.
        uint8_t  padding1;
        int32_t  x;  ///< X coordinate, relative to window
        int32_t  y;  ///< Y coordinate, relative to window
    } MouseButtonEvent;

    /**
     *  \brief Mouse wheel event structure (event.wheel.*)
     */
    typedef struct MouseWheelEvent
    {
        uint32_t type;  ///< ::MOUSEWHEEL
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The window with mouse focus, if any
        uint32_t which;     ///< The mouse instance id, or TOUCH_MOUSEID
        int32_t x;  ///< The amount scrolled horizontally, positive to the right
                    ///< and negative to the left
        int32_t y;  ///< The amount scrolled vertically, positive away from the
                    ///< user and negative toward the user
        uint32_t direction;  ///< Set to one of the MOUSEWHEEL_* defines. When
                             ///< FLIPPED the values in X and Y will be
                             ///< opposite. Multiply by -1 to change them back
        float preciseX;  ///< The amount scrolled horizontally, positive to the
                         ///< right and negative to the left, with float
                         ///< precision (added in 2.0.18)
        float preciseY;  ///< The amount scrolled vertically, positive away from
                         ///< the user and negative toward the user, with float
                         ///< precision (added in 2.0.18)
        int32_t mouseX;  ///< X coordinate, relative to window (added in 2.26.0)
        int32_t mouseY;  ///< Y coordinate, relative to window (added in 2.26.0)
    } MouseWheelEvent;

    /**
     *  \brief Joystick axis motion event structure (event.jaxis.*)
     */
    typedef struct JoyAxisEvent
    {
        uint32_t type;  ///< ::JOYAXISMOTION
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t axis;       ///< The joystick axis index
        uint8_t padding1;
        uint8_t padding2;
        uint8_t padding3;
        int16_t value;  ///< The axis value (range: -32768 to 32767)
        uint16_t padding4;
    } JoyAxisEvent;

    /**
     *  \brief Joystick trackball motion event structure (event.jball.*)
     */
    typedef struct JoyBallEvent
    {
        uint32_t type;  ///< ::JOYBALLMOTION
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t ball;       ///< The joystick trackball index
        uint8_t padding1;
        uint8_t padding2;
        uint8_t padding3;
        int16_t xrel;  ///< The relative motion in the X direction
        int16_t yrel;  ///< The relative motion in the Y direction
    } JoyBallEvent;

    /**
     *  \brief Joystick hat position change event structure (event.jhat.*)
     */
    typedef struct JoyHatEvent
    {
        uint32_t type;  ///< ::JOYHATMOTION
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t hat;        ///< The joystick hat index
        uint8_t value;      ///< The hat position value.
                            ///<   \sa HAT_LEFTUP HAT_UP HAT_RIGHTUP
                            ///<   \sa HAT_LEFT HAT_CENTERED HAT_RIGHT
                            ///<   \sa HAT_LEFTDOWN HAT_DOWN HAT_RIGHTDOWN
                            ///
                            ///<   Note that zero means the POV is centered.
        uint8_t padding1;
        uint8_t padding2;
    } JoyHatEvent;

    /**
     *  \brief Joystick button event structure (event.jbutton.*)
     */
    typedef struct JoyButtonEvent
    {
        uint32_t type;  ///< ::JOYBUTTONDOWN or ::JOYBUTTONUP
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t button;     ///< The joystick button index
        uint8_t state;      ///< ::PRESSED or ::RELEASED
        uint8_t padding1;
        uint8_t padding2;
    } JoyButtonEvent;

    /**
     *  \brief Joystick device event structure (event.jdevice.*)
     */
    typedef struct JoyDeviceEvent
    {
        uint32_t type;  ///< ::JOYDEVICEADDED or ::JOYDEVICEREMOVED
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick device index for the ADDED event,
                            ///< instance id for the REMOVED event
    } JoyDeviceEvent;

    enum class JoystickPowerLevel : int32_t
    {
        UNKNOWN = -1,
        EMPTY,  /* <= 5% */
        LOW,    /* <= 20% */
        MEDIUM, /* <= 70% */
        FULL,   /* <= 100% */
        WIRED,
        MAX
    };

    /**
     *  \brief Joystick battery level change event structure (event.jbattery.*)
     */
    typedef struct JoyBatteryEvent
    {
        uint32_t type;  ///< ::JOYBATTERYUPDATED
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        JoystickPowerLevel level;  ///< The joystick battery level
    } JoyBatteryEvent;

    /**
     *  \brief Game controller axis motion event structure (event.caxis.*)
     */
    typedef struct ControllerAxisEvent
    {
        uint32_t type;  ///< ::CONTROLLERAXISMOTION
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t axis;       ///< The controller axis (GameControllerAxis)
        uint8_t padding1;
        uint8_t padding2;
        uint8_t padding3;
        int16_t value;  ///< The axis value (range: -32768 to 32767)
        uint16_t padding4;
    } ControllerAxisEvent;

    /**
     *  \brief Game controller button event structure (event.cbutton.*)
     */
    typedef struct ControllerButtonEvent
    {
        uint32_t type;  ///< ::CONTROLLERBUTTONDOWN or ::CONTROLLERBUTTONUP
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        uint8_t button;     ///< The controller button (GameControllerButton)
        uint8_t state;      ///< ::PRESSED or ::RELEASED
        uint8_t padding1;
        uint8_t padding2;
    } ControllerButtonEvent;

    /**
     *  \brief Controller device event structure (event.cdevice.*)
     */
    typedef struct ControllerDeviceEvent
    {
        uint32_t type;  ///< ::CONTROLLERDEVICEADDED, ::CONTROLLERDEVICEREMOVED,
                        ///< or ::CONTROLLERDEVICEREMAPPED
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick device index for the ADDED event,
                            ///< instance id for the REMOVED or REMAPPED event
    } ControllerDeviceEvent;

    /**
     *  \brief Game controller touchpad event structure (event.ctouchpad.*)
     */
    typedef struct ControllerTouchpadEvent
    {
        uint32_t
            type;  ///< ::CONTROLLERTOUCHPADDOWN or
                   ///< ::CONTROLLERTOUCHPADMOTION or ::CONTROLLERTOUCHPADUP
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        int32_t touchpad;   ///< The index of the touchpad
        int32_t finger;     ///< The index of the finger on the touchpad
        float   x;  ///< Normalized in the range 0...1 with 0 being on the left
        float   y;  ///< Normalized in the range 0...1 with 0 being at the top
        float   pressure;  ///< Normalized in the range 0...1
    } ControllerTouchpadEvent;

    /**
     *  \brief Game controller sensor event structure (event.csensor.*)
     */
    typedef struct ControllerSensorEvent
    {
        uint32_t type;  ///< ::CONTROLLERSENSORUPDATE
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The joystick instance id
        int32_t sensor;     ///< The type of the sensor, one of the values of
                            ///< SensorType
        float data[3];      ///< Up to 3 values from the sensor, as defined in
                            ///< SDL_sensor.h
        uint64_t timestamp_us;  ///< The timestamp of the sensor reading in
                                ///< microseconds, if the hardware provides this
                                ///< information.
    } ControllerSensorEvent;

    /**
     *  \brief Audio device event structure (event.adevice.*)
     */
    typedef struct AudioDeviceEvent
    {
        uint32_t type;  ///< ::AUDIODEVICEADDED, or ::AUDIODEVICEREMOVED
        uint32_t
            timestamp;   ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t which;  ///< The audio device index for the ADDED event (valid
                         ///< until next SDL_GetNumAudioDevices() call),
                         ///< SDL_AudioDeviceID for the REMOVED event
        uint8_t iscapture;  ///< zero if an output device, non-zero if a capture
                            ///< device.
        uint8_t padding1;
        uint8_t padding2;
        uint8_t padding3;
    } AudioDeviceEvent;

    /**
     *  \brief Touch finger event structure (event.tfinger.*)
     */
    typedef struct TouchFingerEvent
    {
        uint32_t type;  ///< ::FINGERMOTION or ::FINGERDOWN or ::FINGERUP
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int64_t touchId;    ///< The touch device id
        int64_t fingerId;
        float   x;          ///< Normalized in the range 0...1
        float   y;          ///< Normalized in the range 0...1
        float   dx;         ///< Normalized in the range -1...1
        float   dy;         ///< Normalized in the range -1...1
        float   pressure;   ///< Normalized in the range 0...1
        uint32_t windowID;  ///< The window underneath the finger, if any
    } TouchFingerEvent;

    /**
     *  \brief Multiple Finger Gesture Event (event.mgesture.*)
     */
    typedef struct MultiGestureEvent
    {
        uint32_t type;  ///< ::MULTIGESTURE
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int64_t touchId;    ///< The touch device id
        float   dTheta;
        float   dDist;
        float   x;
        float   y;
        uint16_t numFingers;
        uint16_t padding;
    } MultiGestureEvent;

    /**
     * \brief Dollar Gesture Event (event.dgesture.*)
     */
    typedef struct DollarGestureEvent
    {
        uint32_t type;  ///< ::DOLLARGESTURE or ::DOLLARRECORD
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int64_t touchId;    ///< The touch device id
        int64_t gestureId;
        uint32_t numFingers;
        float    error;
        float    x;  ///< Normalized center of gesture
        float    y;  ///< Normalized center of gesture
    } DollarGestureEvent;

    /**
     *  \brief An event used to request a file open by the system (event.drop.*)
     *         This event is enabled by default, you can disable it with
     * SDL_EventState(). \note If this event is enabled, you must free the
     * filename in the event.
     */
    typedef struct DropEvent
    {
        uint32_t type;  ///< ::DROPBEGIN or ::DROPFILE or ::DROPTEXT or
                        ///< ::DROPCOMPLETE
        uint32_t
              timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        char* file;  ///< The file name, which should be freed with SDL_free(),
                     ///< is NULL on begin/complete
        uint32_t windowID;  ///< The window that was dropped on, if any
    } DropEvent;

    /**
     *  \brief Sensor event structure (event.sensor.*)
     */
    typedef struct SensorEvent
    {
        uint32_t type;  ///< ::SENSORUPDATE
        uint32_t
                timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        int32_t which;      ///< The instance ID of the sensor
        float data[6];  ///< Up to 6 values from the sensor - additional values
                        ///< can be queried using SDL_SensorGetData()
        uint64_t timestamp_us;  ///< The timestamp of the sensor reading in
                                ///< microseconds, if the hardware provides this
                                ///< information.
    } SensorEvent;

    /**
     *  \brief The "quit requested" event
     */
    typedef struct QuitEvent
    {
        uint32_t type;  ///< ::QUIT
        uint32_t
            timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } QuitEvent;

    /**
     *  \brief OS Specific event
     */
    typedef struct OSEvent
    {
        uint32_t type;  ///< ::QUIT
        uint32_t
            timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } OSEvent;

    /**
     *  \brief A user-defined event type (event.user.*)
     */
    typedef struct UserEvent
    {
        uint32_t type;  ///< ::USEREVENT through ::LASTEVENT-1
        uint32_t
            timestamp;      ///< In milliseconds, populated using SDL_GetTicks()
        uint32_t windowID;  ///< The associated window if any
        int32_t  code;      ///< User defined event code
        void*    data1;     ///< User defined data pointer
        void*    data2;     ///< User defined data pointer
    } UserEvent;

    /**
     *  \brief A video driver dependent system event (event.syswm.*)
     *         This event is disabled by default, you can enable it with
     * SDL_EventState()
     *
     *  \note If you want to use this event, you should include SDL_syswm.h.
     */
    typedef struct SysWMEvent
    {
        uint32_t type;  ///< ::SYSWMEVENT
        uint32_t
              timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        void* msg;        ///< driver dependent data, defined in SDL_syswm.h
    } SysWMEvent;

    /**
     *  \brief General event structure
     */
    typedef union eventStorage
    {
        Type                  type;      ///< Event type, shared with all events
        CommonEvent           common;    ///< Common event data
        DisplayEvent          display;   ///< Display event data
        WindowEvent           window;    ///< Window event data
        KeyboardEvent         key;       ///< Keyboard event data
        TextEditingEvent      edit;      ///< Text editing event data
        TextEditingExtEvent   editExt;   ///< Extended text editing event data
        TextInputEvent        text;      ///< Text input event data
        MouseMotionEvent      motion;    ///< Mouse motion event data
        MouseButtonEvent      button;    ///< Mouse button event data
        MouseWheelEvent       wheel;     ///< Mouse wheel event data
        JoyAxisEvent          jaxis;     ///< Joystick axis event data
        JoyBallEvent          jball;     ///< Joystick ball event data
        JoyHatEvent           jhat;      ///< Joystick hat event data
        JoyButtonEvent        jbutton;   ///< Joystick button event data
        JoyDeviceEvent        jdevice;   ///< Joystick device change event data
        JoyBatteryEvent       jbattery;  ///< Joystick battery event data
        ControllerAxisEvent   caxis;     ///< Game Controller axis event data
        ControllerButtonEvent cbutton;   ///< Game Controller button event data
        ControllerDeviceEvent cdevice;   ///< Game Controller device event data
        ControllerTouchpadEvent
            ctouchpad;                  ///< Game Controller touchpad event data
        ControllerSensorEvent csensor;  ///< Game Controller sensor event data
        AudioDeviceEvent      adevice;  ///< Audio device event data
        SensorEvent           sensor;   ///< Sensor event data
        QuitEvent             quit;     ///< Quit request event data
        UserEvent             user;     ///< Custom event data
        SysWMEvent            syswm;    ///< System dependent window event data
        TouchFingerEvent      tfinger;  ///< Touch finger event data
        MultiGestureEvent     mgesture;  ///< Gesture event data
        DollarGestureEvent    dgesture;  ///< Gesture event data
        DropEvent             drop;      ///< Drag and drop event data

        /* This is necessary for ABI compatibility between Visual C++ and GCC.
           Visual C++ will respect the push pack pragma and use 52 bytes (size
           of TextEditingEvent, the largest structure for 32-bit and 64-bit
           architectures) for this union, and GCC will use the alignment of the
           largest datatype within the union, resulting in 80 bytes. Since the
           event size has been exposed in the SDL API up to this point, we must
           continue to define the size of the event structure as 80 bytes.
         */
        uint8_t padding[sizeof(void*) <= 8    ? 56
                        : sizeof(void*) == 16 ? 64
                                              : 3 * sizeof(void*)];
    } EventDetails;

    Event() = default;

    ~Event() = default;

    EventDetails& getDetails();

    Type getEventType() const;

    bool wasHandled() const;

    void markAsHandled();

    void clear();

    const std::string toString() const;

   private:
    EventDetails m_details;
    bool         m_wasHandled = false;
};
}  // namespace nimbus