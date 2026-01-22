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

Text distance_text{font_JetBrains};

float ground_thickness = 75.0f;
Box ground_collider;

#include "Dino.h"
#include "Cactus.h"

Dino dino_player;

bool jump_pressed = false;

std::vector<Cactus> vegetation;

void AddCactus(int chance_percent)
{
    bool should_spwan = (std::rand() % 100) < chance_percent;

    if(should_spwan)
    {
        Cactus cactus;
        cactus.Init(window_width + 100.0f, 30.0f, 80.0f);

        vegetation.push_back(cactus);
    }
}

int cactus_spawn_chance = 10;

float game_speed = 1.25f;
float increment_game_speed = 0.05f;

float spawn_cactus_delay = 1.0f;
float spawn_cactus_timer;

void VegetationUpdate()
{
    spawn_cactus_timer += deltaTime;

    if(spawn_cactus_timer >= spawn_cactus_delay)
    {
        AddCactus(cactus_spawn_chance);
        spawn_cactus_timer = 0;
    }

    for(int i = 0; i < vegetation.size(); i++)
    {
        Cactus &cactus = vegetation[i];
        
        cactus.position.x -= 100.0f * game_speed * deltaTime;
        cactus.Update();
        
        if(IntersectBox(cactus.collider, dino_player.collider))
        {
            dino_player.color = Color::Red;
            dino_player.rectangle_shape.setFillColor(dino_player.color);
            game_speed = 0;
        }

        if(cactus.collider.right < 0.0f)
            vegetation.erase(vegetation.begin() + i);
    }
}

float distance;
int distance = 0;

void Start()
{
    srand(time(NULL));

    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    if(font_JetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
    {
        distance_text.setFont(font_JetBrains);
    }

    distance_text.setFillColor(Color::Black);

    ground_rectangle = RectangleShape({window_width, ground_thickness});
    ground_rectangle.setFillColor(Color::Black);
    ground_rectangle.setPosition({0.0f, window_height - ground_thickness});

    ground_collider.Create(0.0f, window_height - ground_thickness + 1.0f, window_width, ground_thickness);

    dino_player.Init(150.0f, 50.0f, 100.0f);
    dino_player.jump_force = 600.0f;
}

void Update()
{
    distance += deltaTime;
    if(distance >= 1.0f * (1.0f / game_speed)){
        distance++;

        if((distance + 1) % 100 == 0)
        {
            game_speed += increment_game_speed;
            std::cout << game_speed << '\n';
        }

        distance = 0;
    }

    distance_text.setString(std::to_string(distance));

    dino_player.Update();

    if(Keyboard::isKeyPressed(Keyboard::Key::Space) && !jump_pressed)
    {        
        dino_player.Jump();
        jump_pressed = true;
    }

    if(!Keyboard::isKeyPressed(Keyboard::Key::Space))
        jump_pressed = false;

    VegetationUpdate();
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    for(auto cactus : vegetation)
        cactus.Draw();
        
    dino_player.Draw();

    window.draw(ground_rectangle);
    window.draw(distance_text);
}
