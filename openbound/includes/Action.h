#ifndef SBURB_ACTION_H
#define SBURB_ACTION_H

#include <pugixml.hpp>
#include "Common.h"

namespace SBURB
{
    class Action
    {
    public:
        Action(std::string command, std::string info, std::string nam, std::string sprite, std::shared_ptr<Action> followUp = NULL, bool noWait = false, bool noDelay = false, uint16_t times = 1, bool isSoft = false, bool isSilent = false);
        ~Action();

        Action Clone();
        std::string Serialize(std::string output);

        void SetFollowUp(std::shared_ptr<Action> followUp) { this->followUp = followUp; };
      
        std::string GetSprite() { return this->sprite; };
        std::string GetName() { return this->name; };

        std::string info;

    protected:
        std::shared_ptr<Action> followUp;
        std::string sprite;
        std::string name;
        std::string command;
        bool noWait;
        bool noDelay;
        uint16_t times;
        bool isSoft;
        bool isSilent;

    };
}
#endif
