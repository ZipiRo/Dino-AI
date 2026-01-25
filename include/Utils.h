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