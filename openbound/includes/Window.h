#ifndef SBURB_WINDOW_H
#define SBURB_WINDOW_H

#include "Common.h"

#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

namespace SBURB
{
    enum class WindowFlags
    {
        Focused = 0x00020001 /* GLFW_FOCUSED's value */,
        Resizable = 0x00020003,
        Visible = 0x00020004,
        Decorated = 0x00020005,
        AutoIconify = 0x00020006,
        Floating = 0x00020007,
        Maximized = 0x00020008,
        CenterCursor = 0x00020009,
        TransparentFramebuffer = 0x0002000A,
        FocusOnShow = 0x0002000C
    };

    // Handles a game window
    class Window
    {
    public:
        Window();
        ~Window();

        void Init(std::string title = "Openbound", sf::Vector2i size = {650, 450}, int style = sf::Style::Default, sf::Image icon = sf::Image());
        void CenterWindow();

        sf::RenderWindow *operator->() { return win; }

        sf::RenderWindow *GetWin();

        std::string GetTitle();
        void SetTitle(std::string title);

        sf::Vector2i GetSize();
        void SetSize(sf::Vector2i size);

    private:
        sf::RenderWindow *win;
        std::string title;
        sf::Vector2i size;
    };
}

#endif // SBURB_WINDOW_H