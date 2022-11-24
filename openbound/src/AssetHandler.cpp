#include "AssetHandler.h"
#include <vector>
#include <unordered_map>

static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
static std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> sounds;
static std::unordered_map<std::string, std::shared_ptr<sf::Font>> fonts;
static std::map<std::string, std::string_view> paths;

namespace SBURB
{
    // Texture handling
    std::shared_ptr<sf::Texture> AssetHandler::GetTextureByName(const std::string& name)
    {
        return textures[name];
    }

    std::shared_ptr<sf::Texture> AssetHandler::LoadTextureFromFile(const std::string& name, const std::string &path)
    {
        std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(path))
        {
            texture.reset();
            return nullptr;
        }
        // textures.push_back(std::move(texture));
        textures.insert(std::pair(name, texture));
        return texture;
    }

    std::shared_ptr<sf::Texture> AssetHandler::LoadTextureFromMemory(const std::string& name, const void *data, size_t size)
    {
        std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(data, size))
        {
            texture.reset();
            return nullptr;
        }
        textures.insert(std::pair(name, texture));
        return texture;
    }

    void AssetHandler::ClearTextures()
    {
        for (auto texture : textures)
        {
            texture.second.reset();
        }

        textures.clear();
    }

    // Sound handling
    std::shared_ptr<sf::SoundBuffer> AssetHandler::GetSoundByName(const std::string& name)
    {
        return sounds[name];
    }

    std::shared_ptr<sf::SoundBuffer> AssetHandler::LoadSoundFromFile(const std::string& name, const std::string &path)
    {
        if (sounds[name]) return sounds[name];

        std::shared_ptr<sf::SoundBuffer> buffer = std::make_shared<sf::SoundBuffer>();
        if (!buffer->loadFromFile(path))
        {
            GlobalLogger->Log(Logger::Error, "Failed to load sound from path: '" + path + "'");
            buffer.reset();
            return nullptr;
        }

        sounds.insert(std::pair(name, buffer));

        return buffer;
    }

    void AssetHandler::ClearSounds()
    {
        for (auto sound : sounds)
        {
            sound.second.reset();
        }

        sounds.clear();
    }

    // Font handling
    std::shared_ptr<sf::Font> AssetHandler::GetFontByName(const std::string& name)
    {
        return fonts[name];
    }

    std::shared_ptr<sf::Font> AssetHandler::LoadFontFromFile(const std::string& name, const std::string &path)
    {
        std::shared_ptr<sf::Font> font;
        if (!font->loadFromFile(path)) {
            GlobalLogger->Log(Logger::Error, "Failed to load font from path: '" + path + "'");
            return nullptr;
        }

        fonts.insert(std::pair(name, font));

        return font;
    }

    void AssetHandler::ClearFonts()
    {
        for (auto font : fonts)
        {
            font.second.reset();
        }

        fonts.clear();
    }
}