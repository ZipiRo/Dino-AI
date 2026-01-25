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