#pragma once

#include <string>

// Konvertiert UTF-16 (wstring) nach UTF-8 (string)
std::string WStringToUTF8(const std::wstring& wstr);

// Konvertiert UTF-8 (string) nach UTF-16 (wstring)
std::wstring UTF8ToWString(const std::string& str);

// Holt den aktuellen Clipboard-Text als UTF-8-String
std::string getClipboard();

// Setzt einen UTF-8-String in die Zwischenablage
bool setClipboard(const std::string& text);

bool clearClipboard();