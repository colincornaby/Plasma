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

#include "plClipboard.h"
#include "hsWindows.h"
#include <string_theory/string>

#include <memory>

plClipboard& plClipboard::GetInstance()
{
    static plClipboard theInstance;
    return theInstance;
}

bool plClipboard::IsTextInClipboard() 
{
#ifdef HS_BUILD_FOR_WIN32
    return ::IsClipboardFormatAvailable(CF_UNICODETEXT);
#else
    return false;
#endif
}

ST::string plClipboard::GetClipboardText()
{
    if (!IsTextInClipboard()) 
        return ST::string();

#ifdef HS_BUILD_FOR_WIN32
    if (!::OpenClipboard(NULL))
        return ST::string();

    HANDLE clipboardData = ::GetClipboardData(CF_UNICODETEXT);
    size_t size = ::GlobalSize(clipboardData) / sizeof(wchar_t);
    wchar_t* clipboardDataPtr = (wchar_t*)::GlobalLock(clipboardData);

    ST::string result = ST::string::from_wchar(clipboardDataPtr, size);

    ::GlobalUnlock(clipboardData);	
    ::CloseClipboard();

    return result;
#else
    return ST::string();
#endif	
}

void plClipboard::SetClipboardText(const ST::string& text)
{
    if (text.empty())
        return;
#ifdef HS_BUILD_FOR_WIN32
    ST::wchar_buffer buf = text.to_wchar();
    size_t len = buf.size();

    if (len == 0) 
        return;

    std::unique_ptr<void, HGLOBAL(WINAPI*)(HGLOBAL)> copy(::GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(wchar_t)), ::GlobalFree);
    if (!copy)
        return;

    if (!::OpenClipboard(NULL))
        return;

    ::EmptyClipboard();

    wchar_t* target = (wchar_t*)::GlobalLock(copy.get());
    memcpy(target, buf.data(), (len + 1) * sizeof(wchar_t));
    target[len] = '\0';
    ::GlobalUnlock(copy.get());

    ::SetClipboardData(CF_UNICODETEXT, copy.get());
    ::CloseClipboard();
#endif
}

