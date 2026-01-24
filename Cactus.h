struct Cactus
{ 
    Vector2f position;
    float width;
    float height;
    Box collider;

    RectangleShape rectangle_shape;
    Color color = Color::Green;

    float collider_y_offset = 10.0f;

    std::vector<bool> reward_hit;

    void Init(float x, float width, float height)
    {
        position = Vector2f(x, window_height - ground_thickness - height);
        this->width = width;
        this->height = height;
        collider.Create(x, position.y + collider_y_offset, width, height - collider_y_offset);

        rectangle_shape = RectangleShape({1, 1});
        rectangle_shape.setFillColor(color);

        rectangle_shape.setPosition(position);
        rectangle_shape.scale({width, height});
    }

    void Update()
    {
        collider.Create(position.x, position.y + collider_y_offset, width, height - collider_y_offset);
    }

    void Draw()
    {
        rectangle_shape.setFillColor(color);
        rectangle_shape.setPosition(position);
        window.draw(rectangle_shape);
    }
};