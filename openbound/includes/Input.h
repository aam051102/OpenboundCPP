#ifndef SBURB_INPUT_H
#define SBURB_INPUT_H

#include "Window.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <unordered_map>
#include <functional>
#include <map>
#include <array>
#include <vector>

namespace SBURB
{
    class InputHandler
    {
        friend class Sburb;

    public:
        static std::map<sf::Keyboard::Key, bool> GetPressed();
        static bool GetPressed(sf::Keyboard::Key key);
        static void SetPressed(sf::Keyboard::Key key, bool value);

        static std::vector<sf::Keyboard::Key> GetPressedOrder();
        void AddToPressedOrder(sf::Keyboard::Key key);
        void RemoveFromPressedOrder(sf::Keyboard::Key key);

        static void OnKeyDown(sf::Keyboard::Key key);
        static void OnKeyUp(sf::Keyboard::Key key);
        static void OnMouseDown();
        static void OnMouseUp();

        static sf::Vector2i GetMousePosition();
        static bool GetMouseDown();

        void Update(sf::Event e, bool focused);

    private:
        InputHandler();

        std::vector<sf::Keyboard::Key> pressedOrder;
        std::map<sf::Keyboard::Key, bool> pressed;
        bool mouseDown;
    };
}

#endif
