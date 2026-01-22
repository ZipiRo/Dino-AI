struct Dino
{
    Vector2f position;
    float width;
    float height;
    bool isOnGround = false;
    Box collider;
    Box ground_trigger;

    float ground_trigger_height = 10.0f;

    Vector2f linear_velocity;
    float jump_force = 650.0f;

    RectangleShape rectangle_shape;
    Color color = Color::Black;

    bool isJumping = false;
    bool jumpHeld = false;

    void Init(float x, float width, float height)
    {
        position = Vector2f(x, window_height - ground_thickness - height);
        this->width = width;
        this->height = height;
        collider.Create(position.x, position.y, width, height);
        ground_trigger.Create(position.x, collider.bottom, width, ground_trigger_height);
        
        linear_velocity = Vector2f(0.0f, 0.0f);

        rectangle_shape = RectangleShape({1, 1});
        rectangle_shape.setFillColor(color);

        rectangle_shape.setPosition(position);
        rectangle_shape.scale({width, height});
    }

    void Update()
    {
        float gravity = GRAVITY;

        if(linear_velocity.y < 0.0f)
        {
            if(isJumping && !jumpHeld)
                gravity += 1000;
        }

        linear_velocity += Vector2f(0, gravity) * deltaTime;
        position += linear_velocity * deltaTime;
        rectangle_shape.setPosition(position);

        collider.Create(position.x, position.y, width, height);

        if(collider.bottom > ground_collider.top)
        {
            position = Vector2f(position.x, ground_collider.top - height);
            collider.Create(position.x, position.y, width, height);
            linear_velocity.y = 0.0f;
        }

        ground_trigger.Create(position.x, collider.bottom, width, ground_trigger_height);

        isOnGround = IntersectBox(ground_trigger, ground_collider);
        
        if(isOnGround)
        {
            isJumping = false;
        } 
    }

    void Draw()
    {
        window.draw(rectangle_shape);
    }

    void Jump()
    {
        if(!isOnGround) return;

        isJumping = true;
        jumpHeld = true;
        linear_velocity.y = -jump_force;
    }

    void ReleaseJump()
    {
        jumpHeld = false;
    }
};