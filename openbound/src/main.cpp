// Engine
#include <Common.h>
#include <Game.h>
#include <Window.h>
#include <Object.h>
#include <Logger.h>
#include <Resources.h>
#include <Camera.h>
#include <Sprite.h>
#include <TileMap.h>
#include <AssetHandler.h>
#include <Player.h>
#include <CollisionHandler.h>
#include <SaveHandler.h>
#include <Collidable.h>
#include <ViewZone.h>

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