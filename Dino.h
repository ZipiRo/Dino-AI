struct Dino
{
    Vector2f position;
    float width;
    float height;
    bool isOnGround;
    Box collider;
    Box ground_trigger;

    float ground_trigger_width = 20.0f;
    float ground_trigger_height = 10.0f;

    Vector2f linear_velocity;
    float jumpForce;

    RectangleShape rectangle_shape;
    Color color = Color::Black;

    void Init(float x, float width, float height)
    {
        position = Vector2f(x, window_height - ground_thickness - height);
        this->width = width;
        this->height = height;
        collider.Create(position.x, position.y, width, height);
        ground_trigger.Create((collider.right * 0.5f) - (ground_trigger_width * 0.5f), collider.bottom, ground_trigger_width, ground_trigger_height);
        
        rectangle_shape = RectangleShape({1, 1});
        rectangle_shape.setFillColor(color);

        rectangle_shape.setPosition(position);
        rectangle_shape.scale({width, height});
    }

    void Update()
    {
        linear_velocity += gravity_acc * deltaTime;
        position += linear_velocity * deltaTime;

        collider.Create(position.x, position.y, width, height);

        if(IntersectBox(collider, ground_collider))
        {
            position = Vector2f(position.x, ground_collider.top - height);
            collider.Create(position.x, position.y, width, height);
        }

        ground_trigger.Create((collider.right * 0.5f) - (ground_trigger_width * 0.5f), collider.bottom, ground_trigger_width, ground_trigger_height);

        isOnGround = IntersectBox(ground_trigger, ground_collider);

        if(isOnGround)
        {
            std::cout << "On Ground!\n";
        }

    }

    void Draw()
    {
        rectangle_shape.setPosition(position);
        window.draw(rectangle_shape);
    }

    void Jump()
    {
        if(isOnGround)
        {
            linear_velocity.y = -jumpForce;
        }
    }
};