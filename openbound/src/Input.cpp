#include "Input.h"

namespace SBURB
{
    static InputHandler* inputHandlerInstance;

    typedef std::function<void(sf::Event::KeyEvent)> CallbackSignature;
    InputHandler::InputHandler()
    {
        keyboardAliases = {
            {InputActions::Confirm, {sf::Keyboard::Space}},
            {InputActions::Back, {sf::Keyboard::X, sf::Keyboard::LShift}},
            {InputActions::Menu, {sf::Keyboard::C, sf::Keyboard::LControl}},
            {InputActions::Skip, {sf::Keyboard::X, sf::Keyboard::LShift}},
            {InputActions::Exit, {sf::Keyboard::Escape, sf::Keyboard::Escape}},
            {InputActions::Up, {sf::Keyboard::W, sf::Keyboard::Up}},
            {InputActions::Left, {sf::Keyboard::A, sf::Keyboard::Left}},
            {InputActions::Right, {sf::Keyboard::D, sf::Keyboard::Right}},
            {InputActions::Down, {sf::Keyboard::S, sf::Keyboard::Down}},
            {InputActions::Attack, {sf::Keyboard::Space, sf::Keyboard::Enter, sf::Keyboard::LControl}}};

        mouseAliases = {
           {MouseInputActions::Click, {sf::Mouse::Left}}
        };

        gamepadAliases = {
            {InputActions::Confirm, {1, 1}},
            {InputActions::Back, {2, 2}},
            {InputActions::Menu, {4, 9}},
            {InputActions::Skip, {3, 2}},
            {InputActions::Exit, {10, 10}},
            {InputActions::Up, {LSAXIS_NEGY, 12}},
            {InputActions::Down, {LSAXIS_POSY, 13}},
            {InputActions::Left, {LSAXIS_NEGX, 14}},
            {InputActions::Right, {LSAXIS_POSX, 15}}};

        callbacks = std::map<InputState, std::vector<CallbackSignature>>{
            {InputState::Pressed, std::vector<CallbackSignature>()},
            {InputState::Released, std::vector<CallbackSignature>()}};

        inputHandlerInstance = this;
    }

    void InputHandler::Update(sf::Event event, bool focused)
    {
        if (focused)
            return;

        if (event.type == sf::Event::KeyPressed)
        {
            for (auto &callback : callbacks[InputState::Pressed])
            {
                callback(event.key);
            }
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            for (auto &callback : callbacks[InputState::Released])
            {
                callback(event.key);
            }
        }

        std::map<InputActions, bool> actionHandled =
            {
                {InputActions::Confirm, false},
                {InputActions::Back, false},
                {InputActions::Menu, false},
                {InputActions::Skip, false},
                {InputActions::Exit, false},
                {InputActions::Up, false},
                {InputActions::Down, false},
                {InputActions::Left, false},
                {InputActions::Right, false}};

        for (auto &&keyAlias : keyboardAliases)
        {
            if (actionHandled[keyAlias.first])
                continue;

            bool isPressed = false;

            for (auto alias : keyAlias.second) {
                if (sf::Keyboard::isKeyPressed(alias)) {
                    isPressed = true;
                    break;
                }
            }

            if (isPressed)
            {
                if (keyStates[keyAlias.first] != InputState::Pressed && keyStates[keyAlias.first] != InputState::Held)
                {
                    keyStates[keyAlias.first] = InputState::Pressed;
                }
                actionHandled[keyAlias.first] = true;
            }
            else
            {
                if (keyStates[keyAlias.first] == InputState::Released)
                {
                    keyStates[keyAlias.first] = InputState::None;
                }
                else
                {
                    keyStates[keyAlias.first] = InputState::Released;
                }
                actionHandled[keyAlias.first] = true;
            }
        }

        if (sf::Joystick::isConnected(0))
        {
            for (auto &&gamepadAlias : gamepadAliases)
            {
                if (actionHandled[gamepadAlias.first])
                    continue;

                bool isPressed = false;
                for (auto alias : gamepadAlias.second) {
                    if (alias >= LSAXIS_NEGX)
                    {
                        switch (alias)
                        {
                        case LSAXIS_NEGX:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::X) < -10;
                            break;
                        case LSAXIS_POSX:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::X) > 10;
                            break;
                        case LSAXIS_NEGY:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::Y) < -10;
                            break;
                        case LSAXIS_POSY:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::Y) > 10;
                            break;
                        case RSAXIS_NEGX:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) < -10;
                            break;
                        case RSAXIS_POSX:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) > 10;
                            break;
                        case RSAXIS_NEGY:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) < -10;
                            break;
                        case RSAXIS_POSY:
                            isPressed = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) > 10;
                            break;
                        }
                    }
                    else
                    {
                        isPressed = sf::Joystick::isButtonPressed(0, alias);
                    }

                    if (isPressed) break;
                }

                if (isPressed)
                {
                    if (keyStates[gamepadAlias.first] != InputState::Pressed && keyStates[gamepadAlias.first] != InputState::Held)
                    {
                        keyStates[gamepadAlias.first] = InputState::Pressed;
                    }
                    actionHandled[gamepadAlias.first] = true;
                }
                else
                {
                    if (keyStates[gamepadAlias.first] == InputState::Released)
                    {
                        keyStates[gamepadAlias.first] = InputState::None;
                    }
                    else
                    {
                        keyStates[gamepadAlias.first] = InputState::Released;
                    }
                    actionHandled[gamepadAlias.first] = true;
                }
            }
        }

        for (auto mouseAction : this->mouseAliases) {
            bool isPressed = false;

            for (auto alias : mouseAction.second) {
                if (sf::Mouse::isButtonPressed(alias)) {
                    isPressed = true;
                    break;
                }
            }

            if (isPressed)
            {
                if (this->mouseButtonStates[mouseAction.first] != InputState::Pressed && this->mouseButtonStates[mouseAction.first] != InputState::Held)
                {
                    this->mouseButtonStates[mouseAction.first] = InputState::Pressed;
                }
            }
            else
            {
                if (this->mouseButtonStates[mouseAction.first] == InputState::Released)
                {
                    this->mouseButtonStates[mouseAction.first] = InputState::None;
                }
                else
                {
                    this->mouseButtonStates[mouseAction.first] = InputState::Released;
                }
            }
        }
    }

    bool InputHandler::IsInputPressed(InputActions input)
    {
        if (inputHandlerInstance->keyStates[input] == InputState::Pressed)
        {
            inputHandlerInstance->keyStates[input] = InputState::Held;
            return true;
        }
        return false;
    }

    bool InputHandler::IsInputHeld(InputActions input)
    {
        return inputHandlerInstance->keyStates[input] == InputState::Pressed || inputHandlerInstance->keyStates[input] == InputState::Held;
    }

    bool InputHandler::IsInputReleased(InputActions input)
    {
        return inputHandlerInstance->keyStates[input] == InputState::Released;
    }

    sf::Vector2i InputHandler::GetMousePosition() {
        return sf::Mouse::getPosition();
    }

    bool InputHandler::IsMousePressed(MouseInputActions button)
    {
        if (inputHandlerInstance->mouseButtonStates[button] == InputState::Pressed)
        {
            inputHandlerInstance->mouseButtonStates[button] = InputState::Held;
            return true;
        }
        return false;
    }

    bool InputHandler::IsMouseHeld(MouseInputActions button)
    {
        return inputHandlerInstance->mouseButtonStates[button] == InputState::Pressed || inputHandlerInstance->mouseButtonStates[button] == InputState::Held;
    }

    bool InputHandler::IsMouseReleased(MouseInputActions button)
    {
        return inputHandlerInstance->mouseButtonStates[button] == InputState::Released;
    }
}
