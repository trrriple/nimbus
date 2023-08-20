#pragma once

#include "nimbus/core/common.hpp"

namespace nimbus
{

enum class ScanCode : i32_t
{
    UNKNOWN = 0,

    // Usage page 0x07
    // These values are from usage page 0x07 (USB keyboard page).

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    NUM1 = 30,
    NUM2 = 31,
    NUM3 = 32,
    NUM4 = 33,
    NUM5 = 34,
    NUM6 = 35,
    NUM7 = 36,
    NUM8 = 37,
    NUM9 = 38,
    NUM0 = 39,

    RETURN    = 40,
    ESCAPE    = 41,
    BACKSPACE = 42,
    TAB       = 43,
    SPACE     = 44,

    MINUS        = 45,
    EQUALS       = 46,
    LEFTBRACKET  = 47,
    RIGHTBRACKET = 48,
    BACKSLASH    = 49,
    NONUSHASH    = 50,
    SEMICOLON    = 51,
    APOSTROPHE   = 52,
    GRAVE        = 53,
    COMMA        = 54,
    PERIOD       = 55,
    SLASH        = 56,

    CAPSLOCK = 57,

    F1  = 58,
    F2  = 59,
    F3  = 60,
    F4  = 61,
    F5  = 62,
    F6  = 63,
    F7  = 64,
    F8  = 65,
    F9  = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,

    PRINTSCREEN = 70,
    SCROLLLOCK  = 71,
    PAUSE       = 72,
    INSERT      = 73,  // insert on PC, help on some Mac keyboards (but does send
                       // code 73, not 117)
    HOME     = 74,
    PAGEUP   = 75,
    DELETE   = 76,
    END      = 77,
    PAGEDOWN = 78,
    RIGHT    = 79,
    LEFT     = 80,
    DOWN     = 81,
    UP       = 82,

    NUMLOCKCLEAR = 83,  // num lock on PC, clear on Mac keyboards
    KP_DIVIDE    = 84,
    KP_MULTIPLY  = 85,
    KP_MINUS     = 86,
    KP_PLUS      = 87,
    KP_ENTER     = 88,
    KP_1         = 89,
    KP_2         = 90,
    KP_3         = 91,
    KP_4         = 92,
    KP_5         = 93,
    KP_6         = 94,
    KP_7         = 95,
    KP_8         = 96,
    KP_9         = 97,
    KP_0         = 98,
    KP_PERIOD    = 99,

    NONUSBACKSLASH = 100,
    APPLICATION    = 101,
    POWER          = 102,
    KP_EQUALS      = 103,
    F13            = 104,
    F14            = 105,
    F15            = 106,
    F16            = 107,
    F17            = 108,
    F18            = 109,
    F19            = 110,
    F20            = 111,
    F21            = 112,
    F22            = 113,
    F23            = 114,
    F24            = 115,
    EXECUTE        = 116,
    HELP           = 117,
    MENU           = 118,
    SELECT         = 119,
    STOP           = 120,
    AGAIN          = 121,
    UNDO           = 122,
    CUT            = 123,
    COPY           = 124,
    PASTE          = 125,
    FIND           = 126,
    MUTE           = 127,
    VOLUMEUP       = 128,
    VOLUMEDOWN     = 129,

    KP_COMMA       = 133,
    KP_EQUALSAS400 = 134,
    INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    INTERNATIONAL2 = 136,
    INTERNATIONAL3 = 137, /**< Yen */
    INTERNATIONAL4 = 138,
    INTERNATIONAL5 = 139,
    INTERNATIONAL6 = 140,
    INTERNATIONAL7 = 141,
    INTERNATIONAL8 = 142,
    INTERNATIONAL9 = 143,
    LANG1          = 144, /**< Hangul/English toggle */
    LANG2          = 145, /**< Hanja conversion */
    LANG3          = 146, /**< Katakana */
    LANG4          = 147, /**< Hiragana */
    LANG5          = 148, /**< Zenkaku/Hankaku */
    LANG6          = 149, /**< reserved */
    LANG7          = 150, /**< reserved */
    LANG8          = 151, /**< reserved */
    LANG9          = 152, /**< reserved */
    ALTERASE       = 153, /**< Erase-Eaze */
    SYSREQ         = 154,
    CANCEL         = 155,
    CLEAR          = 156,
    PRIOR          = 157,
    RETURN2        = 158,
    SEPARATOR      = 159,
    OUT            = 160,
    OPER           = 161,
    CLEARAGAIN     = 162,
    CRSEL          = 163,
    EXSEL          = 164,

    KP_00              = 176,
    KP_000             = 177,
    THOUSANDSSEPARATOR = 178,
    DECIMALSEPARATOR   = 179,
    CURRENCYUNIT       = 180,
    CURRENCYSUBUNIT    = 181,
    KP_LEFTPAREN       = 182,
    KP_RIGHTPAREN      = 183,
    KP_LEFTBRACE       = 184,
    KP_RIGHTBRACE      = 185,
    KP_TAB             = 186,
    KP_BACKSPACE       = 187,
    KP_A               = 188,
    KP_B               = 189,
    KP_C               = 190,
    KP_D               = 191,
    KP_E               = 192,
    KP_F               = 193,
    KP_XOR             = 194,
    KP_POWER           = 195,
    KP_PERCENT         = 196,
    KP_LESS            = 197,
    KP_GREATER         = 198,
    KP_AMPERSAND       = 199,
    KP_DBLAMPERSAND    = 200,
    KP_VERTICALBAR     = 201,
    KP_DBLVERTICALBAR  = 202,
    KP_COLON           = 203,
    KP_HASH            = 204,
    KP_SPACE           = 205,
    KP_AT              = 206,
    KP_EXCLAM          = 207,
    KP_MEMSTORE        = 208,
    KP_MEMRECALL       = 209,
    KP_MEMCLEAR        = 210,
    KP_MEMADD          = 211,
    KP_MEMSUBTRACT     = 212,
    KP_MEMMULTIPLY     = 213,
    KP_MEMDIVIDE       = 214,
    KP_PLUSMINUS       = 215,
    KP_CLEAR           = 216,
    KP_CLEARENTRY      = 217,
    KP_BINARY          = 218,
    KP_OCTAL           = 219,
    KP_DECIMAL         = 220,
    KP_HEXADECIMAL     = 221,

    LCTRL  = 224,
    LSHIFT = 225,
    LALT   = 226,  // alt, option
    LGUI   = 227,  // windows, command (apple), meta
    RCTRL  = 228,
    RSHIFT = 229,
    RALT   = 230,  // alt gr, option
    RGUI   = 231,  // windows, command (apple), meta

    MODE = 257,  // I'm not sure if this is really not covered by any of the
                 // above, but since there's a special KMOD_MODE for it I'm
                 // adding it here

    // Usage page 0x0C
    // These values are mapped from usage page 0x0C (USB consumer page).
    AUDIONEXT    = 258,
    AUDIOPREV    = 259,
    AUDIOSTOP    = 260,
    AUDIOPLAY    = 261,
    AUDIOMUTE    = 262,
    MEDIASELECT  = 263,
    WWW          = 264,  // AL Internet Browser
    MAIL         = 265,
    CALCULATOR   = 266,  // AL Calculator
    COMPUTER     = 267,
    AC_SEARCH    = 268,  // AC Search
    AC_HOME      = 269,  // AC Home
    AC_BACK      = 270,  // AC Back
    AC_FORWARD   = 271,  // AC Forward
    AC_STOP      = 272,  // AC Stop
    AC_REFRESH   = 273,  // AC Refresh
    AC_BOOKMARKS = 274,  // AC Bookmarks

    // Walther keys
    // These are values that Christian Walther added (for mac keyboard?).

    BRIGHTNESSDOWN = 275,
    BRIGHTNESSUP   = 276,
    DISPLAYSWITCH  = 277,  // display mirroring/dual display switch, video mode switch
    KBDILLUMTOGGLE = 278,
    KBDILLUMDOWN   = 279,
    KBDILLUMUP     = 280,
    EJECT          = 281,
    SLEEP          = 282,  // SC System Sleep

    APP1 = 283,
    APP2 = 284,

    // Usage page 0x0C (additional media keys)
    // These values are mapped from usage page 0x0C (USB consumer page).

    AUDIOREWIND      = 285,
    AUDIOFASTFORWARD = 286,

    // Mobile keys
    // These are values that are often used on mobile phones.

    SOFTLEFT = 287,   // Usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom left of the display.
    SOFTRIGHT = 288,  // Usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom right of the display.
    CALL    = 289,    // Used for accepting phone calls.
    ENDCALL = 290,    // Used for rejecting phone calls.

    NUM_SCANCODES = 512  // not a key, just marks the number of scancodes for array bounds
};

#define NB_SCANCODE_MASK (1 << 30)
#define NB_SCANCODE_TO_KEYCODE(X) ((i32_t)X | NB_SCANCODE_MASK)

enum class KeyCode : i32_t
{
    UNKNOWN = 0,

    RETURN     = '\r',
    ESCAPE     = '\x1B',
    BACKSPACE  = '\b',
    TAB        = '\t',
    SPACE      = ' ',
    EXCLAIM    = '!',
    QUOTEDBL   = '"',
    HASH       = '#',
    PERCENT    = '%',
    DOLLAR     = '$',
    AMPERSAND  = '&',
    QUOTE      = '\'',
    LEFTPAREN  = '(',
    RIGHTPAREN = ')',
    ASTERISK   = '*',
    PLUS       = '+',
    COMMA      = ',',
    MINUS      = '-',
    PERIOD     = '.',
    SLASH      = '/',
    NUM0       = '0',
    NUM1       = '1',
    NUM2       = '2',
    NUM3       = '3',
    NUM4       = '4',
    NUM5       = '5',
    NUM6       = '6',
    NUM7       = '7',
    NUM8       = '8',
    NUM9       = '9',
    COLON      = ':',
    SEMICOLON  = ';',
    LESS       = '<',
    EQUALS     = '=',
    GREATER    = '>',
    QUESTION   = '?',
    AT         = '@',

    /*
       Skip uppercase letters
     */

    LEFTBRACKET  = '[',
    BACKSLASH    = '\\',
    RIGHTBRACKET = ']',
    CARET        = '^',
    UNDERSCORE   = '_',
    BACKQUOTE    = '`',
    a            = 'a',
    b            = 'b',
    c            = 'c',
    d            = 'd',
    e            = 'e',
    f            = 'f',
    g            = 'g',
    h            = 'h',
    i            = 'i',
    j            = 'j',
    k            = 'k',
    l            = 'l',
    m            = 'm',
    n            = 'n',
    o            = 'o',
    p            = 'p',
    q            = 'q',
    r            = 'r',
    s            = 's',
    t            = 't',
    u            = 'u',
    v            = 'v',
    w            = 'w',
    x            = 'x',
    y            = 'y',
    z            = 'z',

    CAPSLOCK = NB_SCANCODE_TO_KEYCODE(ScanCode::CAPSLOCK),

    F1  = NB_SCANCODE_TO_KEYCODE(ScanCode::F1),
    F2  = NB_SCANCODE_TO_KEYCODE(ScanCode::F2),
    F3  = NB_SCANCODE_TO_KEYCODE(ScanCode::F3),
    F4  = NB_SCANCODE_TO_KEYCODE(ScanCode::F4),
    F5  = NB_SCANCODE_TO_KEYCODE(ScanCode::F5),
    F6  = NB_SCANCODE_TO_KEYCODE(ScanCode::F6),
    F7  = NB_SCANCODE_TO_KEYCODE(ScanCode::F7),
    F8  = NB_SCANCODE_TO_KEYCODE(ScanCode::F8),
    F9  = NB_SCANCODE_TO_KEYCODE(ScanCode::F9),
    F10 = NB_SCANCODE_TO_KEYCODE(ScanCode::F10),
    F11 = NB_SCANCODE_TO_KEYCODE(ScanCode::F11),
    F12 = NB_SCANCODE_TO_KEYCODE(ScanCode::F12),

    PRINTSCREEN = NB_SCANCODE_TO_KEYCODE(ScanCode::PRINTSCREEN),
    SCROLLLOCK  = NB_SCANCODE_TO_KEYCODE(ScanCode::SCROLLLOCK),
    PAUSE       = NB_SCANCODE_TO_KEYCODE(ScanCode::PAUSE),
    INSERT      = NB_SCANCODE_TO_KEYCODE(ScanCode::INSERT),
    HOME        = NB_SCANCODE_TO_KEYCODE(ScanCode::HOME),
    PAGEUP      = NB_SCANCODE_TO_KEYCODE(ScanCode::PAGEUP),
    DELETE      = '\x7F',
    END         = NB_SCANCODE_TO_KEYCODE(ScanCode::END),
    PAGEDOWN    = NB_SCANCODE_TO_KEYCODE(ScanCode::PAGEDOWN),
    RIGHT       = NB_SCANCODE_TO_KEYCODE(ScanCode::RIGHT),
    LEFT        = NB_SCANCODE_TO_KEYCODE(ScanCode::LEFT),
    DOWN        = NB_SCANCODE_TO_KEYCODE(ScanCode::DOWN),
    UP          = NB_SCANCODE_TO_KEYCODE(ScanCode::UP),

    NUMLOCKCLEAR = NB_SCANCODE_TO_KEYCODE(ScanCode::NUMLOCKCLEAR),
    KP_DIVIDE    = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_DIVIDE),
    KP_MULTIPLY  = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MULTIPLY),
    KP_MINUS     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MINUS),
    KP_PLUS      = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_PLUS),
    KP_ENTER     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_ENTER),
    KP_1         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_1),
    KP_2         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_2),
    KP_3         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_3),
    KP_4         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_4),
    KP_5         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_5),
    KP_6         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_6),
    KP_7         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_7),
    KP_8         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_8),
    KP_9         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_9),
    KP_0         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_0),
    KP_PERIOD    = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_PERIOD),

    APPLICATION    = NB_SCANCODE_TO_KEYCODE(ScanCode::APPLICATION),
    POWER          = NB_SCANCODE_TO_KEYCODE(ScanCode::POWER),
    KP_EQUALS      = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_EQUALS),
    F13            = NB_SCANCODE_TO_KEYCODE(ScanCode::F13),
    F14            = NB_SCANCODE_TO_KEYCODE(ScanCode::F14),
    F15            = NB_SCANCODE_TO_KEYCODE(ScanCode::F15),
    F16            = NB_SCANCODE_TO_KEYCODE(ScanCode::F16),
    F17            = NB_SCANCODE_TO_KEYCODE(ScanCode::F17),
    F18            = NB_SCANCODE_TO_KEYCODE(ScanCode::F18),
    F19            = NB_SCANCODE_TO_KEYCODE(ScanCode::F19),
    F20            = NB_SCANCODE_TO_KEYCODE(ScanCode::F20),
    F21            = NB_SCANCODE_TO_KEYCODE(ScanCode::F21),
    F22            = NB_SCANCODE_TO_KEYCODE(ScanCode::F22),
    F23            = NB_SCANCODE_TO_KEYCODE(ScanCode::F23),
    F24            = NB_SCANCODE_TO_KEYCODE(ScanCode::F24),
    EXECUTE        = NB_SCANCODE_TO_KEYCODE(ScanCode::EXECUTE),
    HELP           = NB_SCANCODE_TO_KEYCODE(ScanCode::HELP),
    MENU           = NB_SCANCODE_TO_KEYCODE(ScanCode::MENU),
    SELECT         = NB_SCANCODE_TO_KEYCODE(ScanCode::SELECT),
    STOP           = NB_SCANCODE_TO_KEYCODE(ScanCode::STOP),
    AGAIN          = NB_SCANCODE_TO_KEYCODE(ScanCode::AGAIN),
    UNDO           = NB_SCANCODE_TO_KEYCODE(ScanCode::UNDO),
    CUT            = NB_SCANCODE_TO_KEYCODE(ScanCode::CUT),
    COPY           = NB_SCANCODE_TO_KEYCODE(ScanCode::COPY),
    PASTE          = NB_SCANCODE_TO_KEYCODE(ScanCode::PASTE),
    FIND           = NB_SCANCODE_TO_KEYCODE(ScanCode::FIND),
    MUTE           = NB_SCANCODE_TO_KEYCODE(ScanCode::MUTE),
    VOLUMEUP       = NB_SCANCODE_TO_KEYCODE(ScanCode::VOLUMEUP),
    VOLUMEDOWN     = NB_SCANCODE_TO_KEYCODE(ScanCode::VOLUMEDOWN),
    KP_COMMA       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_COMMA),
    KP_EQUALSAS400 = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_EQUALSAS400),

    ALTERASE   = NB_SCANCODE_TO_KEYCODE(ScanCode::ALTERASE),
    SYSREQ     = NB_SCANCODE_TO_KEYCODE(ScanCode::SYSREQ),
    CANCEL     = NB_SCANCODE_TO_KEYCODE(ScanCode::CANCEL),
    CLEAR      = NB_SCANCODE_TO_KEYCODE(ScanCode::CLEAR),
    PRIOR      = NB_SCANCODE_TO_KEYCODE(ScanCode::PRIOR),
    RETURN2    = NB_SCANCODE_TO_KEYCODE(ScanCode::RETURN2),
    SEPARATOR  = NB_SCANCODE_TO_KEYCODE(ScanCode::SEPARATOR),
    OUT        = NB_SCANCODE_TO_KEYCODE(ScanCode::OUT),
    OPER       = NB_SCANCODE_TO_KEYCODE(ScanCode::OPER),
    CLEARAGAIN = NB_SCANCODE_TO_KEYCODE(ScanCode::CLEARAGAIN),
    CRSEL      = NB_SCANCODE_TO_KEYCODE(ScanCode::CRSEL),
    EXSEL      = NB_SCANCODE_TO_KEYCODE(ScanCode::EXSEL),

    KP_00              = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_00),
    KP_000             = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_000),
    THOUSANDSSEPARATOR = NB_SCANCODE_TO_KEYCODE(ScanCode::THOUSANDSSEPARATOR),
    DECIMALSEPARATOR   = NB_SCANCODE_TO_KEYCODE(ScanCode::DECIMALSEPARATOR),
    CURRENCYUNIT       = NB_SCANCODE_TO_KEYCODE(ScanCode::CURRENCYUNIT),
    CURRENCYSUBUNIT    = NB_SCANCODE_TO_KEYCODE(ScanCode::CURRENCYSUBUNIT),
    KP_LEFTPAREN       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_LEFTPAREN),
    KP_RIGHTPAREN      = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_RIGHTPAREN),
    KP_LEFTBRACE       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_LEFTBRACE),
    KP_RIGHTBRACE      = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_RIGHTBRACE),
    KP_TAB             = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_TAB),
    KP_BACKSPACE       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_BACKSPACE),
    KP_A               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_A),
    KP_B               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_B),
    KP_C               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_C),
    KP_D               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_D),
    KP_E               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_E),
    KP_F               = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_F),
    KP_XOR             = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_XOR),
    KP_POWER           = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_POWER),
    KP_PERCENT         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_PERCENT),
    KP_LESS            = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_LESS),
    KP_GREATER         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_GREATER),
    KP_AMPERSAND       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_AMPERSAND),
    KP_DBLAMPERSAND    = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_DBLAMPERSAND),
    KP_VERTICALBAR     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_VERTICALBAR),
    KP_DBLVERTICALBAR  = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_DBLVERTICALBAR),
    KP_COLON           = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_COLON),
    KP_HASH            = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_HASH),
    KP_SPACE           = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_SPACE),
    KP_AT              = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_AT),
    KP_EXCLAM          = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_EXCLAM),
    KP_MEMSTORE        = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMSTORE),
    KP_MEMRECALL       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMRECALL),
    KP_MEMCLEAR        = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMCLEAR),
    KP_MEMADD          = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMADD),
    KP_MEMSUBTRACT     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMSUBTRACT),
    KP_MEMMULTIPLY     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMMULTIPLY),
    KP_MEMDIVIDE       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_MEMDIVIDE),
    KP_PLUSMINUS       = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_PLUSMINUS),
    KP_CLEAR           = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_CLEAR),
    KP_CLEARENTRY      = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_CLEARENTRY),
    KP_BINARY          = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_BINARY),
    KP_OCTAL           = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_OCTAL),
    KP_DECIMAL         = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_DECIMAL),
    KP_HEXADECIMAL     = NB_SCANCODE_TO_KEYCODE(ScanCode::KP_HEXADECIMAL),

    LCTRL  = NB_SCANCODE_TO_KEYCODE(ScanCode::LCTRL),
    LSHIFT = NB_SCANCODE_TO_KEYCODE(ScanCode::LSHIFT),
    LALT   = NB_SCANCODE_TO_KEYCODE(ScanCode::LALT),
    LGUI   = NB_SCANCODE_TO_KEYCODE(ScanCode::LGUI),
    RCTRL  = NB_SCANCODE_TO_KEYCODE(ScanCode::RCTRL),
    RSHIFT = NB_SCANCODE_TO_KEYCODE(ScanCode::RSHIFT),
    RALT   = NB_SCANCODE_TO_KEYCODE(ScanCode::RALT),
    RGUI   = NB_SCANCODE_TO_KEYCODE(ScanCode::RGUI),

    MODE = NB_SCANCODE_TO_KEYCODE(ScanCode::MODE),

    AUDIONEXT    = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIONEXT),
    AUDIOPREV    = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOPREV),
    AUDIOSTOP    = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOSTOP),
    AUDIOPLAY    = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOPLAY),
    AUDIOMUTE    = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOMUTE),
    MEDIASELECT  = NB_SCANCODE_TO_KEYCODE(ScanCode::MEDIASELECT),
    WWW          = NB_SCANCODE_TO_KEYCODE(ScanCode::WWW),
    MAIL         = NB_SCANCODE_TO_KEYCODE(ScanCode::MAIL),
    CALCULATOR   = NB_SCANCODE_TO_KEYCODE(ScanCode::CALCULATOR),
    COMPUTER     = NB_SCANCODE_TO_KEYCODE(ScanCode::COMPUTER),
    AC_SEARCH    = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_SEARCH),
    AC_HOME      = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_HOME),
    AC_BACK      = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_BACK),
    AC_FORWARD   = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_FORWARD),
    AC_STOP      = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_STOP),
    AC_REFRESH   = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_REFRESH),
    AC_BOOKMARKS = NB_SCANCODE_TO_KEYCODE(ScanCode::AC_BOOKMARKS),

    BRIGHTNESSDOWN = NB_SCANCODE_TO_KEYCODE(ScanCode::BRIGHTNESSDOWN),
    BRIGHTNESSUP   = NB_SCANCODE_TO_KEYCODE(ScanCode::BRIGHTNESSUP),
    DISPLAYSWITCH  = NB_SCANCODE_TO_KEYCODE(ScanCode::DISPLAYSWITCH),
    KBDILLUMTOGGLE = NB_SCANCODE_TO_KEYCODE(ScanCode::KBDILLUMTOGGLE),
    KBDILLUMDOWN   = NB_SCANCODE_TO_KEYCODE(ScanCode::KBDILLUMDOWN),
    KBDILLUMUP     = NB_SCANCODE_TO_KEYCODE(ScanCode::KBDILLUMUP),
    EJECT          = NB_SCANCODE_TO_KEYCODE(ScanCode::EJECT),
    SLEEP          = NB_SCANCODE_TO_KEYCODE(ScanCode::SLEEP),
    APP1           = NB_SCANCODE_TO_KEYCODE(ScanCode::APP1),
    APP2           = NB_SCANCODE_TO_KEYCODE(ScanCode::APP2),

    AUDIOREWIND      = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOREWIND),
    AUDIOFASTFORWARD = NB_SCANCODE_TO_KEYCODE(ScanCode::AUDIOFASTFORWARD),

    SOFTLEFT  = NB_SCANCODE_TO_KEYCODE(ScanCode::SOFTLEFT),
    SOFTRIGHT = NB_SCANCODE_TO_KEYCODE(ScanCode::SOFTRIGHT),
    CALL      = NB_SCANCODE_TO_KEYCODE(ScanCode::CALL),
    ENDCALL   = NB_SCANCODE_TO_KEYCODE(ScanCode::ENDCALL)
};

/**
 * \brief Enumeration of valid key mods (possibly OR'd together).
 */
enum class KeyMod : i32_t
{
    NONE   = 0x0000,
    LSHIFT = 0x0001,
    RSHIFT = 0x0002,
    LCTRL  = 0x0040,
    RCTRL  = 0x0080,
    LALT   = 0x0100,
    RALT   = 0x0200,
    LGUI   = 0x0400,
    RGUI   = 0x0800,
    NUM    = 0x1000,
    CAPS   = 0x2000,
    MODE   = 0x4000,
    SCROLL = 0x8000,

    CTRL  = LCTRL | RCTRL,
    SHIFT = LSHIFT | RSHIFT,
    ALT   = LALT | RALT,
    GUI   = LGUI | RGUI,

    RESERVED = SCROLL /* This is for source-level compatibility with SDL 2.0.0. */
};

/**
 *  \brief Thekeysym structure, used in key events.
 *
 *  \note  If you are looking for translated character input, see the
 * ::TEXTINPUT event.
 */
typedef struct KeySym
{
    ScanCode scancode; /**< physical key code - see ::Scancode for details */
    KeyCode  sym;      /**< virtual key code - see ::Keycode for details */
    u16_t    mod;      /**< current key modifiers */
    u32_t    unused;

} KeySym;

}  // namespace nimbus