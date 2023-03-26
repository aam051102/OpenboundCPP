#include "AssetFont.h"
#include "Sburb.h"
#include "Logger.h"
#include <Windows.h>
#include <winreg.h>

namespace SBURB {
    /*
     Source: https://stackoverflow.com/questions/11387564/get-a-font-filepath-from-name-and-style-in-c-windows
     */
    inline std::string GetSystemFontFile(const std::string& faceName) {

        static const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
        HKEY hKey;
        LONG result;
        std::wstring wsFaceName(faceName.begin(), faceName.end());

        // Open Windows font registry key
        result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD maxValueNameSize, maxValueDataSize;
        result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD valueIndex = 0;
        LPWSTR valueName = new WCHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD valueNameSize, valueDataSize, valueType;
        std::wstring wsFontFile;

        // Look for a matching font name
        do {

            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValue(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::wstring wsValueName(valueName, valueNameSize);

            // Found a match
            if (_wcsnicmp(wsFaceName.c_str(), wsValueName.c_str(), wsFaceName.length()) == 0) {

                wsFontFile.assign((LPWSTR)valueData, valueDataSize);
                break;
            }
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;

        RegCloseKey(hKey);

        if (wsFontFile.empty()) {
            return "";
        }

        // Build full font file path
        WCHAR winDir[MAX_PATH];
        GetWindowsDirectory(winDir, MAX_PATH);

        std::wstringstream ss;
        ss << winDir << "\\Fonts\\" << wsFontFile;
        wsFontFile = ss.str();

        return std::string(wsFontFile.begin(), wsFontFile.end());
    }

    AssetFont::AssetFont(std::wstring name, std::vector<std::wstring> sources) {
        this->type = L"font";
        this->name = name;
        this->sources = sources;
        this->asset = std::make_shared<sf::Font>();
        this->style = sf::Text::Style::Regular;

        for (int i = 0; i < sources.size(); i++) {
            auto values = split(sources[i], L":", 1);
            auto type = trim(values[0]);
            auto path = trim(values[1]);

            if (type == L"url") {
                auto extension = path.substr(path.find_last_of(L".") + 1, path.size() - (path.find_last_of(L".") + 1));
                auto format = "";

                if (extension == L"ttf" || extension == L"otf" || extension == L"ttc") {
                    format = "truetype";
                }
                else if (extension == L"woff") {
                    format = "woff";
                }
                else if (extension == L"svg") {
                    format = "svg";
                }

                if (format == "truetype" || format == "woff") {
                    // NOTE: Unsure if WOFF is supported
                    const auto resolvedPath = Sburb::ResolvePath(path);
                    if (!this->asset->loadFromFile(std::string(resolvedPath.begin(), resolvedPath.end()))) {
                        GlobalLogger->Log(Logger::Error, "Font does not exist.");
                        continue;
                    }
                }
            }
            else if (type == L"local") {
                const auto resolvedPath = GetSystemFontFile(std::string(path.begin(), path.end()));
                if (!this->asset->loadFromFile(std::string(resolvedPath.begin(), resolvedPath.end()))) {
                    GlobalLogger->Log(Logger::Error, "Font does not exist.");
                    continue;
                }
            }
            else if (type == L"weight") {
                if (path == L"bold") {
                    this->style = sf::Text::Style::Bold;
                }
            }
        }
    }
}