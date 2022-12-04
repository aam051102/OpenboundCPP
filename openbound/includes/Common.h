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

    // Vector 4 Float
    struct Vector4f
    {
        float x;
        float y;
        float z;
        float w;

        Vector4f() : x(0), y(0), z(0), w(0){};
        Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){};
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
    TODO
    */
    static inline const char* unescape(std::string in) {
        return "";
        /*std::basic_ostringstream<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char>> unescaped;
        unescaped.fill('0');
        unescaped << std::hex;
        
        return unescaped.str().c_str();*/
    }

    /*
    Source 1: https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
    Source 2: https://stackoverflow.com/questions/23689733/convert-string-from-utf-8-to-iso-8859-1
    */
    static inline std::string escape(const char* in) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;
        if (in == NULL)
            return escaped.str();

        unsigned int codepoint = 0;
        while (*in != 0)
        {
            unsigned char ch = static_cast<unsigned char>(*in);
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
                    const char c = static_cast<char>(codepoint);

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

        return escaped.str();
    }

    /*
    Source: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    */
    static inline std::vector<std::string> split(std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    static inline std::string trim(std::string s) {
        const int len = s.length();

        int start = 0;
        int end = len - 1;

        for (int i = 0; i < len; i++) {
            if (s[i] != ' ') {
                start = i;
                break;
            }
        }

        for (int i = len - 1; i >= start; i--) {
            if (s[i] != ' ') {
                end = i;
                break;
            }
        }

        std::string newStr = "";

        for (int i = start; i <= end; i++) {
            newStr += s[i];
        }

        return newStr;
    }

    static inline std::string replace(std::string s, std::string oldSubstr, std::string newSubstr) {
        // TODO
        return "";
    }

    static inline std::vector<pugi::xml_node> GetNestedChildren(pugi::xml_node* node, std::string tagName)
    {
        std::vector<pugi::xml_node> nodes = {};

        for (auto child : node->children()) {            
            if (child.name() == tagName) {
                nodes.push_back(child);
            }
            else {
                auto childsChildren = GetNestedChildren(&child, tagName);
                nodes.insert(nodes.end(), childsChildren.begin(), childsChildren.end());
            }
        }

        return nodes;
    }

    static inline pugi::xml_node GetNestedChild(pugi::xml_node* node, std::string tagName)
    {
        for (auto child : node->children()) {
            if (child.name() == tagName) {
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

std::string GetExecutableDirectory();

#endif