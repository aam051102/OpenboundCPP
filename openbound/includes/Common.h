#ifndef SBURB_COMMON_H
#define SBURB_COMMON_H

#include <vector>
#include <string>
#include <stdint.h>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <pugixml.hpp>
#include <iostream>
#include <filesystem>
#include <map>

const std::map<char, bool> specialCharsIgnore = {
    {'a', true}
};

namespace SBURB
{
    class Sprite;

    // Vector 2
    struct Vector2
    {
        int x;
        int y;

        Vector2() : x(0), y(0) {};
        Vector2(int x, int y) : x(x), y(y) {};
    };

    // Vector 4
    struct Vector4
    {
        int x;
        int y;
        int z;
        int w;

        Vector4() : x(0), y(0), z(0), w(0){};
        Vector4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w){};
    };

    // Transform
    struct Transform
    {
        sf::Vector2f position;
        int rotation;

        Transform() : position({0, 0}), rotation(0){};
        Transform(sf::Vector2f position, int rotation) : position(position), rotation(rotation){};
    };


    /*
    Source 1: https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
    */
    static inline std::wstring unescape(const std::wstring& string) {
        std::wstring ret;
        wchar_t ch;
        int ii;

        for (size_t i = 0, len = string.size(); i < len; i++) {
            if (string[i] != '%') {
                if (string[i] == '+')
                    ret += ' ';
                else
                    ret += string[i];
            }
            else {
                if (swscanf(string.substr(i + 1, 2).c_str(), L"%x", &ii) == EOF) {
                    throw std::runtime_error("Failed to scan escaped unicode data");
                }

                ch = static_cast<wchar_t>(ii);
                ret += ch;
                i = i + 2;
            }
        }

        return ret;

       // NOTE: This is my attempt at implementing the function according to the actual spec
       /*std::wstring R = L"";

        for (size_t k = 0, len = string.size(); k < len; k++) {
            auto C = string[k];

            if (C == L'%') {
                std::wstring hexDigits = L"";
                size_t optionalAdvance = 0;

                if (k + 5 < len && string[k + 1] == L'u') {
                    hexDigits = string.substr(k + 2, 4);
                    optionalAdvance = 5;
                }
                else if(k + 3 <= len) {
                    hexDigits = string.substr(k + 1, 2);
                    optionalAdvance = 2;
                }

                auto parseResult = ;

                C = static_cast<wchar_t>(parseResult);
                k += optionalAdvance;
            }

            R += C;
        }

        return R;*/
    }

    /*
    Source 1: https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
    Source 2: https://stackoverflow.com/questions/23689733/convert-string-from-utf-8-to-iso-8859-1 - no longer used
    */
    static inline std::wstring escape(const std::wstring& string) {
        std::wstring new_str = L"";
        wchar_t c;
        int ic;
        const wchar_t* chars = string.c_str();
        wchar_t bufHex[10];
        size_t len = wcslen(chars);

        for (int i = 0; i < len; i++) {
            c = chars[i];
            ic = c;
            // uncomment this if you want to encode spaces with +
            /*if (c==' ') new_str += '+';
            else */if (isalnum(c) || c == L'-' || c == L'_' || c == L'.' || c == L'~') new_str += c;
            else {
                swprintf(bufHex, L"%X", c);
                if (ic < 16)
                    new_str += L"%0";
                else
                    new_str += L"%";
                new_str += bufHex;
            }
        }

        return new_str;

        /*std::wostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;
        if (in == NULL)
            return escaped.str();

        unsigned int codepoint = 0;
        while (*in != 0)
        {
            wchar_t ch = static_cast<wchar_t>(*in);
            if (ch <= 0x7f) {
                codepoint = ch;
            }
            else if (ch <= 0xbf) {
                codepoint = (codepoint << 6) | (ch & 0x3f);
            }
            else if (ch <= 0xdf) {
                codepoint = ch & 0x1f;
            }
            else if (ch <= 0xef) {
                codepoint = ch & 0x0f;
            }
            else {
                codepoint = ch & 0x07;
            }

            in++;

            if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
            {
                if (codepoint <= 255)
                {
                    const wchar_t c = static_cast<char>(codepoint);

                    // Keep alphanumeric and other accepted characters intact
                    if (isalnum(c) || c == '_' || c == '@' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/') {
                        escaped << c;
                        continue;
                    }

                    // Any other characters are percent-encoded
                    escaped << std::setfill('0') << std::uppercase;
                    escaped << "%";
                    escaped << std::setw(2);
                    escaped << codepoint;
                    escaped << std::nouppercase;
                }
                else
                {
                    // out-of-bounds
                    escaped << std::setfill('0') << std::uppercase;
                    escaped << "%u";
                    escaped << std::setw(4);
                    escaped << codepoint;
                    escaped << std::nouppercase;
                }
            }
        }

        return escaped.str();*/
    }

    /*
    Source: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    */
    static inline std::vector<std::wstring> split(const std::wstring& s, const std::wstring& delimiter, int max = -1) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::wstring token;
        std::vector<std::wstring> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::wstring::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);

            if (max != -1 && res.size() == max) {
                break;
            }
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    static inline std::wstring trim(const std::wstring& s) {
        const int len = s.length();

        int start = 0;
        int end = len - 1;

        for (int i = 0; i < len; i++) {
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {
                start = i;
                break;
            }
        }

        for (int i = len - 1; i >= start; i--) {
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {
                end = i;
                break;
            }
        }

        std::wstring newStr = L"";

        for (int i = start; i <= end; i++) {
            newStr += s[i];
        }

        return newStr;
    }

    static inline std::wstring replace(std::wstring s, const std::wstring& oldSubstr, const std::wstring& newSubstr) {
        size_t pos = 0;
        
        while ((pos = s.find(oldSubstr, pos)) != std::wstring::npos) {
            s.replace(s.begin() + pos, s.begin() + pos + oldSubstr.size(), newSubstr.c_str());
            pos += newSubstr.size();
        }

        return s;
    }

    static inline std::vector<pugi::xml_node> GetNestedChildren(pugi::xml_node* node, const std::wstring& tagName)
    {
        std::vector<pugi::xml_node> nodes = {};

        for (auto child : node->children()) {            
            if (std::wstring(child.name()) == tagName) {
                nodes.push_back(child);
            }

            auto childsChildren = GetNestedChildren(&child, tagName);
            nodes.insert(nodes.end(), childsChildren.begin(), childsChildren.end());
        }

        return nodes;
    }

    static inline pugi::xml_node GetNestedChild(pugi::xml_node* node, const std::wstring& tagName)
    {
        for (auto child : node->children()) {
            if (std::wstring(child.name()) == tagName) {
                return child;
            }
            else {
                auto childData = GetNestedChild(&child, tagName);
                if (childData) return childData;
            }
        }

        return pugi::xml_node();
    }
}

uint16_t swapbits(uint16_t src);
uint32_t swapbits(uint32_t src);
uint64_t swapbits(uint64_t src);

std::filesystem::path GetAppDataDirectory(std::string appendedPath = "");
std::string GetExecutableDirectory();

#endif