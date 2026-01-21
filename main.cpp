#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "DinoAI - Neural Network";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color::White;

View camera;
View canvas;

Font font_JetBrains{};

#include "AI_Game.h"

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
 
    Start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
 
        Update();

        window.clear(BackgroundColor);

        Draw();

        window.display();

        sleep(seconds(deltaTime));
    }
}