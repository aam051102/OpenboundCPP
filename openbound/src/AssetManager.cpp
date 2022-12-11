#include "AssetManager.h"
#include <vector>
#include <unordered_map>

#include <mutex>

namespace SBURB
{
    static int totalLoaded = 0;
    static int totalAssets = 0;

    static std::unordered_map<std::string, std::shared_ptr<AssetGraphic>> graphics;
    static std::unordered_map<std::string, std::shared_ptr<AssetAudio>> audio;
    static std::unordered_map<std::string, std::shared_ptr<AssetFont>> fonts;
    static std::unordered_map<std::string, std::shared_ptr<AssetPath>> paths;
    static std::unordered_map<std::string, std::shared_ptr<AssetMovie>> movies;
    static std::unordered_map<std::string, std::shared_ptr<AssetText>> text;

    static std::mutex graphicsMutex;
    static std::mutex fontMutex;

    void AssetManager::AddToTotalAssets(int val) {
        totalAssets += val;
    }

    int AssetManager::GetTotalLoaded() {
        return totalLoaded;
    }

    int AssetManager::GetTotalAssets() {
        return totalAssets;
    }

    void AssetManager::LoadAsset(std::shared_ptr<Asset> asset)
    {
        totalLoaded++;

        if (asset->GetType() == "graphic")
        {
            graphicsMutex.lock();
            graphics.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetGraphic>(asset)));
            graphicsMutex.unlock();
        }
        else if (asset->GetType() == "path")
        {
            paths.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetPath>(asset)));
        }
        else if(asset->GetType() == "font")
        {
            fontMutex.lock();
            fonts.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetFont>(asset)));
            fontMutex.unlock();
        }
        else if (asset->GetType() == "audio")
        {
            audio.insert(std::pair(asset->GetName(), std::static_pointer_cast<AssetAudio>(asset)));
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
        graphicsMutex.lock();

        // Ensure that item exists to prevent preload issues.
        if (graphics.find(name) == graphics.end()) {
            graphicsMutex.unlock();
            return nullptr;
        }

        auto result = graphics[name];
        graphicsMutex.unlock();

        return result;
    }

    void AssetManager::ClearGraphics()
    {
        graphicsMutex.lock();

        for (auto graphic : graphics)
        {
            graphic.second.reset();
        }

        graphics.clear();

        graphicsMutex.unlock();
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
        fontMutex.lock();

        if (fonts.find(name) == fonts.end()) {
            fontMutex.unlock();
            return nullptr;
        }

        auto result = fonts[name];
        fontMutex.unlock();
        return result;
    }

    void AssetManager::ClearFonts()
    {
        fontMutex.lock();

        for (auto font : fonts)
        {
            font.second.reset();
        }

        fonts.clear();

        fontMutex.unlock();
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