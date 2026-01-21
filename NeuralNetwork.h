int RandomIntRange(int min, int max)
{
    return min + rand() % (max - min + 1);
}

float RandomFloatRange(float min, float max, int floating_points)
{   
    int min_int = min * floating_points;
    int max_int = max * floating_points;

    int min_number = min_int;
    int max_number = max_int;

    bool negative_number = false;

    if(min_int < 0)
    {
        min_number = 0;
        max_number += std::abs(min_int);
        negative_number = true;
    }

    int random_number = min_number + rand() % (max_number - min_number + 1);

    if(negative_number)
        random_number -= std::abs(min_int);
    
    return (float)random_number / (float)floating_points;
}

float Sigmoid(float x)
{
    return 1.0f / (1.0f + std::exp(-x));    
}

struct NeuralNetwork
{  
    float input_weight[3][6];
    float input_bias[6];

    float output_weights[6];
    float output_bias;

    void Init()
    {
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 6; j++)
                input_weight[i][j] = RandomFloatRange(-1.0f, 1.0f, 10);

        for(int i = 0; i < 6; i++)
        {
            input_bias[i] = RandomFloatRange(-1.0f, 1.0f, 10);
            output_weights[i] = RandomFloatRange(-1.0f, 1.0f, 10);
        }

        output_bias = RandomFloatRange(-1.0f, 1.0f, 10);
    }
    
    float Think(float distance_to_cactus, float size_of_cactus, float game_speed)
    {
        float output_calculation = 0;
        for(int i = 0; i < 6; i++)
        {
            float input_calculation = distance_to_cactus * input_weight[0][i] + 
                                size_of_cactus * input_weight[1][i] +
                                game_speed * input_weight[2][i] + input_bias[i];
            
            output_calculation += Sigmoid(input_calculation) * output_weights[i];
        }

        output_calculation += output_bias;

        return Sigmoid(output_calculation);
    }

    void Mutate()
    {
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 6; j++)
            {
                if(RandomFloatRange(0.0f, 1.0f, 100) < 0.05f)
                {
                    input_weight[i][j] += RandomFloatRange(-0.3f, 0.3f, 10);
                    input_weight[i][j] = std::clamp(input_weight[i][j], -1.0f, 1.0f);
                }
            }

        for(int i = 0; i < 6; i++)
        {
            input_bias[i] = RandomFloatRange(-0.3f, 0.3f, 10);
            input_bias[i] = std::clamp(input_bias[i], -1.0f, 1.0f);

            output_weights[i] = RandomFloatRange(-0.3f, 0.3f, 10);
            output_weights[i] = std::clamp(output_weights[i], -1.0f, 1.0f);
        }

        output_bias = RandomFloatRange(-0.3f, 0.3f, 10);
        output_bias = std::clamp(output_bias, -1.0f, 1.0f);
    } 
};

struct Agent
{
    int id;
    NeuralNetwork brain;
    Dino dino;
    float fitness;
    bool alive;
};