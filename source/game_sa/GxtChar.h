#pragma once
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <cstring>

using GxtChar = uint8_t; // 8-bit GXT character.

/**
 * Convert ANSI string to GXT-supported null-terminated string
 *
 * @param   src    pointer to null-terminated source string
 * @param   dst    pointer to destination buffer
 */
void AsciiToGxtChar(const char* src, GxtChar* dst);

/**
 * Convert GXT null terminated string to UTF-8 string.
 * NOTE: encoding bugs can appear!
 * @param   out        buffer to hold encoded UTF-8 string
 * @param   src        pointer to null-terminated source string
 * @param   size       size of the `out` buffer
 * @param   offset     start position
 * @return  pointer to static var containing null-terminated ASCII string
 */
char* GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset = 0);

template<size_t N>
char* GxtCharToUTF8(char (&out)[N], const GxtChar* src, size_t offset = 0) {
    return GxtCharToUTF8(out, src, N, offset);
}

inline size_t CalculateUTF8BufferSize(const GxtChar* gxtStr) {
    size_t len = 0;
    if (gxtStr) {
        while (gxtStr[len] != 0) len++;
        // Modern c++ 20+: dynamic size 4 bytes max per char + null terminator
        return len * 4 + 1 + 64;
    }
    return 0;
}

inline const char* GxtCharToUTF8(const GxtChar* src, size_t offset = 0) {
    static char* utf8Text = nullptr;
    static size_t currentSize = 0;
    
    size_t requiredSize = CalculateUTF8BufferSize(src);
    if (requiredSize > currentSize) {
        delete[] utf8Text;
        utf8Text = new char[requiredSize];
        currentSize = requiredSize;
    }
    
    return GxtCharToUTF8(utf8Text, src, currentSize, offset);
};

/**
 * Concatenate null terminated GXT strings
 *
 * @param   dst    pointer to destination buffer containing null-terminated string
 * @param   src    pointer to null-terminated string you wanna add to destination
 */
GxtChar* GxtCharStrcat(GxtChar* dst, const GxtChar* src);

/**
 * Get GXT null terminated string length
 *
 * @param    str  pointer to null-terminated string
 * @return   string length
 */
uint32 GxtCharStrlen(const GxtChar* str);

/**
 * Lowercase null terminated string
 * NOTE: this function does NOT use locale.
 *
 * @param   str  pointer to null-terminated string you wanna lowercase
 */
void MakeLowerCase(char* str);

/**
 * Copy GXT null terminated string (like strcpy)
 *
 * @param   dst    pointer to destination buffer
 * @param   src    pointer to null-terminated string you wanna copy
 */
void TextCopy(GxtChar* dst, const GxtChar* src);

/**
 * Copy GXT null terminated strings
 *
 * @param   dst    pointer to destination buffer containing null-terminated string
 * @param   src    pointer to null-terminated string you wanna copy to destination
 * @return  pointer to destination buffer
 */
GxtChar* GxtCharStrcpy(GxtChar* dst, const GxtChar* src);

/**
 * Use this function to convert ASCII text to GxtChar* easily.
 * NOTE: Use this function only when you're sure that the whole text
 * contains ASCII chars only.
 * 
 * @notsa
 * @param   asciiText  ASCII text
 * @return  Converted text
 */
inline const GxtChar* GxtCharFromAscii(const char* asciiText) {
    return (const GxtChar*)asciiText;
}

/**
 * Use this function to convert GxtChar* to ASCII easily.
 * NOTE: Use this function only when you're sure that the whole text
 * contains ASCII chars only, or when you make comparison or copying.
 *
 * @notsa
 * @param   gxtAsciiText  GXT text that only has ASCII chars.
 * @return  Converted text
 */
inline const char* AsciiFromGxtChar(const GxtChar* gxtAsciiText) {
    return (const char*)gxtAsciiText;
}

inline const GxtChar* operator""_gxt(const char* text, size_t) {
    return GxtCharFromAscii(text);
}
