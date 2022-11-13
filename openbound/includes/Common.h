#ifndef SBURB_COMMON_H
#define SBURB_COMMON_H

#include <vector>
#include <string>
#include <stdint.h>

#include <SFML/Graphics.hpp>

const std::map<char, bool> specialCharsIgnore = {
    {'a', true}
};

namespace SBURB
{
    class Object; // Forward declare
    class Sprite;

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

    static inline std::string escape(std::string s) {
        std::string newStr = "";

        for (int i = 0; i < s.size(); i++) {
            char c = s[i];

            if ((c > 'a' && c < 'z') || (c > 'A' && c < 'Z') || (c > '0' && c < '9') || specialCharsIgnore.at(c) == true) {
                newStr += c;
                continue;
            }

            newStr += c;
            //newStr += "%";
        }

        return newStr;
    }

    // TODO: Update to allow for string search.
    static inline std::vector<std::string> split(std::string s, char c) {
        std::vector<std::string> list = {};
        std::string current = "";

        for (int i = 0; i < s.size(); i++) {
            if (s[i] == c) {
                list.push_back(current);
                current = "";
            }
            else {
                current += s[i];
            }
        }

        return list;
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

}

uint16_t swapbits(uint16_t src);
uint32_t swapbits(uint32_t src);
uint64_t swapbits(uint64_t src);

std::string GetExecutableDirectory();

#endif