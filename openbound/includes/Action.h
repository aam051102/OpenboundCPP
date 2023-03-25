#ifndef SBURB_ACTION_H
#define SBURB_ACTION_H

#include <pugixml.hpp>
#include "Common.h"

namespace SBURB
{
    class Action
    {
    public:
        Action(std::wstring command, std::wstring info = L"", std::wstring name = L"", std::wstring sprite = L"", std::shared_ptr<Action> followUp = nullptr, bool noWait = false, bool noDelay = false, uint16_t times = 1, bool soft = false, std::wstring silent = L"");
        ~Action();

        std::shared_ptr<Action> Clone();
        std::wstring Serialize(std::wstring output);

        void SetFollowUp(std::shared_ptr<Action> followUp) { this->followUp = followUp; };

        std::wstring GetSprite() { return this->sprite; };
        std::wstring GetName() { return this->name; };

        void SetTimes(int times) { this->times = times; };
        int GetTimes() { return this->times; };

        std::wstring GetCommand() { return this->command; };

        void SetSoft(bool soft) { this->soft = soft; };
        bool GetSoft() { return this->soft; };

        bool GetNoDelay() { return this->noDelay; };

        void SetSilent(std::wstring silent)
        {
            if (silent == L"" || silent == L"false")
            {
                this->silent = false;
            }
            else
            {
                this->silent = true;
            }

            this->silentCause = silentCause;
        };
        bool GetSilent() { return this->silent; };
        std::wstring GetSilentCause() { return this->silentCause; };

        std::shared_ptr<Action> GetFollowUp() { return this->followUp; };

        std::wstring info;

        bool GetNoWait() { return this->noWait; };

    protected:
        std::shared_ptr<Action> followUp;
        std::wstring sprite;
        std::wstring name;
        std::wstring command;
        bool noWait;
        bool noDelay;
        int times;
        bool soft;
        bool silent;
        std::wstring silentCause;
    };
}
#endif
