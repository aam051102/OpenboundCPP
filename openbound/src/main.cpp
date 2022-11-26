#include <Common.h>
#include <Sburb.h>
#include <Logger.h>

using namespace SBURB;

int main()
{
    Sburb mainGame = Sburb();
  
    if (!mainGame.Start())
    {
        GlobalLogger->Log(Logger::Error, "Failed to initialize game object.");
        return -1;
    }

    return 0;
}