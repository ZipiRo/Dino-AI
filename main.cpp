#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "Template";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color::White;

View camera;
View canvas;

Vector2f gravity_acc{0, 981.0f};

struct Box
{
    float left;
    float top;
    float right;
    float bottom;

    void Create(float x, float y, float width, float height)
    {
        left = x;
        top = y;
        right = x + width;
        bottom = y + height;
    }
};

bool IntersectBox(const Box& boxA, const Box& boxB)
{
    return boxA.right > boxB.left &&  boxA.left < boxB.right &&
            boxA.bottom > boxB.top && boxA.top < boxB.bottom;
}

RectangleShape ground_rectangle;

float ground_thickness = 75.0f;
Box ground_collider;

#include "Dino.h"
#include "Cactus.h"

Dino dino_player;

bool jump_pressed = false;

std::vector<Cactus> vegetation;

void AddCactus(int chance_percent)
{
    bool should_spwan = (std::rand() % 1000) < chance_percent;

    if(should_spwan)
    {
        Cactus cactus;
        cactus.Init(window_width + 100.0f, 30.0f, 80.0f);

        vegetation.push_back(cactus);
    }
}

int cactus_spawn_chance = 5;
float game_speed = 200.0f;
int distance = 0;

void Start()
{
    srand(time(NULL));

    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    ground_rectangle = RectangleShape({window_width, ground_thickness});
    ground_rectangle.setFillColor(Color::Black);
    ground_rectangle.setPosition({0.0f, window_height - ground_thickness});

    ground_collider.Create(0.0f, window_height - ground_thickness + 1.0f, window_width, ground_thickness);

    // 

    dino_player.Init(150.0f, 50.0f, 100.0f);
    dino_player.jumpForce = 600.0f;
}

void Update()
{
    distance += 10;
    std::cout << distance / 100 << "\n";
    dino_player.Update();

    if(Keyboard::isKeyPressed(Keyboard::Key::Space) && !jump_pressed)
    {        
        dino_player.Jump();
        jump_pressed = true;
    }

    if(!Keyboard::isKeyPressed(Keyboard::Key::Space))
        jump_pressed = false;
    
    AddCactus(cactus_spawn_chance);

    for(auto &cactus : vegetation)
    {
        cactus.position.x -= game_speed * deltaTime;
        cactus.Update();
        
        if(IntersectBox(cactus.collider, dino_player.collider))
        {
            dino_player.color = Color::Red;
            dino_player.rectangle_shape.setFillColor(dino_player.color);
        }
    }
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    for(auto cactus : vegetation)
        cactus.Draw();
        
    dino_player.Draw();

    window.draw(ground_rectangle);
}

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