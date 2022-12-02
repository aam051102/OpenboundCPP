#ifndef SBURB_INPUT_H
#define SBURB_INPUT_H

#include "Window.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <unordered_map>
#include <functional>
#include <map>
#include <array>
#include <vector>

#define LSAXIS_NEGX 101
#define LSAXIS_NEGY 102
#define LSAXIS_POSX 111
#define LSAXIS_POSY 112
#define RSAXIS_NEGX 201
#define RSAXIS_NEGY 202
#define RSAXIS_POSX 211
#define RSAXIS_POSY 212

namespace SBURB
{
    enum class InputActions
    {
        Confirm,
        Back,
        Menu,
        Skip,
        Exit,
        Up,
        Down,
        Left,
        Right,
        Attack
    };

    enum class MouseInputActions
    {
        Click
    };

    enum class InputState
    {
        None,
        Pressed,
        Held,
        Released
    };

    class InputHandler
    {
        friend class Sburb;

    public:
        static bool IsInputPressed(InputActions input);
        static bool IsInputHeld(InputActions input);
        static bool IsInputReleased(InputActions input);

        static sf::Vector2i GetMousePosition();
        static bool IsMousePressed(MouseInputActions button);
        static bool IsMouseHeld(MouseInputActions button);
        static bool IsMouseReleased(MouseInputActions button);

        void Update(sf::Event event, bool focused);

        static std::vector<InputActions> GetKeyOrder();

    private:
        InputHandler();

        std::unordered_map<InputActions, std::vector<sf::Keyboard::Key>> keyboardAliases;
        std::unordered_map<InputActions, std::vector<unsigned int>> gamepadAliases;
        std::map<InputActions, InputState> keyStates;
        std::vector<InputActions> keyOrder;

        std::unordered_map<MouseInputActions, std::vector<sf::Mouse::Button>> mouseAliases;
        std::map<MouseInputActions, InputState> mouseButtonStates;

        std::map<InputState, std::vector<std::function<void(sf::Event::KeyEvent)>>> callbacks;
    };
}

#endif
