#include "AssetManager.h"
#include <vector>
#include <unordered_map>

namespace SBURB
{
    static std::unordered_map<std::string, std::shared_ptr<AssetGraphic>> graphics;
    static std::unordered_map<std::string, std::shared_ptr<AssetAudio>> audio;
    static std::unordered_map<std::string, std::shared_ptr<AssetFont>> fonts;
    static std::unordered_map<std::string, std::shared_ptr<AssetPath>> paths;
    static std::unordered_map<std::string, std::shared_ptr<AssetMovie>> movies;
    static std::unordered_map<std::string, std::shared_ptr<AssetText>> text;

    void AssetManager::LoadAsset(std::shared_ptr<Asset> asset)
    {
        if (asset->GetType() == "graphic")
        {
            graphics.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetGraphic>(asset)));
        }
        else if (asset->GetType() == "path")
        {
            paths.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetPath>(asset)));
        }
        else if(asset->GetType() == "font")
        {
            fonts.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetFont>(asset)));
        }
    }

    // Path
    std::shared_ptr<AssetPath> AssetManager::GetPathByName(const std::string &name)
    {
        return paths[name];
    }

    void AssetManager::ClearPaths()
    {
        for (auto path : paths)
        {
            path.second.reset();
        }

        paths.clear();
    }

    // Graphic
    std::shared_ptr<AssetGraphic> AssetManager::GetGraphicByName(const std::string &name)
    {
        return graphics[name];
    }

    void AssetManager::ClearGraphics()
    {
        for (auto graphic : graphics)
        {
            graphic.second.reset();
        }

        graphics.clear();
    }

    // Audio
    std::shared_ptr<AssetAudio> AssetManager::GetAudioByName(const std::string &name)
    {
        return audio[name];
    }

    void AssetManager::ClearAudio()
    {
        for (auto asset : audio)
        {
            asset.second.reset();
        }

        audio.clear();
    }

    // Font
    std::shared_ptr<AssetFont> AssetManager::GetFontByName(const std::string &name)
    {
        return fonts[name];
    }

    void AssetManager::ClearFonts()
    {
        for (auto font : fonts)
        {
            font.second.reset();
        }

        fonts.clear();
    }

    // Movie
    std::shared_ptr<AssetMovie> AssetManager::GetMovieByName(const std::string &name)
    {
        return movies[name];
    }

    void AssetManager::ClearMovies()
    {
        for (auto movie : movies)
        {
            movie.second.reset();
        }

        movies.clear();
    }

    // Text
    std::shared_ptr<AssetText> AssetManager::GetTextByName(const std::string &name)
    {
        return text[name];
    }

    void AssetManager::ClearText()
    {
        for (auto asset : text)
        {
            asset.second.reset();
        }

        text.clear();
    }
}