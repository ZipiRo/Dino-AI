float Sigmoid(float x)
{
    return 1.0f / (1.0f + std::exp(-x));    
}

const float MUTATE_AMOUNT = 0.3f;
const float MUTATION_RATE = 0.1f;

const int INPUT_COUNT = 5;
const int NEURON_COUNT = 8;

struct NeuralNetwork
{  
    float input_weight[INPUT_COUNT][NEURON_COUNT];
    float input_bias[NEURON_COUNT];

    float output_weights[NEURON_COUNT];
    float output_bias;

    void Init()
    {
        for(int i = 0; i < INPUT_COUNT; i++)
            for(int j = 0; j < NEURON_COUNT; j++)
                input_weight[i][j] = RandomFloatRange(-1.0f, 1.0f);

        for(int i = 0; i < NEURON_COUNT; i++)
        {
            input_bias[i] = RandomFloatRange(-1.0f, 1.0f);
            output_weights[i] = RandomFloatRange(-1.0f, 1.0f);
        }

        output_bias = RandomFloatRange(-1.0f, 1.0f);
    }
    
    float Think(const std::vector<float> &inputs)
    {
        float output_calculation = 0.0f;
        for(int i = 0; i < NEURON_COUNT; i++)
        {
            float input_calculation = 0.0f;

            for(int j = 0; j < INPUT_COUNT; j++)
                input_calculation += inputs[j] * input_weight[j][i];

            input_calculation += input_bias[i]; 
            
            output_calculation += output_weights[i] * input_calculation;
        }

        output_calculation += output_bias;

        return output_calculation;
    }

    void Mutate()
    {
        for(int i = 0; i < INPUT_COUNT; i++)
            for(int j = 0; j < NEURON_COUNT; j++)
            {
                if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
                {
                    input_weight[i][j] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                    input_weight[i][j] = std::clamp(input_weight[i][j], -1.0f, 1.0f);
                }
            }

        for(int i = 0; i < NEURON_COUNT; i++)
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