#pragma once

class DisplayChar
{
private:
    // VAO
    VAO* vao;       // VAO, that the char is saved in
    BMPFont* font;
    int vaoOffset;  // offset in the VAO where exactly the char is saved
    bool vaoChanged;

    // defaults
    vec2 defaultPos;    // normal position  
    float defaultScale; // normal scale

    // display
    byte c;         // displayed char
    vec2 pos;       // position (centered)
    vec2 scale;     // scaling
    float rotation; // rotation in degree
    Color color;    // color

    // properties
    bool shaking;           // random rotation and scale each frame
    bool gravity;           // constant downward acceleration
    vec2 velocity;          // speed at which the char is moving
    float angularVelocity;  // speed at which the char is rotation in degree/sec

    void updateVAO();

public:
    DisplayChar(VAO* vao, BMPFont* font, int vaoOffset, vec2 defaultPos, float defaultScale);

    void update(float deltaTime);
    void render();

    void reset(); // reset position/motion

    // getter
    byte getChar() const;
    Color getColor() const;
    vec2 getPos() const;
    vec2 getVelocity() const;
    float getRotation() const;
    float getAngularVelocity() const;
    bool isShaking() const;
    bool hasGravity() const;


    // setter that automatically force a VAO update before rendering if neccessary
    void setChar(byte c);
    void setColor(Color color);

    void setPos(vec2 pos);    
    void setVelocity(vec2 velocity);
    void setRotation(float rotation);
    void setAngularVelocity(float angularVelocity);
    
    void setShaking(bool shaking);
    void setGravity(bool gravity);

    static const ivec2 pixelSize;
    static const float pixelAspect;
};

