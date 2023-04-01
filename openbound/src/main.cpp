#include <Common.h>
#include <Sburb.h>
#include <Logger.h>

using namespace SBURB;

int main(int argc, char* argv[])
{
    std::string initFilePath = "";
    if (argc == 2) {
        initFilePath = argv[1];
    }

    Sburb mainGame = Sburb();

    if (!mainGame.Start(initFilePath))
    {
        GlobalLogger->Log(Logger::Error, "Failed to initialize game object.");
        return -1;
    }

    return 0;
}