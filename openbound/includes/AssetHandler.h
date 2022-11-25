#ifndef SBURB_ASSET_HANDLER_H
#define SBURB_ASSET_HANDLER_H

#include "Logger.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>
#include "AssetTexture.h"

namespace SBURB
{
    class AssetHandler
    {
    public:
        // Texture handling
        static std::shared_ptr<AssetTexture> GetTextureByName(const std::string& name);
        static std::shared_ptr<AssetTexture> LoadTextureFromFile(const std::string& name, const std::string &path);
        static std::shared_ptr<AssetTexture> LoadTextureFromMemory(const std::string& name, const void *data, size_t size);
        static void ClearTextures();

        // Sound handling
        static std::shared_ptr<sf::SoundBuffer> GetSoundByName(const std::string& name);
        static std::shared_ptr<sf::SoundBuffer> LoadSoundFromFile(const std::string& name, const std::string &path);
        static void ClearSounds();

        // Font handling
        static std::shared_ptr<sf::Font> GetFontByName(const std::string& name);
        static std::shared_ptr<sf::Font> LoadFontFromFile(const std::string& name, const std::string &path);
        static void ClearFonts();
    };
}
#endif