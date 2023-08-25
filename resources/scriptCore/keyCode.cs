namespace Nimbus;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Must be kept in sync with nimbus\include\nimbus\core\keyCode.hpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public enum ScanCode
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
    escape = 41,
    backspace = 42,
    tab = 43,
    space = 44,

    minus = 45,
    equals = 46,
    leftBracket = 47,
    rightBracket = 48,
    backslash = 49,
    nonUsHash = 50,
    semicolon = 51,
    apostrophe = 52,
    grave = 53,
    comma = 54,
    period = 55,
    slash = 56,

    capsLock = 57,

    f1 = 58,
    f2 = 59,
    f3 = 60,
    f4 = 61,
    f5 = 62,
    f6 = 63,
    f7 = 64,
    f8 = 65,
    f9 = 66,
    f10 = 67,
    f11 = 68,
    f12 = 69,

    printScreen = 70,
    scrollLock = 71,
    pause = 72,
    insert = 73,  // insert on pc, help on some mac keyboards (but does send
                  // code 73, not 117)
    home = 74,
    pageUp = 75,
    deleteKey = 76,
    end = 77,
    pageDown = 78,
    right = 79,
    left = 80,
    down = 81,
    up = 82,

    numLockClear = 83,  // num lock on pc, clear on mac keyboards
    kpDivide = 84,
    kpMultiply = 85,
    kpMinus = 86,
    kpPlus = 87,
    kpEnter = 88,
    kp1 = 89,
    kp2 = 90,
    kp3 = 91,
    kp4 = 92,
    kp5 = 93,
    kp6 = 94,
    kp7 = 95,
    kp8 = 96,
    kp9 = 97,
    kp0 = 98,
    kpPeriod = 99,

    nonUsBackslash = 100,
    application = 101,
    power = 102,
    kpEquals = 103,
    f13 = 104,
    f14 = 105,
    f15 = 106,
    f16 = 107,
    f17 = 108,
    f18 = 109,
    f19 = 110,
    f20 = 111,
    f21 = 112,
    f22 = 113,
    f23 = 114,
    f24 = 115,
    execute = 116,
    help = 117,
    menu = 118,
    select = 119,
    stop = 120,
    again = 121,
    undo = 122,
    cut = 123,
    copy = 124,
    paste = 125,
    find = 126,
    mute = 127,
    volumeUp = 128,
    volumeDown = 129,

    kpComma = 133,
    kpEqualsAs400 = 134,
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
    lang1 = 144, /**< hangul/english toggle */
    lang2 = 145, /**< hanja conversion */
    lang3 = 146, /**< katakana */
    lang4 = 147, /**< hiragana */
    lang5 = 148, /**< zenkaku/hankaku */
    lang6 = 149, /**< reserved */
    lang7 = 150, /**< reserved */
    lang8 = 151, /**< reserved */
    lang9 = 152, /**< reserved */
    altErase = 153, /**< erase-eaze */
    sysReq = 154,
    cancel = 155,
    clear = 156,
    prior = 157,
    return2 = 158,
    separator = 159,
    outKey = 160,
    oper = 161,
    clearAgain = 162,
    crSel = 163,
    exSel = 164,

    kp00 = 176,
    kp000 = 177,
    thousandsSeparator = 178,
    decimalSeparator = 179,
    currencyUnit = 180,
    currencySubunit = 181,
    kpLeftParen = 182,
    kpRightParen = 183,
    kpLeftBrace = 184,
    kpRightBrace = 185,
    kpTab = 186,
    kpBackspace = 187,
    kpA = 188,
    kpB = 189,
    kpC = 190,
    kpD = 191,
    kpE = 192,
    kpF = 193,
    kpXor = 194,
    kpPower = 195,
    kpPercent = 196,
    kpLess = 197,
    kpGreater = 198,
    kpAmpersand = 199,
    kpDblAmpersand = 200,
    kpVerticalBar = 201,
    kpDblVerticalBar = 202,
    kpColon = 203,
    kpHash = 204,
    kpSpace = 205,
    kpAt = 206,
    kpExclam = 207,
    kpMemStore = 208,
    kpMemRecall = 209,
    kpMemClear = 210,
    kpMemAdd = 211,
    kpMemSubtract = 212,
    kpMemMultiply = 213,
    kpMemDivide = 214,
    kpPlusMinus = 215,
    kpClear = 216,
    kpClearEntry = 217,
    kpBinary = 218,
    kpOctal = 219,
    kpDecimal = 220,
    kpHexadecimal = 221,

    lCtrl = 224,
    lShift = 225,
    lAlt = 226,  // alt, option
    lGui = 227,  // windows, command (apple), meta
    rCtrl = 228,
    rShift = 229,
    rAlt = 230,  // alt gr, option
    rGui = 231,  // windows, command (apple), meta

    mode = 257,  // i'm not sure if this is really not covered by any of the
                 // above, but since there's a special kmod_mode for it i'm
                 // adding it here

    // usage page 0x0c
    // these values are mapped from usage page 0x0c (usb consumer page).
    audioNext = 258,
    audioPrev = 259,
    audioStop = 260,
    audioPlay = 261,
    audioMute = 262,
    mediaSelect = 263,
    www = 264,  // al internet browser
    mail = 265,
    calculator = 266,  // al calculator
    computer = 267,
    acSearch = 268,
    acHome = 269,
    acBack = 270,
    acForward = 271,
    acStop = 272,
    acRefresh = 273,
    acBookmarks = 274,

    // walther keys
    // these are values that christian walther added (for mac keyboard?).

    brightnessDown = 275,
    brightnessUp = 276,
    displaySwitch = 277,  // display mirroring/dual display switch, video mode switch
    kbdIllumToggle = 278,
    kbdIllumDown = 279,
    kbdIllumUp = 280,
    eject = 281,
    sleep = 282,  // sc system sleep

    app1 = 283,
    app2 = 284,

    // usage page 0x0c (additional media keys)
    // these values are mapped from usage page 0x0c (usb consumer page).

    audioRewind = 285,
    audioFastForward = 286,

    // mobile keys
    // these are values that are often used on mobile phones.

    softLeft = 287,   // usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom left of the display.
    softRight = 288,  // usually situated below the display on phones and used as a
                      // multi-function feature key for selecting a software defined
                      // function shown on the bottom right of the display.
    call = 289,    // used for accepting phone calls.
    endCall = 290,    // used for rejecting phone calls.

};

public enum KeyMod
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

};