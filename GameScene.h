#include <algorithm>
#include <fstream>

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

int RandomIntRange(int min, int max)
{
    return min + rand() % (max - min + 1);
}

float RandomFloatRange(float min, float max)
{   
    return min + (float)rand() / RAND_MAX * (max - min);
}

Color RandomDominantColor()
{
    int dominant_color = RandomIntRange(1, 3);

    int r = RandomIntRange(50, 255);
    r *= (dominant_color == 1) ? 1 : 0;
    int g = RandomIntRange(50, 255);
    g *= (dominant_color == 2) ? 1 : 0;
    int b = RandomIntRange(50, 255);
    b *= (dominant_color == 3) ? 1 : 0;

    return Color(r, g, b);
}

const float GRAVITY = 900.0f;

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

const float MAX_GAME_SPEED = 30.0f;
const float BASE_GAME_SPEED = 2.0f;
const float INCREMENT_GAME_SPEED_BY = 0.25f;
const int GAME_SPEED_INCREASE_AT = 50;

float game_speed = BASE_GAME_SPEED;
int last_speed_increase;

const int MAX_AGENT_COUNT = 200;

int agent_count = MAX_AGENT_COUNT;
int generation_count = 1;

float distance = 0;
int best_distance = 0;

std::vector<Cactus> vegetation;

/// THIS ARE TIME BASED
const float MIN_NEXT_CACTUS_DISTANCE = 3.0f;
const float MAX_NEXT_CACTUS_DISTANCE = 7.0f; 

float distance_from_last_spawn = 0.0f;
float next_spawn_distance;
/// THIS ARE TIME BASED

const float MAX_CACTUS_DISTANCE = 500.0f;

const float MAX_CACTUS_HEIGHT = 150.0f;
const float MIN_CACTUS_HEIGHT = 70.0f;

const float MAX_CACTUS_WIDTH = 70.0f;
const float MIN_CACTUS_WIDTH = 30.0f;

void VegetationUpdate()
{
    distance_from_last_spawn += game_speed * Timer::deltaTime;

    if(distance_from_last_spawn >= next_spawn_distance)
    {
        distance_from_last_spawn = 0.0f;

        float speed_factor = game_speed / BASE_GAME_SPEED;

        float scaled_min_distance = MIN_NEXT_CACTUS_DISTANCE * speed_factor;
        float scaled_max_distance = MAX_NEXT_CACTUS_DISTANCE * speed_factor;

        next_spawn_distance = RandomFloatRange(scaled_min_distance, scaled_max_distance);
        float random_cactus_height = RandomIntRange(MIN_CACTUS_HEIGHT, MAX_CACTUS_HEIGHT);
        float random_cactus_width = RandomIntRange(MIN_CACTUS_WIDTH, MAX_CACTUS_WIDTH);

        Cactus cactus;
        cactus.Init(window_width + 100.0f, random_cactus_width, random_cactus_height);
        cactus.reward_hit.resize(MAX_AGENT_COUNT, false);

        vegetation.push_back(cactus);
    }

    for(int i = 0; i < vegetation.size(); i++)
    {
        Cactus &cactus = vegetation[i];
        
        cactus.position.x -= 150.0f * game_speed * Timer::deltaTime;
        cactus.Update();

        if(cactus.collider.right < 0.0f)
            vegetation.erase(vegetation.begin() + i);
    }
}

RectangleShape star_rectangle;
std::vector<Vector2f> star_positions;
const int MAX_STAR_COUNT = 100;

void StarsUpdate()
{
    if(star_positions.size() < MAX_STAR_COUNT)
    {
        Vector2f random_position(RandomIntRange(window_width, window_width * 2 + 100.0f), RandomIntRange(0, window_height - ground_thickness));
        star_positions.push_back(random_position);
    }
        
    for(int i = 0; i < star_positions.size(); i++)
    {
        star_positions[i].x -= 100.0f * game_speed * Timer::deltaTime;

        if(star_positions[i].x < 0)
            star_positions.erase(star_positions.begin() + i);
    }
}

struct Agent
{
    NeuralNetwork brain;
    Dino dino;

    int id = 0;
    int fitness = 0;
    bool alive = true;
    bool used_jump = false;
};

Agent default_agent;

std::vector<Agent> population;
std::vector<Agent> elite;

const int TOP_BRAINS_COUNT = 10;

const int JUMP_PENALTY = 5;
const int JUMP_CACTUS_REWARD = 5;

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
        const Agent &parent = elite[RandomIntRange(0, TOP_BRAINS_COUNT)];
        NeuralNetwork new_brain = parent.brain;
        new_brain.Mutate();

        Agent child = default_agent;
        child.brain = new_brain;
        child.id = population.size() + 1;
        child.dino.color = parent.dino.color;

        population.push_back(child);
    }
}

void ResetGame()
{
    vegetation.clear();
    distance = 0.0f;
    distance_from_last_spawn = 0.0f;
    next_spawn_distance = 0.0f;
    game_speed = BASE_GAME_SPEED;
    last_speed_increase = 0;
    game_speed_text.setString("GAME SPEED: " + std::to_string(game_speed));
}

void SaveElites()
{
    std::ofstream file(".neural_save", std::ios_base::binary);

    for(const auto& agent : elite)
    {   
        const NeuralNetwork &brain = agent.brain;

        file.write((char*)&brain.input_weight[0][0], INPUT_COUNT * NEURON_COUNT * sizeof(float));
        file.write((char*)brain.input_bias, NEURON_COUNT * sizeof(float));

        file.write((char*)&brain.output_weight[0][0], NEURON_COUNT * OUTPUT_COUNT * sizeof(float));
        file.write((char*)brain.output_bias, OUTPUT_COUNT * sizeof(float));
    }

    file.close();
}

void LoadElites(const std::string &save_file)
{
    std::ifstream file(save_file, std::ios::binary);

    elite.resize(TOP_BRAINS_COUNT);

    for(auto &agent : elite)
    {   
        agent = default_agent;

        file.read((char*)&agent.brain.input_weight[0][0], INPUT_COUNT * NEURON_COUNT * sizeof(float));
        file.read((char*)agent.brain.input_bias, NEURON_COUNT * sizeof(float));

        file.read((char*)&agent.brain.output_weight[0][0], NEURON_COUNT * OUTPUT_COUNT * sizeof(float));
        file.read((char*)agent.brain.output_bias, OUTPUT_COUNT * sizeof(float));
    }

    file.close();
}

void UpdatePopulation()
{
    /// UPDATE AGENTS AND THINK PHASE
    for(auto &agent : population)
    {
        if(!agent.alive) continue;

        float distance_to_cactus = MAX_CACTUS_DISTANCE;

        std::vector<float> inputs(INPUT_COUNT);
        Cactus cactus;

        for(auto &cactus_it : vegetation)
        {
            distance_to_cactus = cactus_it.position.x - agent.dino.position.x; 

            if(distance_to_cactus >= 0.0f)
            {
                cactus = cactus_it;
                break;
            }
        }

        inputs[0] = distance_to_cactus / MAX_CACTUS_DISTANCE;
        inputs[1] = cactus.height / MAX_CACTUS_HEIGHT;
        inputs[2] = cactus.width / MAX_CACTUS_WIDTH;
        inputs[3] = agent.dino.isOnGround ? 1.0f : 0.0f;
        inputs[4] = game_speed / MAX_GAME_SPEED; 

        float jump_output = agent.brain.Think(inputs)[0]; /// JUMP OUTPUT

        bool shouldJump = jump_output > 0.5f;

        if(shouldJump)
        {
            if(!agent.dino.isJumping)
            {
                agent.dino.Jump();
                agent.used_jump = true;
            }
        }
        else 
        {
            if(agent.dino.jumpHeld)
            {
                agent.dino.ReleaseJump();
            }
        }
        
        agent.dino.Update();
    }

    /// REWARDS AND COLLISION PHASE
    for(auto &agent : population)
    {
        if(!agent.alive) continue;

        Cactus cactus;

        for(auto &cactus_it : vegetation)
        {
            float distance_to_cactus = cactus_it.collider.right - agent.dino.position.x; 

            if(distance_to_cactus >= 0.0f)
            {
                cactus = cactus_it;
                break;
            }
        }

        if(cactus.position.x - agent.dino.position.x > MAX_CACTUS_DISTANCE) continue;
        if(cactus.collider.right < agent.dino.position.x - 10.0f) continue;
    
        if(IntersectBox(cactus.collider, agent.dino.collider))
        {
            agent.fitness += (int)distance;
            agent.alive = false;
            agent_count--;

            generation_info_text.setString("GENERATION: " + std::to_string(generation_count) + " | ALIVE: " + std::to_string(agent_count));
        }

        if(agent.dino.collider.left > cactus.collider.right && !cactus.reward_hit[agent.id] && agent.used_jump)
        {
            agent.fitness += JUMP_CACTUS_REWARD;
            cactus.reward_hit[agent.id] = true;
            agent.used_jump = false;
        }

        if(agent.dino.isOnGround && agent.used_jump)
        {
            agent.fitness -= JUMP_PENALTY;
            agent.used_jump = false;
        }
    }
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
    distance_text.setCharacterSize(18);
    distance_text.setPosition({10.0f, 0.0f});

    generation_info_text.setFillColor(Color::White);
    generation_info_text.setPosition({10.0f, window_height - 70.0f});
    generation_info_text.setString("GENERATION: 0 | ALIVE: " + std::to_string(agent_count));
    generation_info_text.setCharacterSize(20);

    game_speed_text.setFillColor(Color::White);
    game_speed_text.setPosition({10.0f, window_height - 45.0f});
    game_speed_text.setString("GAME SPEED: " + std::to_string(game_speed));
    game_speed_text.setCharacterSize(20);

    agent_id_text.setFillColor(Color::Red);
    agent_id_text.setCharacterSize(13);
    agent_id_text.setStyle(Text::Bold);

    star_rectangle = RectangleShape({1, 1});
    star_rectangle.setOrigin({0.5f, 0.5f});
    star_rectangle.setFillColor(Color::Black);
    star_rectangle.scale({2.0f, 2.0f});

    ground_rectangle = RectangleShape({window_width, ground_thickness});
    ground_rectangle.setFillColor(Color::Black);
    ground_rectangle.setPosition({0.0f, window_height - ground_thickness});

    ground_collider.Create(0.0f, window_height - ground_thickness + 1.0f, window_width, ground_thickness);

    default_agent.dino.Init(150.0f, 50.0f, 100.0f);

    for (int i = 0; i < agent_count; i++)
    {
        Agent agent = default_agent;
        agent.brain.Init();
        agent.id = i + 1;
        agent.dino.color = RandomDominantColor();
        agent.dino.color.a = 50;

        population.push_back(agent);
    }
}

void Update()
{
    distance += game_speed * Timer::deltaTime;
    if(best_distance < int(distance))
        best_distance = int(distance);

    distance_text.setString("SCORE: " + std::to_string((int)distance) + " HIGHSCORE: " + std::to_string(best_distance));

    int current_speed = int(distance) / GAME_SPEED_INCREASE_AT;

    if(current_speed > last_speed_increase && game_speed < MAX_GAME_SPEED)
    {
        game_speed += INCREMENT_GAME_SPEED_BY;
        last_speed_increase = current_speed;

        game_speed_text.setString("GAME SPEED: " + std::to_string(game_speed));
    }

    StarsUpdate();
    
    VegetationUpdate();

    UpdatePopulation();

    /// RESET THE SIMULATION
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
    
    for(auto &cactus : vegetation)
        cactus.Draw();
    
    for(auto &agent : population)
    {
        if(!agent.alive) continue;
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