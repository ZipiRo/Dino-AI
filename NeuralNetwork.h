float Sigmoid(float x)
{
    return 1.0f / (1.0f + std::exp(-x));    
}

const float MUTATE_AMOUNT = 0.1f;
const float MUTATION_RATE = 0.1f;

const int INPUT_COUNT = 5;
const int NEURON_COUNT = 8;
const int OUTPUT_COUNT = 1;

struct NeuralNetwork
{  
private:
    std::vector<float> outputs;

public:
    float input_weight[INPUT_COUNT][NEURON_COUNT];
    float input_bias[NEURON_COUNT];

    float output_weight[NEURON_COUNT][OUTPUT_COUNT];
    float output_bias[OUTPUT_COUNT];

    void Init()
    {
        outputs.resize(OUTPUT_COUNT);

        for(int i = 0; i < INPUT_COUNT; i++)
            for(int j = 0; j < NEURON_COUNT; j++)
                input_weight[i][j] = RandomFloatRange(-1.0f, 1.0f);

        for(int i = 0; i < NEURON_COUNT; i++)
            for(int j = 0; j < OUTPUT_COUNT; j++)
                output_weight[i][j] = RandomFloatRange(-1.0f, 1.0f);
        
        for(int i = 0; i < NEURON_COUNT; i++)
            input_bias[i] = RandomFloatRange(-1.0f, 1.0f);

        for(int i = 0; i < OUTPUT_COUNT; i++)
            output_bias[i] = RandomFloatRange(-1.0f, 1.0f);
    }
    
    std::vector<float> Think(const std::vector<float> &inputs)
    {
        float hidden[NEURON_COUNT];

        for(int i = 0; i < NEURON_COUNT; i++)
        {
            float input_calculation = 0.0f;

            for(int j = 0; j < INPUT_COUNT; j++)
                input_calculation += inputs[j] * input_weight[j][i];

            input_calculation += input_bias[i]; 
            
            hidden[i] = std::tanh(input_calculation);
        }

        for(int i = 0; i < OUTPUT_COUNT; i++)
        {
            float output_calculation = 0.0f;

            for(int j = 0; j < NEURON_COUNT; j++)
                output_calculation += hidden[j] * output_weight[j][i];

            output_calculation += output_bias[i];

            outputs[i] = Sigmoid(output_calculation);
        }

        return outputs;
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
            for(int j = 0; j < OUTPUT_COUNT; j++)
            {
                if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
                {
                    output_weight[i][j] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                    output_weight[i][j] = std::clamp(output_weight[i][j], -1.0f, 1.0f);
                }
            }

        for(int i = 0; i < NEURON_COUNT; i++)
        {
            if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
            {
                input_bias[i] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                input_bias[i] = std::clamp(input_bias[i], -1.0f, 1.0f);
            }
        }
        
        for(int i = 0; i < OUTPUT_COUNT; i++)
        {
            if(RandomFloatRange(0.0f, 1.0f) < MUTATION_RATE)
            {
                output_bias[i] += RandomFloatRange(-MUTATE_AMOUNT, MUTATE_AMOUNT);
                output_bias[i] = std::clamp(output_bias[i], -1.0f, 1.0f);
            }
        }
    } 
};