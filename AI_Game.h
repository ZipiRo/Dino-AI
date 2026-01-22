#include <algorithm>
#include <fstream>

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

float Distance(const Vector2f &a, const Vector2f &b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}

Text distance_text{font_JetBrains};
Text generation_info_text{font_JetBrains};
Text agent_id_text{font_JetBrains};
Text game_speed_text{font_JetBrains};

RectangleShape ground_rectangle;
float ground_thickness = 75.0f;
Box ground_collider;

#include "Dino.h"
#include "Cactus.h"
#include "NeuralNetwork.h"

bool jump_pressed = false;

const float MAX_GAME_SPEED = 10.0f;
const float BASE_GAME_SPEED = 1.5f;
const float INCREMENT_GAME_SPEED = 0.2f;
const int GAME_SPEED_INCREASE_AT = 50;

float game_speed = BASE_GAME_SPEED;

float distance = 0;

std::vector<Cactus> vegetation;
int cactus_spawn_chance = 10;
float spawn_cactus_delay = 1.0f;
float spawn_cactus_timer;

const float MIN_NEXT_CACTUS_DISTANCE = 3.0f;
const float MAX_NEXT_CACTUS_DISTANCE = 10.0f;

const float MAX_CACTUS_DISTANCE = 500.0f;
const float MAX_CACTUS_HEIGHT = 120.0f;
const float MIN_CACTUS_HEIGHT = 70.0f;

float distance_from_last_spawn = 0.0f;
float next_spawn_distance;

int last_speed_increase;

void VegetationUpdate()
{
    distance_from_last_spawn += game_speed * deltaTime;

    if(distance_from_last_spawn >= next_spawn_distance)
    {
        distance_from_last_spawn = 0.0f;

        float speed_factor = game_speed / BASE_GAME_SPEED;

        float scaled_min_distance = MIN_NEXT_CACTUS_DISTANCE * speed_factor;
        float scaled_max_distance = MAX_NEXT_CACTUS_DISTANCE * speed_factor;

        next_spawn_distance = RandomFloatRange(scaled_min_distance, scaled_max_distance);
        float random_cactus_height = RandomIntRange(MIN_CACTUS_HEIGHT, MAX_CACTUS_HEIGHT);

        Cactus cactus;
        cactus.Init(window_width + 100.0f, 30.0f, random_cactus_height);

        vegetation.push_back(cactus);
    }

    for(int i = 0; i < vegetation.size(); i++)
    {
        Cactus &cactus = vegetation[i];
        
        cactus.position.x -= 150.0f * game_speed * deltaTime;
        cactus.Update();

        if(cactus.collider.right < 0.0f)
            vegetation.erase(vegetation.begin() + i);
    }
}

RectangleShape star_rectangle;
std::vector<Vector2f> star_positions;
const int MAX_STAR_COUNT = 100;

void AddStar()
{
    for(int i = 0; i < 5; i++)
    {
        Vector2f random_position(RandomIntRange(window_width, window_width * 2 + 100.0f), RandomIntRange(0, window_height - ground_thickness));
        star_positions.push_back(random_position);
    }
}

void StarsUpdate()
{
    if(star_positions.size() < MAX_STAR_COUNT - 5)
        AddStar();

    for(int i = 0; i < star_positions.size(); i++)
    {
        star_positions[i].x -= 110.0f * game_speed * deltaTime;

        if(star_positions[i].x < 0)
            star_positions.erase(star_positions.begin() + i);
    }
}

std::vector<Agent> population;
std::vector<Agent> elite;

const int TOP_BRAINS_COUNT = 10;
const int MAX_AGENT_COUNT = 100;

const int JUMP_PENALTY = 1;
const int JUMP_REWARD = 5;

int agent_count = MAX_AGENT_COUNT;
int generation_count = 1;

Agent default_agent;

void SelectElites()
{
    elite.clear();

    std::sort(population.begin(), population.end(), [](auto &a, auto &b)
        { return a.fitness > b.fitness; });
    
    for(int i = 0; i < population.size() * float(TOP_BRAINS_COUNT / 100.0f); i++)
        elite.push_back(population[i]);
}

void CreateBetterPopulation()
{
    population.clear();

    while (population.size() < MAX_AGENT_COUNT)
    {
        const Agent &agent = elite[RandomIntRange(0, TOP_BRAINS_COUNT)];
        NeuralNetwork elite_brain = agent.brain;
        NeuralNetwork new_brain = elite_brain;
        new_brain.Mutate();

        Agent new_agent = default_agent;
        new_agent.brain = new_brain;
        new_agent.dino.color = agent.dino.color;
        new_agent.id = population.size() + 1;

        population.push_back(new_agent);
    }
}

void ResetGame()
{
    vegetation.clear();
    distance = 0.0f;
    spawn_cactus_timer = 0.0f;
    distance_from_last_spawn = 0.0f;
    next_spawn_distance = 0.0f;
    game_speed = 1.0f;
}

void SaveElites()
{
    std::ofstream file(".best_elite_brain", std::ios_base::binary);

    for(const auto& agent : elite)
    {   
        const NeuralNetwork &brain = agent.brain;

        file.write((char*)&brain.input_weight[0][0], 3 * 6 * sizeof(float));
        file.write((char*)brain.input_bias, 6 * sizeof(float));

        file.write((char*)brain.output_weights, 6 * sizeof(float));
        file.write((char*)&brain.output_bias, sizeof(float));
    }

    file.close();
}

void LoadElites()
{
    std::ifstream file(".best_elite_brain", std::ios::binary);

    elite.resize(TOP_BRAINS_COUNT);

    for(auto& agent : elite)
    {
        NeuralNetwork brain = agent.brain;

        file.read((char*)&brain.input_weight[0][0], 3 * 6 * sizeof(float));
        file.read((char*)brain.input_bias, 6 * sizeof(float));

        file.read((char*)brain.output_weights, 6 * sizeof(float));
        file.read((char*)&brain.output_bias, sizeof(float));
    }

    file.close();
}

void Start()
{
    srand(time(NULL));

    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    if(font_JetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
    {
        distance_text.setFont(font_JetBrains);
        generation_info_text.setFont(font_JetBrains);
        agent_id_text.setFont(font_JetBrains);
        game_speed_text.setFont(font_JetBrains);
    }

    distance_text.setFillColor(Color::Black);
    distance_text.setPosition({10.0f, 0.0f});

    generation_info_text.setFillColor(Color::White);
    generation_info_text.setPosition({10.0f, window_height - 50.0f});
    generation_info_text.setString("GENERATION: 0 | ALIVE: " + std::to_string(agent_count));
    generation_info_text.setCharacterSize(20);

    game_speed_text.setFillColor(Color::White);
    game_speed_text.setPosition({10.0f, window_height - 25.0f});
    game_speed_text.setString("GAME SPEED: " + std::to_string(game_speed));
    game_speed_text.setCharacterSize(20);

    agent_id_text.setFillColor(Color::Red);
    agent_id_text.setCharacterSize(13);
    agent_id_text.setStyle(Text::Bold);

    star_rectangle = RectangleShape({1, 1});
    star_rectangle.setOrigin({0.5f, 0.5f});
    star_rectangle.setFillColor(Color::Black);
    star_rectangle.scale({5.0f, 5.0f});

    ground_rectangle = RectangleShape({window_width, ground_thickness});
    ground_rectangle.setFillColor(Color::Black);
    ground_rectangle.setPosition({0.0f, window_height - ground_thickness});

    ground_collider.Create(0.0f, window_height - ground_thickness + 1.0f, window_width, ground_thickness);

    default_agent.dino.color = Color(0, 0, 0, 50);
    default_agent.dino.Init(150.0f, 50.0f, 100.0f);
    default_agent.dino.jump_force = 650.0f;
    default_agent.alive = true;

    for (int i = 0; i < agent_count; i++)
    {
        Agent agent = default_agent;
        agent.brain.Init();
        agent.id = i + 1;
        agent.dino.color = Color(RandomIntRange(0, 200), RandomIntRange(0, 200), RandomIntRange(0, 200), 100);

        population.push_back(agent);
    }
}

void Update()
{
    distance += game_speed * deltaTime;
    distance_text.setString(std::to_string((int)distance));

    int current_speed = int(distance) / GAME_SPEED_INCREASE_AT;

    if(current_speed > last_speed_increase)
    {
        game_speed += INCREMENT_GAME_SPEED;
        last_speed_increase = current_speed;

        game_speed_text.setString("GAME SPEED: " + std::to_string(game_speed));
    }

    for(auto &agent : population)
    {
        if(!agent.alive) continue;

        float distance_to_cactus = MAX_CACTUS_DISTANCE;
        bool noObstacleNear = true;

        float input_one = 1.0f;
        float input_two = 0.0f;
        float input_three = game_speed / MAX_GAME_SPEED;
        float input_four = agent.dino.isOnGround ? 1.0f : 0.0f;

        if(!vegetation.empty())
        {
            Cactus &cactus = vegetation.front();
            distance_to_cactus = cactus.position.x - agent.dino.position.x; 

            if(distance_to_cactus > 0.0f)
            {
                noObstacleNear = distance_to_cactus > 200.0f;
                input_one = distance_to_cactus / MAX_CACTUS_DISTANCE;
                input_two = vegetation.front().height / MAX_CACTUS_HEIGHT;
            }
        }

        float output = agent.brain.Think(input_one, input_two, input_three, input_four);

        if(output > 0.5f)
        {
            agent.dino.Jump();

            if(noObstacleNear) agent.fitness -= JUMP_PENALTY;
                else agent.fitness += JUMP_REWARD;
        }
        
        agent.dino.Update();
    }

    StarsUpdate();
    VegetationUpdate();
    
    for(const auto &cactus : vegetation)
    {
        for(auto &agent : population)
        {
            if(!agent.alive) continue;
            if(Distance(agent.dino.position, cactus.position) > MAX_CACTUS_DISTANCE) continue;
        
            if(IntersectBox(cactus.collider, agent.dino.collider))
            {
                agent.fitness += (int)distance;
                agent.alive = false;
                agent_count--;

                generation_info_text.setString("GENERATION: " + std::to_string(generation_count) + " | ALIVE: " + std::to_string(agent_count));
            }
        }
    }

    if(agent_count <= 0) 
    {
        SelectElites();
        CreateBetterPopulation();
        agent_count = MAX_AGENT_COUNT;
        
        generation_count++;
        generation_info_text.setString("GENERATION: " + std::to_string(generation_count) + " | ALIVE: " + std::to_string(agent_count));
        ResetGame();
        SaveElites();
    }
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    for(const auto &star : star_positions)
    {
        star_rectangle.setPosition(star);
        window.draw(star_rectangle);
    }
    
    for(auto cactus : vegetation)
        cactus.Draw();
    
    for(auto agent : population)
    {
        if(!agent.alive) continue;
        agent.dino.rectangle_shape.setFillColor(agent.dino.color);
        agent.dino.Draw();

        agent_id_text.setString(std::to_string(agent.id));
        agent_id_text.setPosition(agent.dino.position);
        window.draw(agent_id_text);
    }

    window.draw(ground_rectangle);
    window.draw(distance_text);
    window.draw(game_speed_text);
    window.draw(generation_info_text);
}
