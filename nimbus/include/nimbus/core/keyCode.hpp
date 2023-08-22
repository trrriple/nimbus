#pragma once

#include "nimbus/core/common.hpp"

namespace nimbus
{

enum class ScanCode : i32_t
{
    unknown = 0,

    // Usage page 0x07
    // These values are from usage page 0x07 (USB keyboard page).

    a = 4,
    b = 5,
    c = 6,
    d = 7,
    e = 8,
    f = 9,
    g = 10,
    h = 11,
    i = 12,
    j = 13,
    k = 14,
    l = 15,
    m = 16,
    n = 17,
    o = 18,
    p = 19,
    q = 20,
    r = 21,
    s = 22,
    t = 23,
    u = 24,
    v = 25,
    w = 26,
    x = 27,
    y = 28,
    z = 29,

    num1 = 30,
    num2 = 31,
    num3 = 32,
    num4 = 33,
    num5 = 34,
    num6 = 35,
    num7 = 36,
    num8 = 37,
    num9 = 38,
    num0 = 39,

    returnKey = 40,
    escape    = 41,
    backspace = 42,
    tab       = 43,
    space     = 44,

    minus        = 45,
    equals       = 46,
    leftBracket  = 47,
    rightBracket = 48,
    backslash    = 49,
    nonUsHash    = 50,
    semicolon    = 51,
    apostrophe   = 52,
    grave        = 53,
    comma        = 54,
    period       = 55,
    slash        = 56,

    capsLock = 57,

    f1  = 58,
    f2  = 59,
    f3  = 60,
    f4  = 61,
    f5  = 62,
    f6  = 63,
    f7  = 64,
    f8  = 65,
    f9  = 66,
    f10 = 67,
    f11 = 68,
    f12 = 69,

    printScreen = 70,
    scrollLock  = 71,
    pause       = 72,
    insert      = 73,  // insert on pc, help on some mac keyboards (but does send
                       // code 73, not 117)
    home      = 74,
    pageUp    = 75,
    deleteKey = 76,
    end       = 77,
    pageDown  = 78,
    right     = 79,
    left      = 80,
    down      = 81,
    up        = 82,

    numLockClear = 83,  // num lock on pc, clear on mac keyboards
    kpDivide     = 84,
    kpMultiply   = 85,
    kpMinus      = 86,
    kpPlus       = 87,
    kpEnter      = 88,
    kp1          = 89,
    kp2          = 90,
    kp3          = 91,
    kp4          = 92,
    kp5          = 93,
    kp6          = 94,
    kp7          = 95,
    kp8          = 96,
    kp9          = 97,
    kp0          = 98,
    kpPeriod     = 99,

    nonUsBackslash = 100,
    application    = 101,
    power          = 102,
    kpEquals       = 103,
    f13            = 104,
    f14            = 105,
    f15            = 106,
    f16            = 107,
    f17            = 108,
    f18            = 109,
    f19            = 110,
    f20            = 111,
    f21            = 112,
    f22            = 113,
    f23            = 114,
    f24            = 115,
    execute        = 116,
    help           = 117,
    menu           = 118,
    select         = 119,
    stop           = 120,
    again          = 121,
    undo           = 122,
    cut            = 123,
    copy           = 124,
    paste          = 125,
    find           = 126,
    mute           = 127,
    volumeUp       = 128,
    volumeDown     = 129,

    kpComma        = 133,
    kpEqualsAs400  = 134,
    international1 = 135, /**< used on asian keyboards, see
                                            footnotes in usb doc */
    international2 = 136,
    international3 = 137, /**< yen */
    international4 = 138,
    international5 = 139,
    international6 = 140,
    international7 = 141,
    international8 = 142,
    international9 = 143,
    lang1          = 144, /**< hangul/english toggle */
    lang2          = 145, /**< hanja conversion */
    lang3          = 146, /**< katakana */
    lang4          = 147, /**< hiragana */
    lang5          = 148, /**< zenkaku/hankaku */
    lang6          = 149, /**< reserved */
    lang7          = 150, /**< reserved */
    lang8          = 151, /**< reserved */
    lang9          = 152, /**< reserved */
    altErase       = 153, /**< erase-eaze */
    sysReq         = 154,
    cancel         = 155,
    clear          = 156,
    prior          = 157,
    return2        = 158,
    separator      = 159,
    out            = 160,
    oper           = 161,
    clearAgain     = 162,
    crSel          = 163,
    exSel          = 164,

    kp00               = 176,
    kp000              = 177,
    thousandsSeparator = 178,
    decimalSeparator   = 179,
    currencyUnit       = 180,
    currencySubunit    = 181,
    kpLeftParen        = 182,
    kpRightParen       = 183,
    kpLeftBrace        = 184,
    kpRightBrace       = 185,
    kpTab              = 186,
    kpBackspace        = 187,
    kpA                = 188,
    kpB                = 189,
    kpC                = 190,
    kpD                = 191,
    kpE                = 192,
    kpF                = 193,
    kpXor              = 194,
    kpPower            = 195,
    kpPercent          = 196,
    kpLess             = 197,
    kpGreater          = 198,
    kpAmpersand        = 199,
    kpDblAmpersand     = 200,
    kpVerticalBar      = 201,
    kpDblVerticalBar   = 202,
    kpColon            = 203,
    kpHash             = 204,
    kpSpace            = 205,
    kpAt               = 206,
    kpExclam           = 207,
    kpMemStore         = 208,
    kpMemRecall        = 209,
    kpMemClear         = 210,
    kpMemAdd           = 211,
    kpMemSubtract      = 212,
    kpMemMultiply      = 213,
    kpMemDivide        = 214,
    kpPlusMinus        = 215,
    kpClear            = 216,
    kpClearEntry       = 217,
    kpBinary           = 218,
    kpOctal            = 219,
    kpDecimal          = 220,
    kpHexadecimal      = 221,

    lCtrl  = 224,
    lShift = 225,
    lAlt   = 226,  // alt, option
    lGui   = 227,  // windows, command (apple), meta
    rCtrl  = 228,
    rShift = 229,
    rAlt   = 230,  // alt gr, option
    rGui   = 231,  // windows, command (apple), meta

    mode = 257,  // i'm not sure if this is really not covered by any of the
                 // above, but since there's a special kmod_mode for it i'm
                 // adding it here

    // usage page 0x0c
    // these values are mapped from usage page 0x0c (usb consumer page).
    audioNext   = 258,
    audioPrev   = 259,
    audioStop   = 260,
    audioPlay   = 261,
    audioMute   = 262,
    mediaSelect = 263,
    www         = 264,  // al internet browser
    mail        = 265,
    calculator  = 266,  // al calculator
    computer    = 267,
    acSearch    = 268,
    acHome      = 269,
    acBack      = 270,
    acForward   = 271,
    acStop      = 272,
    acRefresh   = 273,
    acBookmarks = 274,

    // walther keys
    // these are values that christian walther added (for mac keyboard?).

    brightnessDown = 275,
    brightnessUp   = 276,
    displaySwitch  = 277,  // display mirroring/dual display switch, video mode switch
    kbdIllumToggle = 278,
    kbdIllumDown   = 279,
    kbdIllumUp     = 280,
    eject          = 281,
    sleep          = 282,  // sc system sleep

    app1 = 283,
    app2 = 284,

    // usage page 0x0c (additional media keys)
    // these values are mapped from usage page 0x0c (usb consumer page).

    audioRewind      = 285,
    audioFastForward = 286,

    // mobile keys
    // these are values that are often used on mobile phones.

    softLeft = 287,   // usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom left of the display.
    softRight = 288,  // usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom right of the display.
    call    = 289,    // used for accepting phone calls.
    endCall = 290,    // used for rejecting phone calls.

};

#define NB_SCANCODE_MASK (1 << 30)
#define NB_SCANCODE_TO_KEYCODE(X) ((i32_t)X | NB_SCANCODE_MASK)

enum class KeyCode : i32_t
{
    unknown = 0,

    returnKey  = '\r',
    escape     = '\x1B',
    backspace  = '\b',
    tab        = '\t',
    space      = ' ',
    exclaim    = '!',
    quotedbl   = '"',
    hash       = '#',
    percent    = '%',
    dollar     = '$',
    ampersand  = '&',
    quote      = '\'',
    leftParen  = '(',
    rightParen = ')',
    asterisk   = '*',
    plus       = '+',
    comma      = ',',
    minus      = '-',
    period     = '.',
    slash      = '/',
    num0       = '0',
    num1       = '1',
    num2       = '2',
    num3       = '3',
    num4       = '4',
    num5       = '5',
    num6       = '6',
    num7       = '7',
    num8       = '8',
    num9       = '9',
    colon      = ':',
    semicolon  = ';',
    less       = '<',
    equals     = '=',
    greater    = '>',
    question   = '?',
    at         = '@',

    // Skip uppercase letters

    leftBracket  = '[',
    backslash    = '\\',
    rightBracket = ']',
    caret        = '^',
    underscore   = '_',
    backquote    = '`',
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

    capsLock = NB_SCANCODE_TO_KEYCODE(ScanCode::capsLock),

    f1  = NB_SCANCODE_TO_KEYCODE(ScanCode::f1),
    f2  = NB_SCANCODE_TO_KEYCODE(ScanCode::f2),
    f3  = NB_SCANCODE_TO_KEYCODE(ScanCode::f3),
    f4  = NB_SCANCODE_TO_KEYCODE(ScanCode::f4),
    f5  = NB_SCANCODE_TO_KEYCODE(ScanCode::f5),
    f6  = NB_SCANCODE_TO_KEYCODE(ScanCode::f6),
    f7  = NB_SCANCODE_TO_KEYCODE(ScanCode::f7),
    f8  = NB_SCANCODE_TO_KEYCODE(ScanCode::f8),
    f9  = NB_SCANCODE_TO_KEYCODE(ScanCode::f9),
    f10 = NB_SCANCODE_TO_KEYCODE(ScanCode::f10),
    f11 = NB_SCANCODE_TO_KEYCODE(ScanCode::f11),
    f12 = NB_SCANCODE_TO_KEYCODE(ScanCode::f12),

    printScreen = NB_SCANCODE_TO_KEYCODE(ScanCode::printScreen),
    scrollLock  = NB_SCANCODE_TO_KEYCODE(ScanCode::scrollLock),
    pause       = NB_SCANCODE_TO_KEYCODE(ScanCode::pause),
    insert      = NB_SCANCODE_TO_KEYCODE(ScanCode::insert),
    home        = NB_SCANCODE_TO_KEYCODE(ScanCode::home),
    pageUp      = NB_SCANCODE_TO_KEYCODE(ScanCode::pageUp),
    deleteKey   = '\x7F',
    end         = NB_SCANCODE_TO_KEYCODE(ScanCode::end),
    pageDown    = NB_SCANCODE_TO_KEYCODE(ScanCode::pageDown),
    right       = NB_SCANCODE_TO_KEYCODE(ScanCode::right),
    left        = NB_SCANCODE_TO_KEYCODE(ScanCode::left),
    down        = NB_SCANCODE_TO_KEYCODE(ScanCode::down),
    up          = NB_SCANCODE_TO_KEYCODE(ScanCode::up),

    numLockClear = NB_SCANCODE_TO_KEYCODE(ScanCode::numLockClear),
    kpDivide     = NB_SCANCODE_TO_KEYCODE(ScanCode::kpDivide),
    kpMultiply   = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMultiply),
    kpMinus      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMinus),
    kpPlus       = NB_SCANCODE_TO_KEYCODE(ScanCode::kpPlus),
    kpEnter      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpEnter),
    kp1          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp1),
    kp2          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp2),
    kp3          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp3),
    kp4          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp4),
    kp5          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp5),
    kp6          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp6),
    kp7          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp7),
    kp8          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp8),
    kp9          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp9),
    kp0          = NB_SCANCODE_TO_KEYCODE(ScanCode::kp0),
    kpPeriod     = NB_SCANCODE_TO_KEYCODE(ScanCode::kpPeriod),

    application   = NB_SCANCODE_TO_KEYCODE(ScanCode::application),
    power         = NB_SCANCODE_TO_KEYCODE(ScanCode::power),
    kpEquals      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpEquals),
    f13           = NB_SCANCODE_TO_KEYCODE(ScanCode::f13),
    f14           = NB_SCANCODE_TO_KEYCODE(ScanCode::f14),
    f15           = NB_SCANCODE_TO_KEYCODE(ScanCode::f15),
    f16           = NB_SCANCODE_TO_KEYCODE(ScanCode::f16),
    f17           = NB_SCANCODE_TO_KEYCODE(ScanCode::f17),
    f18           = NB_SCANCODE_TO_KEYCODE(ScanCode::f18),
    f19           = NB_SCANCODE_TO_KEYCODE(ScanCode::f19),
    f20           = NB_SCANCODE_TO_KEYCODE(ScanCode::f20),
    f21           = NB_SCANCODE_TO_KEYCODE(ScanCode::f21),
    f22           = NB_SCANCODE_TO_KEYCODE(ScanCode::f22),
    f23           = NB_SCANCODE_TO_KEYCODE(ScanCode::f23),
    f24           = NB_SCANCODE_TO_KEYCODE(ScanCode::f24),
    execute       = NB_SCANCODE_TO_KEYCODE(ScanCode::execute),
    help          = NB_SCANCODE_TO_KEYCODE(ScanCode::help),
    menu          = NB_SCANCODE_TO_KEYCODE(ScanCode::menu),
    select        = NB_SCANCODE_TO_KEYCODE(ScanCode::select),
    stop          = NB_SCANCODE_TO_KEYCODE(ScanCode::stop),
    again         = NB_SCANCODE_TO_KEYCODE(ScanCode::again),
    undo          = NB_SCANCODE_TO_KEYCODE(ScanCode::undo),
    cut           = NB_SCANCODE_TO_KEYCODE(ScanCode::cut),
    copy          = NB_SCANCODE_TO_KEYCODE(ScanCode::copy),
    paste         = NB_SCANCODE_TO_KEYCODE(ScanCode::paste),
    find          = NB_SCANCODE_TO_KEYCODE(ScanCode::find),
    mute          = NB_SCANCODE_TO_KEYCODE(ScanCode::mute),
    volumeUp      = NB_SCANCODE_TO_KEYCODE(ScanCode::volumeUp),
    volumeDown    = NB_SCANCODE_TO_KEYCODE(ScanCode::volumeDown),
    kpComma       = NB_SCANCODE_TO_KEYCODE(ScanCode::kpComma),
    kpEqualsAs400 = NB_SCANCODE_TO_KEYCODE(ScanCode::kpEqualsAs400),

    altErase   = NB_SCANCODE_TO_KEYCODE(ScanCode::altErase),
    sysReq     = NB_SCANCODE_TO_KEYCODE(ScanCode::sysReq),
    cancel     = NB_SCANCODE_TO_KEYCODE(ScanCode::cancel),
    clear      = NB_SCANCODE_TO_KEYCODE(ScanCode::clear),
    prior      = NB_SCANCODE_TO_KEYCODE(ScanCode::prior),
    return2    = NB_SCANCODE_TO_KEYCODE(ScanCode::return2),
    separator  = NB_SCANCODE_TO_KEYCODE(ScanCode::separator),
    out        = NB_SCANCODE_TO_KEYCODE(ScanCode::out),
    oper       = NB_SCANCODE_TO_KEYCODE(ScanCode::oper),
    clearAgain = NB_SCANCODE_TO_KEYCODE(ScanCode::clearAgain),
    crSel      = NB_SCANCODE_TO_KEYCODE(ScanCode::crSel),
    exSel      = NB_SCANCODE_TO_KEYCODE(ScanCode::exSel),

    kp00               = NB_SCANCODE_TO_KEYCODE(ScanCode::kp00),
    kp000              = NB_SCANCODE_TO_KEYCODE(ScanCode::kp000),
    thousandsSeparator = NB_SCANCODE_TO_KEYCODE(ScanCode::thousandsSeparator),
    decimalSeparator   = NB_SCANCODE_TO_KEYCODE(ScanCode::decimalSeparator),
    currencyUnit       = NB_SCANCODE_TO_KEYCODE(ScanCode::currencyUnit),
    currencySubunit    = NB_SCANCODE_TO_KEYCODE(ScanCode::currencySubunit),
    kpLeftParen        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpLeftParen),
    kpRightParen       = NB_SCANCODE_TO_KEYCODE(ScanCode::kpRightParen),
    kpLeftBrace        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpLeftBrace),
    kpRightBrace       = NB_SCANCODE_TO_KEYCODE(ScanCode::kpRightBrace),
    kpTab              = NB_SCANCODE_TO_KEYCODE(ScanCode::kpTab),
    kpBackspace        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpBackspace),
    kpA                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpA),
    kpB                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpB),
    kpC                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpC),
    kpD                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpD),
    kpE                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpE),
    kpF                = NB_SCANCODE_TO_KEYCODE(ScanCode::kpF),
    kpXor              = NB_SCANCODE_TO_KEYCODE(ScanCode::kpXor),
    kpPower            = NB_SCANCODE_TO_KEYCODE(ScanCode::kpPower),
    kpPercent          = NB_SCANCODE_TO_KEYCODE(ScanCode::kpPercent),
    kpLess             = NB_SCANCODE_TO_KEYCODE(ScanCode::kpLess),
    kpGreater          = NB_SCANCODE_TO_KEYCODE(ScanCode::kpGreater),
    kpAmpersand        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpAmpersand),
    kpDblAmpersand     = NB_SCANCODE_TO_KEYCODE(ScanCode::kpDblAmpersand),
    kpVerticalBar      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpVerticalBar),
    kpDblVerticalBar   = NB_SCANCODE_TO_KEYCODE(ScanCode::kpDblVerticalBar),
    kpColon            = NB_SCANCODE_TO_KEYCODE(ScanCode::kpColon),
    kpHash             = NB_SCANCODE_TO_KEYCODE(ScanCode::kpHash),
    kpSpace            = NB_SCANCODE_TO_KEYCODE(ScanCode::kpSpace),
    kpAt               = NB_SCANCODE_TO_KEYCODE(ScanCode::kpAt),
    kpExclam           = NB_SCANCODE_TO_KEYCODE(ScanCode::kpExclam),
    kpMemStore         = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemStore),
    kpMemRecall        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemRecall),
    kpMemClear         = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemClear),
    kpMemAdd           = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemAdd),
    kpMemSubtract      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemSubtract),
    kpMemMultiply      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemMultiply),
    kpMemDivide        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpMemDivide),
    kpPlusMinus        = NB_SCANCODE_TO_KEYCODE(ScanCode::kpPlusMinus),
    kpClear            = NB_SCANCODE_TO_KEYCODE(ScanCode::kpClear),
    kpClearEntry       = NB_SCANCODE_TO_KEYCODE(ScanCode::kpClearEntry),
    kpBinary           = NB_SCANCODE_TO_KEYCODE(ScanCode::kpBinary),
    kpOctal            = NB_SCANCODE_TO_KEYCODE(ScanCode::kpOctal),
    kpDecimal          = NB_SCANCODE_TO_KEYCODE(ScanCode::kpDecimal),
    kpHexadecimal      = NB_SCANCODE_TO_KEYCODE(ScanCode::kpHexadecimal),

    lCtrl  = NB_SCANCODE_TO_KEYCODE(ScanCode::lCtrl),
    lShift = NB_SCANCODE_TO_KEYCODE(ScanCode::lShift),
    lAlt   = NB_SCANCODE_TO_KEYCODE(ScanCode::lAlt),
    lGui   = NB_SCANCODE_TO_KEYCODE(ScanCode::lGui),
    rCtrl  = NB_SCANCODE_TO_KEYCODE(ScanCode::rCtrl),
    rShift = NB_SCANCODE_TO_KEYCODE(ScanCode::rShift),
    rAlt   = NB_SCANCODE_TO_KEYCODE(ScanCode::rAlt),
    rGui   = NB_SCANCODE_TO_KEYCODE(ScanCode::rGui),

    mode = NB_SCANCODE_TO_KEYCODE(ScanCode::mode),

    audioNext   = NB_SCANCODE_TO_KEYCODE(ScanCode::audioNext),
    audioPrev   = NB_SCANCODE_TO_KEYCODE(ScanCode::audioPrev),
    audioStop   = NB_SCANCODE_TO_KEYCODE(ScanCode::audioStop),
    audioPlay   = NB_SCANCODE_TO_KEYCODE(ScanCode::audioPlay),
    audioMute   = NB_SCANCODE_TO_KEYCODE(ScanCode::audioMute),
    mediaSelect = NB_SCANCODE_TO_KEYCODE(ScanCode::mediaSelect),
    www         = NB_SCANCODE_TO_KEYCODE(ScanCode::www),
    mail        = NB_SCANCODE_TO_KEYCODE(ScanCode::mail),
    calculator  = NB_SCANCODE_TO_KEYCODE(ScanCode::calculator),
    computer    = NB_SCANCODE_TO_KEYCODE(ScanCode::computer),
    acSearch    = NB_SCANCODE_TO_KEYCODE(ScanCode::acSearch),
    acHome      = NB_SCANCODE_TO_KEYCODE(ScanCode::acHome),
    acBack      = NB_SCANCODE_TO_KEYCODE(ScanCode::acBack),
    acForward   = NB_SCANCODE_TO_KEYCODE(ScanCode::acForward),
    acStop      = NB_SCANCODE_TO_KEYCODE(ScanCode::acStop),
    acRefresh   = NB_SCANCODE_TO_KEYCODE(ScanCode::acRefresh),
    acBookmarks = NB_SCANCODE_TO_KEYCODE(ScanCode::acBookmarks),

    brightnessDown = NB_SCANCODE_TO_KEYCODE(ScanCode::brightnessDown),
    brightnessUp   = NB_SCANCODE_TO_KEYCODE(ScanCode::brightnessUp),
    displaySwitch  = NB_SCANCODE_TO_KEYCODE(ScanCode::displaySwitch),
    kbdIllumToggle = NB_SCANCODE_TO_KEYCODE(ScanCode::kbdIllumToggle),
    kbdIllumDown   = NB_SCANCODE_TO_KEYCODE(ScanCode::kbdIllumDown),
    kbdIllumUp     = NB_SCANCODE_TO_KEYCODE(ScanCode::kbdIllumUp),
    eject          = NB_SCANCODE_TO_KEYCODE(ScanCode::eject),
    sleep          = NB_SCANCODE_TO_KEYCODE(ScanCode::sleep),
    app1           = NB_SCANCODE_TO_KEYCODE(ScanCode::app1),
    app2           = NB_SCANCODE_TO_KEYCODE(ScanCode::app2),

    audioRewind      = NB_SCANCODE_TO_KEYCODE(ScanCode::audioRewind),
    audioFastForward = NB_SCANCODE_TO_KEYCODE(ScanCode::audioFastForward),

    softLeft  = NB_SCANCODE_TO_KEYCODE(ScanCode::softLeft),
    softRight = NB_SCANCODE_TO_KEYCODE(ScanCode::softRight),
    call      = NB_SCANCODE_TO_KEYCODE(ScanCode::call),
    endCall   = NB_SCANCODE_TO_KEYCODE(ScanCode::endCall)
};

/**
 * \brief Enumeration of valid key mods (possibly OR'd together).
 */
enum class KeyMod : i32_t
{
    none   = 0x0000,
    lShift = 0x0001,
    rShift = 0x0002,
    lCtrl  = 0x0040,
    rCtrl  = 0x0080,
    lAlt   = 0x0100,
    rAlt   = 0x0200,
    lGui   = 0x0400,
    rGui   = 0x0800,
    num    = 0x1000,
    caps   = 0x2000,
    mode   = 0x4000,
    scroll = 0x8000,

    ctrl  = lCtrl | rCtrl,
    shift = lShift | rShift,
    alt   = lAlt | rAlt,
    gui   = lGui | rGui,

    reserved = scroll /* This is for source-level compatibility with SDL 2.0.0. */
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