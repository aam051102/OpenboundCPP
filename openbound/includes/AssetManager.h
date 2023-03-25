#ifndef SBURB_ASSET_HANDLER_H
#define SBURB_ASSET_HANDLER_H

#include "Logger.h"
#include <memory>
#include "Asset.h"
#include "AssetGraphic.h"
#include "AssetFont.h"
#include "AssetAudio.h"
#include "AssetPath.h"
#include "AssetText.h"
#include "AssetMovie.h"

namespace SBURB
{
    class AssetManager
    {
    public:
        static void AddToTotalAssets(int val);
        static int GetTotalAssets();
        static int GetTotalLoaded();

        static void LoadAsset(std::shared_ptr<Asset> asset);
        
        // File paths
        static std::wstring GetFilePathByName(const std::wstring& name);
        static void AddFilePath(const std::wstring& name, const std::wstring path);
        static void ClearFilePaths();

        // Path
        static std::shared_ptr<AssetPath> GetPathByName(const std::wstring &name);
        static void ClearPaths();

        // Graphic
        static std::shared_ptr<AssetGraphic> GetGraphicByName(const std::wstring &name);
        static void ClearGraphics();

        // Audio
        static std::shared_ptr<AssetAudio> GetAudioByName(const std::wstring &name);
        static void ClearAudio();

        // Font
        static std::shared_ptr<AssetFont> GetFontByName(const std::wstring &name);
        static void ClearFonts();

        // Movie
        static std::shared_ptr<AssetMovie> GetMovieByName(const std::wstring &name);
        static void ClearMovies();

        // Text
        static std::shared_ptr<AssetText> GetTextByName(const std::wstring &name);
        static void ClearText();
    };
}
#endif