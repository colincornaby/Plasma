/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/
#ifndef plKeyDef_inc
#define plKeyDef_inc

#if HS_BUILD_FOR_WIN32

#include "hsWindows.h" // FIXME: This gives me a sad.

#define VK_BACK_QUOTE   0xc0
// MinGW is missing these definitions:
#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS     0xBB
#endif
#ifndef VK_OEM_COMMA
#define VK_OEM_COMMA    0xBC
#endif
#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS    0xBD
#endif
#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD   0xBE
#endif

//
// keyboard definitions:
//
// map O.S. specific keyboard defines
// to plasma key defines here...
//
//

// for win32:

enum plKeyDef
{
    KEY_A               = 'A',
    KEY_B               = 'B',
    KEY_C               = 'C',
    KEY_D               = 'D',
    KEY_E               = 'E',
    KEY_F               = 'F',
    KEY_G               = 'G',
    KEY_H               = 'H',
    KEY_I               = 'I',
    KEY_J               = 'J',
    KEY_K               = 'K',
    KEY_L               = 'L',
    KEY_M               = 'M',
    KEY_N               = 'N',
    KEY_O               = 'O',
    KEY_P               = 'P',
    KEY_Q               = 'Q',
    KEY_R               = 'R',
    KEY_S               = 'S',
    KEY_T               = 'T',
    KEY_U               = 'U',
    KEY_V               = 'V',
    KEY_W               = 'W',
    KEY_X               = 'X',
    KEY_Y               = 'Y',
    KEY_Z               = 'Z',
    KEY_0               = 0x30,
    KEY_1               = 0x31,
    KEY_2               = 0x32,
    KEY_3               = 0x33,
    KEY_4               = 0x34,
    KEY_5               = 0x35,
    KEY_6               = 0x36,
    KEY_7               = 0x37,
    KEY_8               = 0x38,
    KEY_9               = 0x39,
    KEY_F1              = VK_F1,
    KEY_F2              = VK_F2,
    KEY_F3              = VK_F3,
    KEY_F4              = VK_F4,
    KEY_F5              = VK_F5,
    KEY_F6              = VK_F6,
    KEY_F7              = VK_F7,
    KEY_F8              = VK_F8,
    KEY_F9              = VK_F9,
    KEY_F10             = VK_F10,
    KEY_F11             = VK_F11,
    KEY_F12             = VK_F12,
    KEY_ESCAPE          = VK_ESCAPE,
    KEY_TAB             = VK_TAB,
    KEY_SHIFT           = VK_SHIFT,
    KEY_CTRL            = VK_CONTROL,
    KEY_ALT             = VK_MENU,
    KEY_UP              = VK_UP,
    KEY_DOWN            = VK_DOWN,
    KEY_LEFT            = VK_LEFT,
    KEY_RIGHT           = VK_RIGHT,
    KEY_BACKSPACE       = VK_BACK,
    KEY_ENTER           = VK_RETURN,
    KEY_PAUSE           = VK_PAUSE,
    KEY_CAPSLOCK        = VK_CAPITAL,
    KEY_PAGEUP          = VK_PRIOR,
    KEY_PAGEDOWN        = VK_NEXT,
    KEY_END             = VK_END,
    KEY_HOME            = VK_HOME,
    KEY_PRINTSCREEN     = VK_SNAPSHOT,
    KEY_INSERT          = VK_INSERT,
    KEY_DELETE          = VK_DELETE,
    KEY_NUMPAD0         = VK_NUMPAD0,
    KEY_NUMPAD1         = VK_NUMPAD1,
    KEY_NUMPAD2         = VK_NUMPAD2,
    KEY_NUMPAD3         = VK_NUMPAD3,
    KEY_NUMPAD4         = VK_NUMPAD4,
    KEY_NUMPAD5         = VK_NUMPAD5,
    KEY_NUMPAD6         = VK_NUMPAD6,
    KEY_NUMPAD7         = VK_NUMPAD7,
    KEY_NUMPAD8         = VK_NUMPAD8,
    KEY_NUMPAD9         = VK_NUMPAD9,
    KEY_NUMPAD_MULTIPLY = VK_MULTIPLY,
    KEY_NUMPAD_ADD      = VK_ADD,
    KEY_NUMPAD_SUBTRACT = VK_SUBTRACT,
    KEY_NUMPAD_PERIOD   = VK_DECIMAL,
    KEY_NUMPAD_DIVIDE   = VK_DIVIDE,
    KEY_SPACE           = VK_SPACE,
    KEY_COMMA           = VK_OEM_COMMA,
    KEY_PERIOD          = VK_OEM_PERIOD,
    KEY_DASH            = VK_OEM_MINUS,
    KEY_EQUAL           = VK_OEM_PLUS,

    // these are only good in the US of A...
    KEY_SEMICOLON       = VK_OEM_1,
    KEY_SLASH           = VK_OEM_2,
    KEY_TILDE           = VK_OEM_3,
    KEY_LBRACKET        = VK_OEM_4,
    KEY_BACKSLASH       = VK_OEM_5,
    KEY_RBRACKET        = VK_OEM_6,
    KEY_QUOTE           = VK_OEM_7,
    KEY_UNMAPPED        = 0xffffffff,
};


#elif HS_BUILD_FOR_UNIX

/**
 * These values match the USB HID specification.
 *
 * They correspond to physical key positions on the keyboard, NOT to the
 * value or label of those keys. The names given here are for a standard US
 * QWERTY keyboard.
 *
 * With another keyboard layout (such as AZERTY), KEY_A would correspond to the
 * key labeled Q.
 */
enum plKeyDef
{
    KEY_A               = 4,
    KEY_B               = 5,
    KEY_C               = 6,
    KEY_D               = 7,
    KEY_E               = 8,
    KEY_F               = 9,
    KEY_G               = 10,
    KEY_H               = 11,
    KEY_I               = 12,
    KEY_J               = 13,
    KEY_K               = 14,
    KEY_L               = 15,
    KEY_M               = 16,
    KEY_N               = 17,
    KEY_O               = 18,
    KEY_P               = 19,
    KEY_Q               = 20,
    KEY_R               = 21,
    KEY_S               = 22,
    KEY_T               = 23,
    KEY_U               = 24,
    KEY_V               = 25,
    KEY_W               = 26,
    KEY_X               = 27,
    KEY_Y               = 28,
    KEY_Z               = 29,

    KEY_1               = 30,
    KEY_2               = 31,
    KEY_3               = 32,
    KEY_4               = 33,
    KEY_5               = 34,
    KEY_6               = 35,
    KEY_7               = 36,
    KEY_8               = 37,
    KEY_9               = 38,
    KEY_0               = 39,

    KEY_ENTER           = 40,
    KEY_ESCAPE          = 41,
    KEY_BACKSPACE       = 42,
    KEY_TAB             = 43,
    KEY_SPACE           = 44,
    KEY_DASH            = 45,
    KEY_EQUAL           = 46,
    KEY_LBRACKET        = 47,
    KEY_RBRACKET        = 48,
    KEY_BACKSLASH       = 49,
    KEY_SEMICOLON       = 51,
    KEY_QUOTE           = 52,
    KEY_TILDE           = 53,
    KEY_COMMA           = 54,
    KEY_PERIOD          = 55,
    KEY_SLASH           = 56,
    KEY_CAPSLOCK        = 57,

    KEY_F1              = 58,
    KEY_F2              = 59,
    KEY_F3              = 60,
    KEY_F4              = 61,
    KEY_F5              = 62,
    KEY_F6              = 63,
    KEY_F7              = 64,
    KEY_F8              = 65,
    KEY_F9              = 66,
    KEY_F10             = 67,
    KEY_F11             = 68,
    KEY_F12             = 69,

    KEY_PRINTSCREEN     = 70,
    KEY_PAUSE           = 72,
    KEY_INSERT          = 73,
    KEY_HOME            = 74,
    KEY_PAGEUP          = 75,
    KEY_DELETE          = 76,
    KEY_END             = 77,
    KEY_PAGEDOWN        = 78,

    KEY_RIGHT           = 79,
    KEY_LEFT            = 80,
    KEY_DOWN            = 81,
    KEY_UP              = 82,

    KEY_NUMPAD_DIVIDE   = 84,
    KEY_NUMPAD_MULTIPLY = 85,
    KEY_NUMPAD_SUBTRACT = 86,
    KEY_NUMPAD_ADD      = 87,

    KEY_NUMPAD1         = 89,
    KEY_NUMPAD2         = 90,
    KEY_NUMPAD3         = 91,
    KEY_NUMPAD4         = 92,
    KEY_NUMPAD5         = 93,
    KEY_NUMPAD6         = 94,
    KEY_NUMPAD7         = 95,
    KEY_NUMPAD8         = 96,
    KEY_NUMPAD9         = 97,
    KEY_NUMPAD0         = 98,
    KEY_NUMPAD_PERIOD   = 99,

    KEY_CTRL            = 224,
    KEY_SHIFT           = 225,
    KEY_ALT             = 226,
    KEY_GUI             = 227,

    KEY_RCTRL           = 228,
    KEY_RSHIFT          = 229,
    KEY_RALT            = 230,
    KEY_RGUI            = 231,

    KEY_UNMAPPED        = 255
};
#endif

#endif // plKeyDef_inc
