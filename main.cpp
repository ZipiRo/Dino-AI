#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "Timer.h"
using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "DinoAI - Neural Network";

const int FPS = 60;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color::White;

View camera;
View canvas;

Font font_JetBrains{};

#include "GameScene.h"

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
 
    Timer::GetInstance();
    Timer::TimeScale = 5;
    Start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
        
        Timer::Tick();

        if(Timer::deltaTime >= 1.0f / FPS)
        {
            Timer::Reset();

            Update();

            window.clear(BackgroundColor);
            Draw();
            window.display();
        }
    }
}