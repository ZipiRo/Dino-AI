int RandomIntRange(int min, int max)
{
    return min + rand() % (max - min + 1);
}

float RandomFloatRange(float min, float max)
{   
    return min + (float)rand() / RAND_MAX * (max - min);
}

float Sigmoid(float x)
{
    return 1.0f / (1.0f + std::exp(-x));    
}

const float MUTATE_AMOUNT = 0.3f;
const float MUTATION_RATE = 0.1f;

struct NeuralNetwork
{  
    float input_weight[4][6];
    float input_bias[6];

    float output_weights[6];
    float output_bias;

    void Init()
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 6; j++)
                input_weight[i][j] = RandomFloatRange(-1.0f, 1.0f);

        for(int i = 0; i < 6; i++)
        {
            input_bias[i] = RandomFloatRange(-1.0f, 1.0f);
            output_weights[i] = RandomFloatRange(-1.0f, 1.0f);
        }

        output_bias = RandomFloatRange(-1.0f, 1.0f);
    }
    
    float Think(float distance_to_cactus, float size_of_cactus, float game_speed, float onground)
    {
        float output_calculation = 0;
        for(int i = 0; i < 6; i++)
        {
            float input_calculation = distance_to_cactus * input_weight[0][i] + 
                                size_of_cactus * input_weight[1][i] +
                                game_speed * input_weight[2][i] +
                                onground * input_weight[3][i] + input_bias[i];
            
            output_calculation += output_weights[i] * input_calculation;
        }

        output_calculation += output_bias;

        return output_calculation;
    }

    void Mutate()
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 6; j++)
            {
                if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
                {
                    input_weight[i][j] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                    input_weight[i][j] = std::clamp(input_weight[i][j], -1.0f, 1.0f);
                }
            }

        for(int i = 0; i < 6; i++)
        {
            if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
            {
                input_bias[i] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                input_bias[i] = std::clamp(input_bias[i], -1.0f, 1.0f);
            }
            
            if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
            {
                output_weights[i] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                output_weights[i] = std::clamp(output_weights[i], -1.0f, 1.0f);
            }
        }
        
        if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
        {
            output_bias += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
            output_bias = std::clamp(output_bias, -1.0f, 1.0f);
        }
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