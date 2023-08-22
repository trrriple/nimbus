#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/keyCode.hpp"

#include <string>

namespace nimbus
{
class NIMBUS_API Event
{
   public:
    enum class Type : i32_t
    {
        firstEvent = 0,  // Unused (do not remove)

        // Application events
        quit = 0x100,  // User-requested quit

        // These application events have special meaning on iOS, see
        // README-ios.md for details
        appTerminating,          // The application is being terminated by the OS
        appLowMemory,            // The application is low on memory, free memory if possible.
        appWillEnterBackground,  // The application is about to enter the background
        appDidEnterBackground,   // The application did enter the background and may not get CPU for some time
        appWillEnterForeground,  // The application is about to enter the foreground
        appDidEnterForeground,   // The application is now interactive

        localeChanged,  // The user's locale preferences have changed.

        // Display events
        display = 0x150,  // Display state change

        // Window events
        window = 0x200,  // Window state change
        syswm,           // System specific event

        // Keyboard events
        keyDown = 0x300,  // Key pressed
        keyUp,            // Key released
        textEditing,      // Keyboard text editing (composition)
        textInput,        // Keyboard text input
        keymapChanged,    // Keymap changed due to a system event such as an input language or keyboard layout change.
        textEditingExt,   // Extended keyboard text editing (composition)

        // Mouse events
        mouseMotion = 0x400,  // Mouse moved
        mouseButtonDown,      // Mouse button pressed
        mouseButtonUp,        // Mouse button released
        mouseWheel,           // Mouse wheel motion

        // Joystick events
        joyAxisMotion = 0x600,  // Joystick axis motion
        joyBallMotion,          // Joystick trackball motion
        joyHatMotion,           // Joystick hat position change
        joyButtonDown,          // Joystick button pressed
        joyButtonUp,            // Joystick button released
        joyDeviceAdded,         // A new joystick has been inserted into the system
        joyDeviceRemoved,       // An opened joystick has been removed
        joyBatteryUpdated,      // Joystick battery level change

        // Game controller events
        controllerAxisMotion = 0x650,  // Game controller axis motion
        controllerButtonDown,          // Game controller button pressed
        controllerButtonUp,            // Game controller button released
        controllerDeviceAdded,         // A new Game controller has been inserted into the system
        controllerDeviceRemoved,       // An opened Game controller has been removed
        controllerDeviceRemapped,      // The controller mapping was updated
        controllerTouchpadDown,        // Game controller touchpad was touched
        controllerTouchpadMotion,      // Game controller touchpad finger was moved
        controllerTouchpadUp,          // Game controller touchpad finger was lifted
        controllerSensorUpdate,        // Game controller sensor was updated

        // Touch events
        fingerDown = 0x700,
        fingerUp,
        fingerMotion,

        // Gesture events
        dollarGesture = 0x800,
        dollarRecord,
        multiGesture,

        // Clipboard events
        clipboardUpdate = 0x900,  // The clipboard or primary selection changed

        // Drag and drop events
        dropFile = 0x1000,  // The system requests a file open
        dropText,           // text/plain drag-and-drop event
        dropBegin,          // A new set of drops is beginning (NULL filename)
        dropComplete,       // Current set of drops is now complete (NULL filename)

        // Audio hotplug events
        audioDeviceAdded = 0x1100,  // A new audio device is available
        audioDeviceRemoved,         // An audio device has been removed.

        // Sensor events
        sensorUpdate = 0x1200,  // A sensor was updated

        // Render events
        renderTargetsReset = 0x2000,  // The render targets have been reset and their contents need to be updated
        renderDeviceReset,            // The device has been reset and all textures need to be recreated

        // Internal events
        pollSentinel = 0x7F00,  // Signals the end of an event poll cycle

        // Events ::userEvent through ::lastEvent are for your use,
        // and should be allocated with registerEvents()
        userEvent = 0x8000,

        // This last event is only for bounding internal arrays
        lastEvent = 0xFFFF
    };

    /**
     *  \brief Fields shared by every event
     */
    typedef struct CommonEvent
    {
        u32_t type;
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } CommonEvent;

    /**
     *  \brief Display state change event data (event.display.*)
     */
    typedef struct DisplayEvent
    {
        u32_t type;       ///< ::DISPLAYEVENT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t display;    ///< The associated display index
        u8_t  event;      ///< ::DisplayEventID
        u8_t  padding1;
        u8_t  padding2;
        u8_t  padding3;
        i32_t data1;  ///< Event dependent data
    } DisplayEvent;

    /**
     *  \brief Window state change event data (event.window.*)
     */
    typedef struct WindowEvent
    {
        u32_t type;       ///< ::WINDOWEVENT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The associated window
        u8_t  event;      ///< ::WindowEventID
        u8_t  padding1;
        u8_t  padding2;
        u8_t  padding3;
        i32_t data1;  ///< Event dependent data
        i32_t data2;  ///< Event dependent data
    } WindowEvent;

    /**
     *  \brief Keyboard button event structure (event.key.*)
     */
    typedef struct KeyboardEvent
    {
        u32_t  type;       ///< ::KEYDOWN or ::KEYUP
        u32_t  timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t  windowID;   ///< The window with keyboard focus, if any
        u8_t   state;      ///< ::PRESSED or ::RELEASED
        u8_t   repeat;     ///< Non-zero if this is a key repeat
        u8_t   padding2;
        u8_t   padding3;
        KeySym keysym;  ///< The key that was pressed or released
    } KeyboardEvent;

#define TEXTEDITINGEVENT_TEXT_SIZE (32)
    /**
     *  \brief Keyboard text editing event structure (event.edit.*)
     */
    typedef struct TextEditingEvent
    {
        u32_t type;                              ///< ::TEXTEDITING
        u32_t timestamp;                         ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;                          ///< The window with keyboard focus, if any
        char  text[TEXTEDITINGEVENT_TEXT_SIZE];  ///< The editing text
        i32_t start;                             ///< The start cursor of selected editing text
        i32_t length;                            ///< The length of selected editing text
    } TextEditingEvent;

    /**
     *  \brief Extended keyboard text editing event structure (event.editExt.*)
     * when text would be truncated if stored in the text buffer
     * SDL_TextEditingEvent
     */
    typedef struct TextEditingExtEvent
    {
        u32_t type;       ///< ::TEXTEDITING_EXT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The window with keyboard focus, if any
        char* text;       ///< The editing text, which should be freed with
                          ///< SDL_free(), and will not be NULL
        i32_t start;      ///< The start cursor of selected editing text
        i32_t length;     ///< The length of selected editing text
    } TextEditingExtEvent;

#define TEXTINPUTEVENT_TEXT_SIZE (32)
    /**
     *  \brief Keyboard text input event structure (event.text.*)
     */
    typedef struct TextInputEvent
    {
        u32_t type;                            ///< ::TEXTINPUT
        u32_t timestamp;                       ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;                        ///< The window with keyboard focus, if any
        char  text[TEXTINPUTEVENT_TEXT_SIZE];  ///< The input text
    } TextInputEvent;

    /**
     *  \brief Mouse motion event structure (event.motion.*)
     */
    typedef struct MouseMotionEvent
    {
        u32_t type;       ///< ::MOUSEMOTION
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The window with mouse focus, if any
        u32_t which;      ///< The mouse instance id, or TOUCH_MOUSEID
        u32_t state;      ///< The current button state
        i32_t x;          ///< X coordinate, relative to window
        i32_t y;          ///< Y coordinate, relative to window
        i32_t xrel;       ///< The relative motion in the X direction
        i32_t yrel;       ///< The relative motion in the Y direction
    } MouseMotionEvent;

    /**
     *  \brief Mouse button event structure (event.button.*)
     */
    typedef struct MouseButtonEvent
    {
        u32_t type;       ///< ::MOUSEBUTTONDOWN or ::MOUSEBUTTONUP
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The window with mouse focus, if any
        u32_t which;      ///< The mouse instance id, or TOUCH_MOUSEID
        u8_t  button;     ///< The mouse button index
        u8_t  state;      ///< ::PRESSED or ::RELEASED
        u8_t  clicks;     ///< 1 for single-click, 2 for f64_t-click, etc.
        u8_t  padding1;
        i32_t x;  ///< X coordinate, relative to window
        i32_t y;  ///< Y coordinate, relative to window
    } MouseButtonEvent;

    /**
     *  \brief Mouse wheel event structure (event.wheel.*)
     */
    typedef struct MouseWheelEvent
    {
        u32_t type;       ///< ::MOUSEWHEEL
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The window with mouse focus, if any
        u32_t which;      ///< The mouse instance id, or TOUCH_MOUSEID
        i32_t x;          ///< The amount scrolled horizontally, positive to the right
                          ///< and negative to the left
        i32_t y;          ///< The amount scrolled vertically, positive away from the
                          ///< user and negative toward the user
        u32_t direction;  ///< Set to one of the MOUSEWHEEL_* defines. When
                          ///< FLIPPED the values in X and Y will be
                          ///< opposite. Multiply by -1 to change them back
        f32_t preciseX;   ///< The amount scrolled horizontally, positive to the
                          ///< right and negative to the left, with f32_t
                          ///< precision (added in 2.0.18)
        f32_t preciseY;   ///< The amount scrolled vertically, positive away from
                          ///< the user and negative toward the user, with f32_t
                          ///< precision (added in 2.0.18)
        i32_t mouseX;     ///< X coordinate, relative to window (added in 2.26.0)
        i32_t mouseY;     ///< Y coordinate, relative to window (added in 2.26.0)
    } MouseWheelEvent;

    /**
     *  \brief Joystick axis motion event structure (event.jaxis.*)
     */
    typedef struct JoyAxisEvent
    {
        u32_t type;       ///< ::JOYAXISMOTION
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  axis;       ///< The joystick axis index
        u8_t  padding1;
        u8_t  padding2;
        u8_t  padding3;
        i16_t value;  ///< The axis value (range: -32768 to 32767)
        u16_t padding4;
    } JoyAxisEvent;

    /**
     *  \brief Joystick trackball motion event structure (event.jball.*)
     */
    typedef struct JoyBallEvent
    {
        u32_t type;       ///< ::JOYBALLMOTION
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  ball;       ///< The joystick trackball index
        u8_t  padding1;
        u8_t  padding2;
        u8_t  padding3;
        i16_t xrel;  ///< The relative motion in the X direction
        i16_t yrel;  ///< The relative motion in the Y direction
    } JoyBallEvent;

    /**
     *  \brief Joystick hat position change event structure (event.jhat.*)
     */
    typedef struct JoyHatEvent
    {
        u32_t type;       ///< ::JOYHATMOTION
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  hat;        ///< The joystick hat index
        u8_t  value;      ///< The hat position value.
                          ///<   \sa HAT_LEFTUP HAT_UP HAT_RIGHTUP
                          ///<   \sa HAT_LEFT HAT_CENTERED HAT_RIGHT
                          ///<   \sa HAT_LEFTDOWN HAT_DOWN HAT_RIGHTDOWN
                          ///
                          ///<   Note that zero means the POV is centered.
        u8_t padding1;
        u8_t padding2;
    } JoyHatEvent;

    /**
     *  \brief Joystick button event structure (event.jbutton.*)
     */
    typedef struct JoyButtonEvent
    {
        u32_t type;       ///< ::JOYBUTTONDOWN or ::JOYBUTTONUP
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  button;     ///< The joystick button index
        u8_t  state;      ///< ::PRESSED or ::RELEASED
        u8_t  padding1;
        u8_t  padding2;
    } JoyButtonEvent;

    /**
     *  \brief Joystick device event structure (event.jdevice.*)
     */
    typedef struct JoyDeviceEvent
    {
        u32_t type;       ///< ::JOYDEVICEADDED or ::JOYDEVICEREMOVED
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick device index for the ADDED event,
                          ///< instance id for the REMOVED event
    } JoyDeviceEvent;

    enum class JoystickPowerLevel : i32_t
    {
        unknown = -1,
        empty,  /* <= 5% */
        low,    /* <= 20% */
        medium, /* <= 70% */
        full,   /* <= 100% */
        wired,
        max
    };

    /**
     *  \brief Joystick battery level change event structure (event.jbattery.*)
     */
    typedef struct JoyBatteryEvent
    {
        u32_t              type;       ///< ::JOYBATTERYUPDATED
        u32_t              timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t              which;      ///< The joystick instance id
        JoystickPowerLevel level;      ///< The joystick battery level
    } JoyBatteryEvent;

    /**
     *  \brief Game controller axis motion event structure (event.caxis.*)
     */
    typedef struct ControllerAxisEvent
    {
        u32_t type;       ///< ::CONTROLLERAXISMOTION
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  axis;       ///< The controller axis (GameControllerAxis)
        u8_t  padding1;
        u8_t  padding2;
        u8_t  padding3;
        i16_t value;  ///< The axis value (range: -32768 to 32767)
        u16_t padding4;
    } ControllerAxisEvent;

    /**
     *  \brief Game controller button event structure (event.cbutton.*)
     */
    typedef struct ControllerButtonEvent
    {
        u32_t type;       ///< ::CONTROLLERBUTTONDOWN or ::CONTROLLERBUTTONUP
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        u8_t  button;     ///< The controller button (GameControllerButton)
        u8_t  state;      ///< ::PRESSED or ::RELEASED
        u8_t  padding1;
        u8_t  padding2;
    } ControllerButtonEvent;

    /**
     *  \brief Controller device event structure (event.cdevice.*)
     */
    typedef struct ControllerDeviceEvent
    {
        u32_t type;       ///< ::CONTROLLERDEVICEADDED, ::CONTROLLERDEVICEREMOVED,
                          ///< or ::CONTROLLERDEVICEREMAPPED
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick device index for the ADDED event,
                          ///< instance id for the REMOVED or REMAPPED event
    } ControllerDeviceEvent;

    /**
     *  \brief Game controller touchpad event structure (event.ctouchpad.*)
     */
    typedef struct ControllerTouchpadEvent
    {
        u32_t type;       ///< ::CONTROLLERTOUCHPADDOWN or
                          ///< ::CONTROLLERTOUCHPADMOTION or ::CONTROLLERTOUCHPADUP
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;      ///< The joystick instance id
        i32_t touchpad;   ///< The index of the touchpad
        i32_t finger;     ///< The index of the finger on the touchpad
        f32_t x;          ///< Normalized in the range 0...1 with 0 being on the left
        f32_t y;          ///< Normalized in the range 0...1 with 0 being at the top
        f32_t pressure;   ///< Normalized in the range 0...1
    } ControllerTouchpadEvent;

    /**
     *  \brief Game controller sensor event structure (event.csensor.*)
     */
    typedef struct ControllerSensorEvent
    {
        u32_t type;          ///< ::CONTROLLERSENSORUPDATE
        u32_t timestamp;     ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;         ///< The joystick instance id
        i32_t sensor;        ///< The type of the sensor, one of the values of
                             ///< SensorType
        f32_t data[3];       ///< Up to 3 values from the sensor, as defined in
                             ///< SDL_sensor.h
        u64_t timestamp_us;  ///< The timestamp of the sensor reading in
                             ///< microseconds, if the hardware provides this
                             ///< information.
    } ControllerSensorEvent;

    /**
     *  \brief Audio device event structure (event.adevice.*)
     */
    typedef struct AudioDeviceEvent
    {
        u32_t type;       ///< ::AUDIODEVICEADDED, or ::AUDIODEVICEREMOVED
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t which;      ///< The audio device index for the ADDED event (valid
                          ///< until next SDL_GetNumAudioDevices() call),
                          ///< SDL_AudioDeviceID for the REMOVED event
        u8_t iscapture;   ///< zero if an output device, non-zero if a capture
                          ///< device.
        u8_t padding1;
        u8_t padding2;
        u8_t padding3;
    } AudioDeviceEvent;

    /**
     *  \brief Touch finger event structure (event.tfinger.*)
     */
    typedef struct TouchFingerEvent
    {
        u32_t type;       ///< ::FINGERMOTION or ::FINGERDOWN or ::FINGERUP
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i64_t touchId;    ///< The touch device id
        i64_t fingerId;
        f32_t x;         ///< Normalized in the range 0...1
        f32_t y;         ///< Normalized in the range 0...1
        f32_t dx;        ///< Normalized in the range -1...1
        f32_t dy;        ///< Normalized in the range -1...1
        f32_t pressure;  ///< Normalized in the range 0...1
        u32_t windowID;  ///< The window underneath the finger, if any
    } TouchFingerEvent;

    /**
     *  \brief Multiple Finger Gesture Event (event.mgesture.*)
     */
    typedef struct MultiGestureEvent
    {
        u32_t type;       ///< ::MULTIGESTURE
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i64_t touchId;    ///< The touch device id
        f32_t dTheta;
        f32_t dDist;
        f32_t x;
        f32_t y;
        u16_t numFingers;
        u16_t padding;
    } MultiGestureEvent;

    /**
     * \brief Dollar Gesture Event (event.dgesture.*)
     */
    typedef struct DollarGestureEvent
    {
        u32_t type;       ///< ::DOLLARGESTURE or ::DOLLARRECORD
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        i64_t touchId;    ///< The touch device id
        i64_t gestureId;
        u32_t numFingers;
        f32_t error;
        f32_t x;  ///< Normalized center of gesture
        f32_t y;  ///< Normalized center of gesture
    } DollarGestureEvent;

    /**
     *  \brief An event used to request a file open by the system (event.drop.*)
     *         This event is enabled by default, you can disable it with
     * SDL_EventState(). \note If this event is enabled, you must free the
     * filename in the event.
     */
    typedef struct DropEvent
    {
        u32_t type;       ///< ::DROPBEGIN or ::DROPFILE or ::DROPTEXT or
                          ///< ::DROPCOMPLETE
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        char* file;       ///< The file name, which should be freed with SDL_free(),
                          ///< is NULL on begin/complete
        u32_t windowID;   ///< The window that was dropped on, if any
    } DropEvent;

    /**
     *  \brief Sensor event structure (event.sensor.*)
     */
    typedef struct SensorEvent
    {
        u32_t type;          ///< ::SENSORUPDATE
        u32_t timestamp;     ///< In milliseconds, populated using SDL_GetTicks()
        i32_t which;         ///< The instance ID of the sensor
        f32_t data[6];       ///< Up to 6 values from the sensor - additional values
                             ///< can be queried using SDL_SensorGetData()
        u64_t timestamp_us;  ///< The timestamp of the sensor reading in
                             ///< microseconds, if the hardware provides this
                             ///< information.
    } SensorEvent;

    /**
     *  \brief The "quit requested" event
     */
    typedef struct QuitEvent
    {
        u32_t type;       ///< ::QUIT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } QuitEvent;

    /**
     *  \brief OS Specific event
     */
    typedef struct OSEvent
    {
        u32_t type;       ///< ::QUIT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
    } OSEvent;

    /**
     *  \brief A user-defined event type (event.user.*)
     */
    typedef struct UserEvent
    {
        u32_t type;       ///< ::USEREVENT through ::LASTEVENT-1
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        u32_t windowID;   ///< The associated window if any
        i32_t code;       ///< User defined event code
        void* data1;      ///< User defined data pointer
        void* data2;      ///< User defined data pointer
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
        u32_t type;       ///< ::SYSWMEVENT
        u32_t timestamp;  ///< In milliseconds, populated using SDL_GetTicks()
        void* msg;        ///< driver dependent data, defined in SDL_syswm.h
    } SysWMEvent;

    /**
     *  \brief General event structure
     */
    typedef union eventStorage
    {
        Type                    type;       ///< Event type, shared with all events
        CommonEvent             common;     ///< Common event data
        DisplayEvent            display;    ///< Display event data
        WindowEvent             window;     ///< Window event data
        KeyboardEvent           key;        ///< Keyboard event data
        TextEditingEvent        edit;       ///< Text editing event data
        TextEditingExtEvent     editExt;    ///< Extended text editing event data
        TextInputEvent          text;       ///< Text input event data
        MouseMotionEvent        motion;     ///< Mouse motion event data
        MouseButtonEvent        button;     ///< Mouse button event data
        MouseWheelEvent         wheel;      ///< Mouse wheel event data
        JoyAxisEvent            jaxis;      ///< Joystick axis event data
        JoyBallEvent            jball;      ///< Joystick ball event data
        JoyHatEvent             jhat;       ///< Joystick hat event data
        JoyButtonEvent          jbutton;    ///< Joystick button event data
        JoyDeviceEvent          jdevice;    ///< Joystick device change event data
        JoyBatteryEvent         jbattery;   ///< Joystick battery event data
        ControllerAxisEvent     caxis;      ///< Game Controller axis event data
        ControllerButtonEvent   cbutton;    ///< Game Controller button event data
        ControllerDeviceEvent   cdevice;    ///< Game Controller device event data
        ControllerTouchpadEvent ctouchpad;  ///< Game Controller touchpad event data
        ControllerSensorEvent   csensor;    ///< Game Controller sensor event data
        AudioDeviceEvent        adevice;    ///< Audio device event data
        SensorEvent             sensor;     ///< Sensor event data
        QuitEvent               quit;       ///< Quit request event data
        UserEvent               user;       ///< Custom event data
        SysWMEvent              syswm;      ///< System dependent window event data
        TouchFingerEvent        tfinger;    ///< Touch finger event data
        MultiGestureEvent       mgesture;   ///< Gesture event data
        DollarGestureEvent      dgesture;   ///< Gesture event data
        DropEvent               drop;       ///< Drag and drop event data

        /* This is necessary for ABI compatibility between Visual C++ and GCC.
           Visual C++ will respect the push pack pragma and use 52 bytes (size
           of TextEditingEvent, the largest structure for 32-bit and 64-bit
           architectures) for this union, and GCC will use the alignment of the
           largest datatype within the union, resulting in 80 bytes. Since the
           event size has been exposed in the SDL API up to this point, we must
           continue to define the size of the event structure as 80 bytes.
         */
        u8_t padding[sizeof(void*) <= 8 ? 56 : sizeof(void*) == 16 ? 64 : 3 * sizeof(void*)];
    } EventDetails;

    Event() = default;

    ~Event() = default;

    inline EventDetails& getDetails()
    {
        return m_details;
    }

    inline Type getEventType() const
    {
        return m_details.type;
    }

    inline bool wasHandled() const
    {
        return m_wasHandled;
    }

    inline void markAsHandled()
    {
        m_wasHandled = true;
    }

    void clear();

    const std::string toString() const;

   private:
    EventDetails m_details;
    bool         m_wasHandled = false;
};
}  // namespace nimbus