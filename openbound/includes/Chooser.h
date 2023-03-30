#ifndef SBURB_CHOOSER_H
#define SBURB_CHOOSER_H

#include "Common.h"
#include "Action.h"
#include "FontEngine.h"
#include <SFML/Graphics/Drawable.hpp>

namespace SBURB {
    class Chooser : public sf::Drawable {
    public:
        Chooser();
        ~Chooser();


        void NextChoice();
        void PrevChoice();
        void BeginChoosing(int x, int y);
        
        void SetChoosing(bool choosing) { this->choosing = choosing; };
        bool GetChoosing() { return this->choosing; };

        void SetChoices(std::vector<std::shared_ptr<Action>> choices) { this->choices = choices; };
        void AddChoice(std::shared_ptr<Action> choice) { this->choices.push_back(choice); };
        std::vector<std::shared_ptr<Action>> GetChoices() { return this->choices; };

        std::shared_ptr<Action> GetChoice() { return this->choices[this->choice]; };
        std::shared_ptr<Action> GetChoice(int index) { return this->choices[index]; };

        void Update();

    private:
        bool choosing;
        std::vector<std::shared_ptr<Action>> choices;
        int choice = 0;
        std::vector<FontEngine> dialogs;
        int time = 0;

    private:
        void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    };
}

#endif