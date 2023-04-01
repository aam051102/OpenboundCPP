#include "Input.h"
#include "Sburb.h"

namespace SBURB
{
    static InputHandler* inputHandlerInst;

    InputHandler::InputHandler()
    {
        this->mouseDown = false;
        this->pressed = {};
        this->pressedOrder = {};

        inputHandlerInst = this;
    }

    std::map<sf::Keyboard::Key, bool> InputHandler::GetPressed()
    {
        return inputHandlerInst->pressed;
    }

    bool InputHandler::GetPressed(sf::Keyboard::Key key)
    {
        return inputHandlerInst->pressed[key];
    }

    void InputHandler::SetPressed(sf::Keyboard::Key key, bool value)
    {
        inputHandlerInst->pressed[key] = value;
    }

    std::vector<sf::Keyboard::Key> InputHandler::GetPressedOrder()
    {
        return inputHandlerInst->pressedOrder;
    }

    void InputHandler::AddToPressedOrder(sf::Keyboard::Key key) {
        inputHandlerInst->pressedOrder.push_back(key);
    }

    void InputHandler::RemoveFromPressedOrder(sf::Keyboard::Key key) {
        auto pos = std::find(inputHandlerInst->pressedOrder.begin(), inputHandlerInst->pressedOrder.end(), key);

        if (pos != inputHandlerInst->pressedOrder.end()) {
            inputHandlerInst->pressedOrder.erase(pos);
        }
    }

    void InputHandler::Update(sf::Event e, bool focused)
    {
        if (!focused) return;

        // Get mouse position within window bounds
        auto mousePos = sf::Mouse::getPosition(*Sburb::GetInstance()->window.GetRenderWindow());
        inputHandlerInst->realMousePosition = sf::Vector2i(
            (mousePos.x / (double)Sburb::GetInstance()->window->getSize().x - Sburb::GetInstance()->window->getView().getViewport().left) / Sburb::GetInstance()->window->getView().getViewport().width * (double)Sburb::GetInstance()->GetViewSize().x,
            (mousePos.y / (double)Sburb::GetInstance()->window->getSize().y - Sburb::GetInstance()->window->getView().getViewport().top) / Sburb::GetInstance()->window->getView().getViewport().height * (double)Sburb::GetInstance()->GetViewSize().y
        );

        // Get mouse and key states
        if (e.type == sf::Event::MouseButtonPressed) {
            if (e.mouseButton.button == sf::Mouse::Left) {
                inputHandlerInst->OnMouseDown();
            }
        }
        else if (e.type == sf::Event::MouseButtonReleased) {
            if (e.mouseButton.button == sf::Mouse::Left) {
                inputHandlerInst->OnMouseUp();
            }
        }
        else if (e.type == sf::Event::KeyPressed) {
            inputHandlerInst->OnKeyDown(e.key.code);
        }
        else if (e.type == sf::Event::KeyReleased) {
            inputHandlerInst->OnKeyUp(e.key.code);
        }
    }

    void InputHandler::OnKeyDown(sf::Keyboard::Key key) {
        auto sburbInst = Sburb::GetInstance();

        if (sburbInst->GetShouldUpdate() && !sburbInst->GetInputDisabled()) { // Make sure we are loaded before trying to do things
            auto chooser = sburbInst->GetChooser();

            if (chooser->GetChoosing()) {
                if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
                    chooser->NextChoice();
                }

                if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
                    chooser->PrevChoice();
                }

                if (key == sf::Keyboard::Space && !inputHandlerInst->pressed[sf::Keyboard::Space]) {
                    sburbInst->PerformAction(chooser->GetChoice());
                    chooser->SetChoosing(false);
                }
            }
            else if (sburbInst->GetDialoger()->GetTalking()) {
                if (key == sf::Keyboard::Space && !inputHandlerInst->pressed[sf::Keyboard::Space]) {
                    sburbInst->GetDialoger()->Nudge();
                }
            }
            else if (sburbInst->HasControl()) {
                if (key == sf::Keyboard::Space && !inputHandlerInst->pressed[sf::Keyboard::Space] && sburbInst->GetEngineMode() == L"wander") {
                    sburbInst->GetChooser()->SetChoices({});
                    auto queries = sburbInst->GetCharacter()->GetActionQueries();

                    for (auto query : queries) {
                        chooser->SetChoices(sburbInst->GetCurrentRoom()->QueryActions(sburbInst->GetCharacter(), query.x, query.y));

                        if (chooser->GetChoices().size() > 0) {
                            break;
                        }
                    }

                    if (sburbInst->GetChooser()->GetChoices().size() > 0) {
                        sburbInst->GetChooser()->AddChoice(std::make_shared<Action>(L"cancel", L"cancel", L"Cancel."));
                        sburbInst->BeginChoosing();
                    }
                }
            }
        }

        if (!inputHandlerInst->GetPressed(key)) inputHandlerInst->AddToPressedOrder(key);
        inputHandlerInst->SetPressed(key, true);
    }

    void InputHandler::OnKeyUp(sf::Keyboard::Key key) {
        if (inputHandlerInst->GetPressed(key)) inputHandlerInst->RemoveFromPressedOrder(key);
        inputHandlerInst->SetPressed(key, false);
    }

    void InputHandler::OnMouseDown() {
        auto sburbInst = Sburb::GetInstance();

        if (!sburbInst->GetShouldUpdate()) return;
        if (sburbInst->GetEngineMode() == L"strife" && sburbInst->HasControl()) {
            sburbInst->GetChooser()->SetChoices(sburbInst->GetCurrentRoom()->QueryActionsVisual(sburbInst->GetCharacter(), sburbInst->GetViewPos().x + inputHandlerInst->GetMousePosition().x, sburbInst->GetViewPos().y + inputHandlerInst->GetMousePosition().y));

            if (sburbInst->GetChooser()->GetChoices().size() > 0) {
                sburbInst->GetChooser()->AddChoice(std::make_shared<Action>(L"cancel", L"cancel", L"cancel"));
                sburbInst->BeginChoosing();
            }
        }

        inputHandlerInst->mouseDown = true;
    }

    void InputHandler::OnMouseUp() {
        inputHandlerInst->mouseDown = false;

        auto sburbInst = Sburb::GetInstance();

        if (!sburbInst->GetShouldUpdate()) return;
        if (sburbInst->GetDialoger() && sburbInst->GetDialoger()->GetBox() && sburbInst->GetDialoger()->GetBox()->IsVisuallyUnder(inputHandlerInst->GetMousePosition().x, inputHandlerInst->GetMousePosition().y)) {
            sburbInst->GetDialoger()->Nudge();
        }
    }

    sf::Vector2i InputHandler::GetMousePosition() {
        return inputHandlerInst->realMousePosition;
    }

    bool InputHandler::GetMouseDown()
    {
        return inputHandlerInst->mouseDown;
    }

}
