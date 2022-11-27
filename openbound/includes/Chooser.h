#ifndef SBURB_CHOOSER_H
#define SBURB_CHOOSER_H

#include "Common.h"
#include "Action.h"
#include "FontEngine.h"
#include <SFML/Graphics/Drawable.hpp>

namespace SBURB {
    class Chooser : sf::Drawable {
    public:
        Chooser();
        ~Chooser();

        void SetChoices(std::vector<Action> choices) { this->choices = choices; };

        void NextChoice();
        void PrevChoice();
        void BeginChoosing(int x, int y);

        void Update();

    private:
        bool choosing;
        std::vector<Action> choices;
        int choice = 0;
        std::vector<FontEngine> dialogs;
        int time = 0;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif