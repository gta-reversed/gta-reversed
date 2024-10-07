#include "StdInc.h"

#include "GxtChar.h"

// 0x718600
void AsciiToGxtChar(const char* src, GxtChar* dst) {
    assert(src && dst);
    auto i = 0u;
    for (; src[i]; ++i) {
        dst[i] = src[i];
    }
    dst[i] = 0;
}

// NOTSA -- Derived from GxtCharToAscii (0x69F7E0)
// FIX_BUGS (sorta): Make caller provide size instead of creating a static buffer.
char* GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) {
    src += offset ? offset : 0;

    static const auto gxtUtf8Map = notsa::make_mapping<uint8_t /*GXT code point*/, const char8_t* /*UTF-8 char*/>({
        { 0xb1, u8"´" }, { 0xaf, u8"¿" }, { 0x80, u8"À" }, { 0x81, u8"Á" }, { 0x82, u8"Â" },
        { 0x83, u8"Ä" }, { 0x84, u8"Æ" }, { 0x85, u8"Ç" }, { 0x86, u8"È" }, { 0x87, u8"É" },
        { 0x88, u8"Ê" }, { 0x89, u8"Ë" }, { 0x8a, u8"Ì" }, { 0x8b, u8"Í" }, { 0x8c, u8"Î" },
        { 0x8d, u8"Ï" }, { 0xad, u8"Ñ" }, { 0x8e, u8"Ò" }, { 0x8f, u8"Ó" }, { 0x90, u8"Ô" },
        { 0x91, u8"Ö" }, { 0x92, u8"Ù" }, { 0x93, u8"Ú" }, { 0x94, u8"Û" }, { 0x95, u8"Ü" },
        { 0x96, u8"ß" }, { 0x97, u8"à" }, { 0x98, u8"á" }, { 0x99, u8"â" }, { 0x9a, u8"ä" },
        { 0x9b, u8"æ" }, { 0x9c, u8"ç" }, { 0x9d, u8"è" }, { 0x9e, u8"é" }, { 0x9f, u8"ê" },
        { 0xa0, u8"ë" }, { 0xa1, u8"ì" }, { 0xa2, u8"í" }, { 0xa3, u8"î" }, { 0xa4, u8"ï" },
        { 0xae, u8"ñ" }, { 0xa5, u8"ò" }, { 0xa6, u8"ó" }, { 0xa7, u8"ô" }, { 0xa8, u8"ö" },
        { 0xa9, u8"ù" }, { 0xaa, u8"ú" }, { 0xab, u8"û" }, { 0xac, u8"ü" }, { 0x5b, u8"<" },
        { 0x5d, u8">" }, { 0x5e, u8"¡" }
    });

    auto out_i = 0u;
    for (auto i = 0u; out_i < size - offset && src && src[i]; i++) {
        if (const char8_t* u8ch = notsa::find_value_or(gxtUtf8Map, src[i], u8"\0"); !*u8ch) {
            if (isascii(src[i])) {
                out[out_i++] = static_cast<char>(src[i]);
            } else {
                NOTSA_UNREACHABLE("Invalid GXT code point '{:X}'", (uint8_t)src[i]);
            }
        } else {
            const auto utf8Size = std::strlen((const char*)u8ch);

            assert(out_i + utf8Size <= size - offset);
            std::memcpy(&out[out_i], u8ch, utf8Size + 1);
            out_i += utf8Size;
        }
    }
    out[out_i] = '\0';

    return out;
}

GxtChar* GxtCharStrcat(GxtChar* dst, const GxtChar* src) {
    GxtChar* target = dst;
    if (*dst) {
        while (target[0])
            ++target;
    }
    if (*src) {
        while (*src)
            *target++ = *src++;
    }
    *target = '\0';
    return dst;
}

uint32 GxtCharStrlen(const GxtChar* str) {
    uint32 len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}

void MakeLowerCase(char* str) {
    char* s = str;
    while (*s) {
        if (*s >= 'A' && *s <= 'Z') {
            *s = std::tolower(*s);
        }
        ++s;
    }
}

// 0x69F910
void TextCopy(GxtChar* dst, const GxtChar* src) {
    while (*src) {
        *dst = *src;
        ++src;
        ++dst;
    }
    *dst = '\0';
}

// 0x718660
GxtChar* GxtCharStrcpy(GxtChar* dst, const GxtChar* src) {
    TextCopy(dst, src);
    return dst;
}
