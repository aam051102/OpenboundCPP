// Engine
#include <Common.h>
#include <Game.h>
#include <Logger.h>

using namespace SBURB;

int main()
{
    Game mainGame = Game();
  
    if (!mainGame.Start())
    {
        GlobalLogger->Log(Logger::Error, "Failed to initialize game object.");
        return -1;
    }

    return 0;
}