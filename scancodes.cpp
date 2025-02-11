/* Ubuntu / GNOME: gkbd-keyboard-display -l us */
#include "scancodes.h"

#include <SDL3/SDL_scancode.h>

enum action_flag {
  ACT_NONE = 0x0000u,
  ACT_KEY  = 0x0100u,
};

typedef unsigned int action_t;

extern const action_t scancode2action[SDL_SCANCODE_COUNT];

action_t convert_scancode(const SDL_KeyboardEvent *e) {
  action_t ret = scancode2action[e->scancode];
  return (ret & ACT_KEY) ? ret & 0xffu : 0xffu;
}

/* *INDENT-ON* */     /* clang-format on */
#define ___(x) (0x00) // TODO
#define KEY(x) (ACT_KEY | x)

constexpr action_t scancode2action[SDL_SCANCODE_COUNT] = {
  ___(0xFF), // SDL_SCANCODE_UNKNOWN
  ___(0xFF), // SDL_SCANCODE_???
  ___(0xFF), // SDL_SCANCODE_???
  ___(0xFF), // SDL_SCANCODE_???
  KEY(0x20), // SDL_SCANCODE_A
  KEY(0x35), // SDL_SCANCODE_B
  KEY(0x33), // SDL_SCANCODE_C
  KEY(0x22), // SDL_SCANCODE_D
  KEY(0x12), // SDL_SCANCODE_E
  KEY(0x23), // SDL_SCANCODE_F
  KEY(0x24), // SDL_SCANCODE_G
  KEY(0x25), // SDL_SCANCODE_H
  KEY(0x17), // SDL_SCANCODE_I
  KEY(0x26), // SDL_SCANCODE_J
  KEY(0x27), // SDL_SCANCODE_K
  KEY(0x28), // SDL_SCANCODE_L
  KEY(0x37), // SDL_SCANCODE_M
  KEY(0x36), // SDL_SCANCODE_N
  KEY(0x18), // SDL_SCANCODE_O
  KEY(0x19), // SDL_SCANCODE_P
  KEY(0x10), // SDL_SCANCODE_Q
  KEY(0x13), // SDL_SCANCODE_R
  KEY(0x21), // SDL_SCANCODE_S
  KEY(0x14), // SDL_SCANCODE_T
  KEY(0x16), // SDL_SCANCODE_U
  KEY(0x34), // SDL_SCANCODE_V
  KEY(0x11), // SDL_SCANCODE_W
  KEY(0x32), // SDL_SCANCODE_X
  KEY(0x15), // SDL_SCANCODE_Y
  KEY(0x31), // SDL_SCANCODE_Z

  KEY(0x01), // SDL_SCANCODE_1
  KEY(0x02), // SDL_SCANCODE_2
  KEY(0x03), // SDL_SCANCODE_3
  KEY(0x04), // SDL_SCANCODE_4
  KEY(0x05), // SDL_SCANCODE_5
  KEY(0x06), // SDL_SCANCODE_6
  KEY(0x07), // SDL_SCANCODE_7
  KEY(0x08), // SDL_SCANCODE_8
  KEY(0x09), // SDL_SCANCODE_9
  KEY(0x0A), // SDL_SCANCODE_0

  KEY(0x44), // SDL_SCANCODE_RETURN = 40,
  KEY(0x45), // SDL_SCANCODE_ESCAPE = 41,
  KEY(0x41), // SDL_SCANCODE_BACKSPACE = 42
  KEY(0x42), // SDL_SCANCODE_TAB = 43,
  KEY(0x40), // SDL_SCANCODE_SPACE = 44,

  KEY(0x0B), // SDL_SCANCODE_MINUS = 45,
  KEY(0x0C), // SDL_SCANCODE_EQUALS = 46,
  KEY(0x1A), // SDL_SCANCODE_LEFTBRACKET = 47,
  KEY(0x1B), // SDL_SCANCODE_RIGHTBRACKET = 48,
  KEY(0x2B), // SDL_SCANCODE_BACKSLASH = 49, please read the comments in SDL_scancode.h
  KEY(0x2B), // SDL_SCANCODE_NONUSHASH = 50,

  KEY(0x29), // SDL_SCANCODE_SEMICOLON = 51,
  KEY(0x2A), // SDL_SCANCODE_APOSTROPHE = 52,
  KEY(0x00), // SDL_SCANCODE_GRAVE = 53,

  KEY(0x38), // SDL_SCANCODE_COMMA = 54,
  KEY(0x39), // SDL_SCANCODE_PERIOD = 55,
  KEY(0x3A), // SDL_SCANCODE_SLASH = 56,

  KEY(0x62), // SDL_SCANCODE_CAPSLOCK = 57,

  KEY(0x50), // SDL_SCANCODE_F1 = 58,
  KEY(0x51), // SDL_SCANCODE_F2 = 59,
  KEY(0x52), // SDL_SCANCODE_F3 = 60,
  KEY(0x53), // SDL_SCANCODE_F4 = 61,
  KEY(0x54), // SDL_SCANCODE_F5 = 62,
  KEY(0x55), // SDL_SCANCODE_F6 = 63,
  KEY(0x56), // SDL_SCANCODE_F7 = 64,
  KEY(0x57), // SDL_SCANCODE_F8 = 65,
  KEY(0x58), // SDL_SCANCODE_F9 = 66,
  KEY(0x59), // SDL_SCANCODE_F10 = 67,

  KEY(0x4B), // SDL_SCANCODE_F11 = 68, ***************** AROS, conflict with UAE convention: BACKSLASH
  KEY(0x6F), // SDL_SCANCODE_F12 = 69,

  KEY(0x6C), // SDL_SCANCODE_PRINTSCREEN = 70,
  KEY(0x6D), // SDL_SCANCODE_SCROLLLOCK = 71,
  KEY(0x6E), // SDL_SCANCODE_PAUSE = 72,
  KEY(0x47), // SDL_SCANCODE_INSERT = 73,
  KEY(0x70), // SDL_SCANCODE_HOME = 74,
  KEY(0x48), // SDL_SCANCODE_PAGEUP = 75,
  KEY(0x46), // SDL_SCANCODE_DELETE = 76,
  KEY(0x71), // SDL_SCANCODE_END = 77, ***************** AROS, conflict with UAE convention: HELP
  KEY(0x49), // SDL_SCANCODE_PAGEDOWN = 78,

  KEY(0x4E), // SDL_SCANCODE_RIGHT = 79,
  KEY(0x4F), // SDL_SCANCODE_LEFT = 80,
  KEY(0x4D), // SDL_SCANCODE_DOWN = 81,
  KEY(0x4C), // SDL_SCANCODE_UP = 82,

  ___(0x00), // SDL_SCANCODE_NUMLOCKCLEAR = 83,

  KEY(0x5C), // SDL_SCANCODE_KP_DIVIDE = 84,
  KEY(0x5D), // SDL_SCANCODE_KP_MULTIPLY = 85,
  KEY(0x4A), // SDL_SCANCODE_KP_MINUS = 86,
  KEY(0x5E), // SDL_SCANCODE_KP_PLUS = 87,
  KEY(0x43), // SDL_SCANCODE_KP_ENTER = 88,
  KEY(0x1D), // SDL_SCANCODE_KP_1 = 89,
  KEY(0x1E), // SDL_SCANCODE_KP_2 = 90,
  KEY(0x1F), // SDL_SCANCODE_KP_3 = 91,
  KEY(0x2D), // SDL_SCANCODE_KP_4 = 92,
  KEY(0x2E), // SDL_SCANCODE_KP_5 = 93,
  KEY(0x2F), // SDL_SCANCODE_KP_6 = 94,
  KEY(0x3D), // SDL_SCANCODE_KP_7 = 95,
  KEY(0x3E), // SDL_SCANCODE_KP_8 = 96,
  KEY(0x3F), // SDL_SCANCODE_KP_9 = 97,
  KEY(0x0F), // SDL_SCANCODE_KP_0 = 98,
  KEY(0x3C), // SDL_SCANCODE_KP_PERIOD = 99,

  KEY(0x30), // SDL_SCANCODE_NONUSBACKSLASH = 100,

  ___(0xFF), // SDL_SCANCODE_APPLICATION = 101,
  ___(0xFF), // SDL_SCANCODE_POWER = 102,

  KEY(0x43), // SDL_SCANCODE_KP_EQUALS = 103,
  ___(0xFF), // SDL_SCANCODE_F13 = 104,
  ___(0xFF), // SDL_SCANCODE_F14 = 105,
  ___(0xFF), // SDL_SCANCODE_F15 = 106,
  ___(0xFF), // SDL_SCANCODE_F16 = 107,
  ___(0xFF), // SDL_SCANCODE_F17 = 108,
  ___(0xFF), // SDL_SCANCODE_F18 = 109,
  ___(0xFF), // SDL_SCANCODE_F19 = 110,
  ___(0xFF), // SDL_SCANCODE_F20 = 111,
  ___(0xFF), // SDL_SCANCODE_F21 = 112,
  ___(0xFF), // SDL_SCANCODE_F22 = 113,
  ___(0xFF), // SDL_SCANCODE_F23 = 114,
  ___(0xFF), // SDL_SCANCODE_F24 = 115,
  KEY(0x43), // SDL_SCANCODE_EXECUTE = 116,
  KEY(0x5F), // SDL_SCANCODE_HELP = 117,
  ___(0xFF), // SDL_SCANCODE_MENU = 118,    /**< Menu (show menu) */
  ___(0xFF), // SDL_SCANCODE_SELECT = 119,
  ___(0xFF), // SDL_SCANCODE_STOP = 120,    /**< AC Stop */
  ___(0xFF), // SDL_SCANCODE_AGAIN = 121,   /**< AC Redo/Repeat */
  ___(0xFF), // SDL_SCANCODE_UNDO = 122,    /**< AC Undo */
  ___(0xFF), // SDL_SCANCODE_CUT = 123,     /**< AC Cut */
  ___(0xFF), // SDL_SCANCODE_COPY = 124,    /**< AC Copy */
  ___(0xFF), // SDL_SCANCODE_PASTE = 125,   /**< AC Paste */
  ___(0xFF), // SDL_SCANCODE_FIND = 126,    /**< AC Find */
  ___(0xFF), // SDL_SCANCODE_MUTE = 127,
  ___(0xFF), // SDL_SCANCODE_VOLUMEUP = 128,
  ___(0xFF), // SDL_SCANCODE_VOLUMEDOWN = 129,

  ___(0xFF), // SDL_SCANCODE_LOCKINGCAPSLOCK = 130,
  ___(0xFF), // SDL_SCANCODE_LOCKINGNUMLOCK = 131,
  ___(0xFF), // SDL_SCANCODE_LOCKINGSCROLLLOCK =

  KEY(0x3C), // SDL_SCANCODE_KP_COMMA = 133,
  ___(0xFF), // SDL_SCANCODE_KP_EQUALSAS400 = 134,

  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL1 = 135,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL2 = 136,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */ // TODO SHIFT-ALT-0x2B
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL4 = 138,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL5 = 139,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL6 = 140,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL7 = 141,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL8 = 142,
  ___(0xFF), // SDL_SCANCODE_INTERNATIONAL9 = 143,
  ___(0xFF), // SDL_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
  ___(0xFF), // SDL_SCANCODE_LANG2 = 145, /**< Hanja conversion */
  ___(0xFF), // SDL_SCANCODE_LANG3 = 146, /**< Katakana */
  ___(0xFF), // SDL_SCANCODE_LANG4 = 147, /**< Hiragana */
  ___(0xFF), // SDL_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
  ___(0xFF), // SDL_SCANCODE_LANG6 = 149, /**< reserved */
  ___(0xFF), // SDL_SCANCODE_LANG7 = 150, /**< reserved */
  ___(0xFF), // SDL_SCANCODE_LANG8 = 151, /**< reserved */
  ___(0xFF), // SDL_SCANCODE_LANG9 = 152, /**< reserved */

  ___(0xFF), // SDL_SCANCODE_ALTERASE = 153,    /**< Erase-Eaze */
  ___(0xFF), // SDL_SCANCODE_SYSREQ = 154,
  ___(0xFF), // SDL_SCANCODE_CANCEL = 155,      /**< AC Cancel */
  ___(0xFF), // SDL_SCANCODE_CLEAR = 156,
  ___(0xFF), // SDL_SCANCODE_PRIOR = 157,
  KEY(0x43), // SDL_SCANCODE_RETURN2 = 158,
  ___(0xFF), // SDL_SCANCODE_SEPARATOR = 159,
  ___(0xFF), // SDL_SCANCODE_OUT = 160,
  ___(0xFF), // SDL_SCANCODE_OPER = 161,
  ___(0xFF), // SDL_SCANCODE_CLEARAGAIN = 162,
  ___(0xFF), // SDL_SCANCODE_CRSEL = 163,
  ___(0xFF), // SDL_SCANCODE_EXSEL = 164,

  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //
  ___(0xFF), //

  ___(0xFF), // SDL_SCANCODE_KP_00 = 176,
  ___(0xFF), // SDL_SCANCODE_KP_000 = 177,
  ___(0xFF), // SDL_SCANCODE_THOUSANDSSEPARATOR = 178,
  ___(0xFF), // SDL_SCANCODE_DECIMALSEPARATOR = 179,
  ___(0xFF), // SDL_SCANCODE_CURRENCYUNIT = 180,
  ___(0xFF), // SDL_SCANCODE_CURRENCYSUBUNIT = 181,
  ___(0xFF), // SDL_SCANCODE_KP_LEFTPAREN = 182,
  ___(0xFF), // SDL_SCANCODE_KP_RIGHTPAREN = 183,
  ___(0xFF), // SDL_SCANCODE_KP_LEFTBRACE = 184,
  ___(0xFF), // SDL_SCANCODE_KP_RIGHTBRACE = 185,
  KEY(0x42), // SDL_SCANCODE_KP_TAB = 186,
  ___(0xFF), // SDL_SCANCODE_KP_BACKSPACE = 187,
  ___(0xFF), // SDL_SCANCODE_KP_A = 188,
  ___(0xFF), // SDL_SCANCODE_KP_B = 189,
  ___(0xFF), // SDL_SCANCODE_KP_C = 190,
  ___(0xFF), // SDL_SCANCODE_KP_D = 191,
  ___(0xFF), // SDL_SCANCODE_KP_E = 192,
  ___(0xFF), // SDL_SCANCODE_KP_F = 193,
  ___(0xFF), // SDL_SCANCODE_KP_XOR = 194,
  ___(0xFF), // SDL_SCANCODE_KP_POWER = 195,
  ___(0xFF), // SDL_SCANCODE_KP_PERCENT = 196,
  ___(0xFF), // SDL_SCANCODE_KP_LESS = 197,
  ___(0xFF), // SDL_SCANCODE_KP_GREATER = 198,
  ___(0xFF), // SDL_SCANCODE_KP_AMPERSAND = 199,
  ___(0xFF), // SDL_SCANCODE_KP_DBLAMPERSAND = 200,
  ___(0xFF), // SDL_SCANCODE_KP_VERTICALBAR = 201,
  ___(0xFF), // SDL_SCANCODE_KP_DBLVERTICALBAR = 202,
  ___(0xFF), // SDL_SCANCODE_KP_COLON = 203,
  ___(0xFF), // SDL_SCANCODE_KP_HASH = 204,
  ___(0xFF), // SDL_SCANCODE_KP_SPACE = 205,
  ___(0xFF), // SDL_SCANCODE_KP_AT = 206,
  ___(0xFF), // SDL_SCANCODE_KP_EXCLAM = 207,
  ___(0xFF), // SDL_SCANCODE_KP_MEMSTORE = 208,
  ___(0xFF), // SDL_SCANCODE_KP_MEMRECALL = 209,
  ___(0xFF), // SDL_SCANCODE_KP_MEMCLEAR = 210,
  ___(0xFF), // SDL_SCANCODE_KP_MEMADD = 211,
  ___(0xFF), // SDL_SCANCODE_KP_MEMSUBTRACT = 212,
  ___(0xFF), // SDL_SCANCODE_KP_MEMMULTIPLY = 213,
  ___(0xFF), // SDL_SCANCODE_KP_MEMDIVIDE = 214,

  ___(0xFF), // SDL_SCANCODE_KP_PLUSMINUS = 215,
  ___(0xFF), // SDL_SCANCODE_KP_CLEAR = 216,
  ___(0xFF), // SDL_SCANCODE_KP_CLEARENTRY = 217,
  ___(0xFF), // SDL_SCANCODE_KP_BINARY = 218,
  ___(0xFF), // SDL_SCANCODE_KP_OCTAL = 219,
  ___(0xFF), // SDL_SCANCODE_KP_DECIMAL = 220,
  ___(0xFF), // SDL_SCANCODE_KP_HEXADECIMAL = 221,

  ___(0xFF), // SDL_SCANCODE_???
  ___(0xFF), // SDL_SCANCODE_???

  KEY(0x63), // SDL_SCANCODE_LCTRL = 224,
  KEY(0x60), // SDL_SCANCODE_LSHIFT = 225,
  KEY(0x64), // SDL_SCANCODE_LALT = 226, /**< alt, option */
  KEY(0x66), // SDL_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
  KEY(0x63), // SDL_SCANCODE_RCTRL = 228,
  KEY(0x61), // SDL_SCANCODE_RSHIFT = 229,
  KEY(0x65), // SDL_SCANCODE_RALT = 230, /**< alt gr, option */
  KEY(0x67), // SDL_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */
};

// function row
static_assert(KEY(0x45) == scancode2action[SDL_SCANCODE_ESCAPE]);
static_assert(KEY(0x50) == scancode2action[SDL_SCANCODE_F1]);
static_assert(KEY(0x51) == scancode2action[SDL_SCANCODE_F2]);
static_assert(KEY(0x52) == scancode2action[SDL_SCANCODE_F3]);
static_assert(KEY(0x53) == scancode2action[SDL_SCANCODE_F4]);
static_assert(KEY(0x54) == scancode2action[SDL_SCANCODE_F5]);
static_assert(KEY(0x55) == scancode2action[SDL_SCANCODE_F6]);
static_assert(KEY(0x56) == scancode2action[SDL_SCANCODE_F7]);
static_assert(KEY(0x57) == scancode2action[SDL_SCANCODE_F8]);
static_assert(KEY(0x58) == scancode2action[SDL_SCANCODE_F9]);
static_assert(KEY(0x59) == scancode2action[SDL_SCANCODE_F10]);

// numbers row
static_assert(KEY(0x00) == scancode2action[SDL_SCANCODE_GRAVE]);
static_assert(KEY(0x01) == scancode2action[SDL_SCANCODE_1]);
static_assert(KEY(0x02) == scancode2action[SDL_SCANCODE_2]);
static_assert(KEY(0x03) == scancode2action[SDL_SCANCODE_3]);
static_assert(KEY(0x04) == scancode2action[SDL_SCANCODE_4]);
static_assert(KEY(0x05) == scancode2action[SDL_SCANCODE_5]);
static_assert(KEY(0x06) == scancode2action[SDL_SCANCODE_6]);
static_assert(KEY(0x07) == scancode2action[SDL_SCANCODE_7]);
static_assert(KEY(0x08) == scancode2action[SDL_SCANCODE_8]);
static_assert(KEY(0x09) == scancode2action[SDL_SCANCODE_9]);
static_assert(KEY(0x0A) == scancode2action[SDL_SCANCODE_0]);
static_assert(KEY(0x0B) == scancode2action[SDL_SCANCODE_MINUS]);
static_assert(KEY(0x0C) == scancode2action[SDL_SCANCODE_EQUALS]);
static_assert(KEY(0x41) == scancode2action[SDL_SCANCODE_BACKSPACE]);

// QWERTY row
static_assert(KEY(0x42) == scancode2action[SDL_SCANCODE_TAB]);
static_assert(KEY(0x10) == scancode2action[SDL_SCANCODE_Q]);
static_assert(KEY(0x11) == scancode2action[SDL_SCANCODE_W]);
static_assert(KEY(0x12) == scancode2action[SDL_SCANCODE_E]);
static_assert(KEY(0x13) == scancode2action[SDL_SCANCODE_R]);
static_assert(KEY(0x14) == scancode2action[SDL_SCANCODE_T]);
static_assert(KEY(0x15) == scancode2action[SDL_SCANCODE_Y]);
static_assert(KEY(0x16) == scancode2action[SDL_SCANCODE_U]);
static_assert(KEY(0x17) == scancode2action[SDL_SCANCODE_I]);
static_assert(KEY(0x18) == scancode2action[SDL_SCANCODE_O]);
static_assert(KEY(0x19) == scancode2action[SDL_SCANCODE_P]);
static_assert(KEY(0x1A) == scancode2action[SDL_SCANCODE_LEFTBRACKET]);
static_assert(KEY(0x1B) == scancode2action[SDL_SCANCODE_RIGHTBRACKET]);
static_assert(KEY(0x46) == scancode2action[SDL_SCANCODE_DELETE]);
static_assert(KEY(0x5F) == scancode2action[SDL_SCANCODE_HELP]);

// ASDF row
static_assert(KEY(0x63) == scancode2action[SDL_SCANCODE_LCTRL]);
static_assert(KEY(0x63) == scancode2action[SDL_SCANCODE_RCTRL]);
static_assert(KEY(0x62) == scancode2action[SDL_SCANCODE_CAPSLOCK]);
static_assert(KEY(0x20) == scancode2action[SDL_SCANCODE_A]);
static_assert(KEY(0x21) == scancode2action[SDL_SCANCODE_S]);
static_assert(KEY(0x22) == scancode2action[SDL_SCANCODE_D]);
static_assert(KEY(0x23) == scancode2action[SDL_SCANCODE_F]);
static_assert(KEY(0x24) == scancode2action[SDL_SCANCODE_G]);
static_assert(KEY(0x25) == scancode2action[SDL_SCANCODE_H]);
static_assert(KEY(0x26) == scancode2action[SDL_SCANCODE_J]);
static_assert(KEY(0x27) == scancode2action[SDL_SCANCODE_K]);
static_assert(KEY(0x28) == scancode2action[SDL_SCANCODE_L]);
static_assert(KEY(0x29) == scancode2action[SDL_SCANCODE_SEMICOLON]);
static_assert(KEY(0x2A) == scancode2action[SDL_SCANCODE_APOSTROPHE]);
static_assert(KEY(0x2B) == scancode2action[SDL_SCANCODE_BACKSLASH]);
static_assert(KEY(0x44) == scancode2action[SDL_SCANCODE_RETURN]);
static_assert(KEY(0x43) == scancode2action[SDL_SCANCODE_RETURN2]);

// ZXCV row
static_assert(KEY(0x60) == scancode2action[SDL_SCANCODE_LSHIFT]);
static_assert(KEY(0x30) == scancode2action[SDL_SCANCODE_NONUSBACKSLASH]);
static_assert(KEY(0x31) == scancode2action[SDL_SCANCODE_Z]);
static_assert(KEY(0x32) == scancode2action[SDL_SCANCODE_X]);
static_assert(KEY(0x33) == scancode2action[SDL_SCANCODE_C]);
static_assert(KEY(0x34) == scancode2action[SDL_SCANCODE_V]);
static_assert(KEY(0x35) == scancode2action[SDL_SCANCODE_B]);
static_assert(KEY(0x36) == scancode2action[SDL_SCANCODE_N]);
static_assert(KEY(0x37) == scancode2action[SDL_SCANCODE_M]);
static_assert(KEY(0x38) == scancode2action[SDL_SCANCODE_COMMA]);
static_assert(KEY(0x39) == scancode2action[SDL_SCANCODE_PERIOD]);
static_assert(KEY(0x3A) == scancode2action[SDL_SCANCODE_SLASH]);
static_assert(KEY(0x61) == scancode2action[SDL_SCANCODE_RSHIFT]);

// space bar row
static_assert(KEY(0x64) == scancode2action[SDL_SCANCODE_LALT]);
static_assert(KEY(0x66) == scancode2action[SDL_SCANCODE_LGUI]);
static_assert(KEY(0x67) == scancode2action[SDL_SCANCODE_RGUI]);
static_assert(KEY(0x65) == scancode2action[SDL_SCANCODE_RALT]);

// AROS / PC105 keys
static_assert(KEY(0x47) == scancode2action[SDL_SCANCODE_INSERT]);
static_assert(KEY(0x48) == scancode2action[SDL_SCANCODE_PAGEUP]);
static_assert(KEY(0x49) == scancode2action[SDL_SCANCODE_PAGEDOWN]);
static_assert(KEY(0x4B) == scancode2action[SDL_SCANCODE_F11]);
static_assert(KEY(0x6C) == scancode2action[SDL_SCANCODE_PRINTSCREEN]);
static_assert(KEY(0x6D) == scancode2action[SDL_SCANCODE_SCROLLLOCK]);
static_assert(KEY(0x6E) == scancode2action[SDL_SCANCODE_PAUSE]);
static_assert(KEY(0x6F) == scancode2action[SDL_SCANCODE_F12]);
static_assert(KEY(0x70) == scancode2action[SDL_SCANCODE_HOME]);
static_assert(KEY(0x71) == scancode2action[SDL_SCANCODE_END]);

// arrow keys
static_assert(KEY(0x4C) == scancode2action[SDL_SCANCODE_UP]);
static_assert(KEY(0x4D) == scancode2action[SDL_SCANCODE_DOWN]);
static_assert(KEY(0x4E) == scancode2action[SDL_SCANCODE_RIGHT]);
static_assert(KEY(0x4F) == scancode2action[SDL_SCANCODE_LEFT]);

// numeric keypad
static_assert(KEY(0x0F) == scancode2action[SDL_SCANCODE_KP_0]);
static_assert(KEY(0x1D) == scancode2action[SDL_SCANCODE_KP_1]);
static_assert(KEY(0x1E) == scancode2action[SDL_SCANCODE_KP_2]);
static_assert(KEY(0x1F) == scancode2action[SDL_SCANCODE_KP_3]);
static_assert(KEY(0x2D) == scancode2action[SDL_SCANCODE_KP_4]);
static_assert(KEY(0x2E) == scancode2action[SDL_SCANCODE_KP_5]);
static_assert(KEY(0x2F) == scancode2action[SDL_SCANCODE_KP_6]);
static_assert(KEY(0x3D) == scancode2action[SDL_SCANCODE_KP_7]);
static_assert(KEY(0x3E) == scancode2action[SDL_SCANCODE_KP_8]);
static_assert(KEY(0x3F) == scancode2action[SDL_SCANCODE_KP_9]);
static_assert(KEY(0x42) == scancode2action[SDL_SCANCODE_KP_TAB]);
static_assert(KEY(0x4A) == scancode2action[SDL_SCANCODE_KP_MINUS]);
static_assert(KEY(0x5C) == scancode2action[SDL_SCANCODE_KP_DIVIDE]);
static_assert(KEY(0x5D) == scancode2action[SDL_SCANCODE_KP_MULTIPLY]);
static_assert(KEY(0x5E) == scancode2action[SDL_SCANCODE_KP_PLUS]);
