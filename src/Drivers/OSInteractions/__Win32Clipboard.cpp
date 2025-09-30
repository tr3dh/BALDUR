#include "__Win32Clipboard.h"

#include <windows.h>

// Konvertiert UTF-16 (wstring) nach UTF-8 (string)
std::string WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                                         (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                        (int)wstr.size(), result.data(), sizeNeeded, nullptr, nullptr);
    return result;
}

// Konvertiert UTF-8 (string) nach UTF-16 (wstring)
std::wstring UTF8ToWString(const std::string& str)
{
    if (str.empty()) return {};
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                         (int)str.size(), nullptr, 0);
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                        (int)str.size(), result.data(), sizeNeeded);
    return result;
}

// Holt den aktuellen Clipboard-Text als UTF-8-String
std::string getClipboard()
{
    std::string result = "";

    if (!OpenClipboard(nullptr)) {
        return {""};
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData) {
        wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
        if (pszText) {
            result = WStringToUTF8(pszText);
            GlobalUnlock(hData);
        }
    }

    CloseClipboard();
    return result;
}

// Setzt einen UTF-8-String in die Zwischenablage
bool setClipboard(const std::string& text)
{
    if (!OpenClipboard(nullptr))
        return false;

    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    // UTF-8 → UTF-16 konvertieren
    std::wstring wtext = UTF8ToWString(text);

    // Speicher allokieren (inkl. Nullterminator)
    size_t sizeInBytes = (wtext.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    // Daten kopieren
    memcpy(GlobalLock(hMem), wtext.c_str(), sizeInBytes);
    GlobalUnlock(hMem);

    // In Clipboard schreiben
    SetClipboardData(CF_UNICODETEXT, hMem);

    CloseClipboard();
    return true;
}

bool clearClipboard()
{
    if (!OpenClipboard(nullptr))
        return false;

    // löscht den kompletten Inhalt
    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}