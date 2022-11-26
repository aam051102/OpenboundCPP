#ifndef SBURB_COMMAND_HANDLER_H
#define SBURB_COMMAND_HANDLER_H

#include "Common.h"
#include <variant>

namespace SBURB {
    class CommandHandler {
    public:
        CommandHandler();

    private:
        std::map<std::string, std::variant<void, Trigger>> commands;

    };
}

#endif