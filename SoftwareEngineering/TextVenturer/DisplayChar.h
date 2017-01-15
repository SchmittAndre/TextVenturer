#pragma once

class DisplayChar
{
private:
    // VAO
    VAO* vao;       // VAO, that the char is saved in
    BMPFont* font;
    int vaoOffset;  // offset in the VAO where exactly the char is saved
    bool vaoChanged;
    float aspect;

    // defaults
    vec2 defaultPos;    // normal position  
    float baseScale;    // normal scale

    // display
    byte c;         // displayed char
    vec2 pos;       // position (centered)
    vec2 scale;     // scaling
    float rotation; // rotation in degree
    Color color;    // color

    // properties
    bool shaking;           // random rotation and scale each frame
    vec2 velocity;          // speed at which the char is moving
    vec2 acceleration;      // speed at which the velocity changes
    float angularVelocity;  // speed at which the char is rotating in degree/sec
    float rainbowVelocity;  // speed at which the char is changing its color

    void updateVAO();

public:
    DisplayChar(VAO* vao, BMPFont* font, int vaoOffset, vec2 defaultPos, float defaultScale, float aspect);
    DisplayChar(const DisplayChar & other);

    DisplayChar & operator=(const DisplayChar & other);

    void update(float deltaTime);
    void render();

    void reset(bool clearChar = false);

    // getter
    byte getChar() const;
    vec2 getPos() const;
    vec2 getScale() const;
    float getRotation() const;
    Color getColor() const;

    bool isShaking() const;
    vec2 getVelocity() const;
    vec2 getAcceleration() const;
    float getAngularVelocity() const;
    float getRainowSpeed() const;

    // setter that automatically force a VAO update before rendering if neccessary
    void setChar(byte c);
    void setPos(vec2 pos);    
    void setScale(vec2 scale);
    void setScale(float scale);
    void setRotation(float rotation);
    void setColor(Color color);

    void setShaking(bool shaking);
    void setVelocity(vec2 velocity);
    void setAcceleration(vec2 acceleration);
    void setAngularVelocity(float angularVelocity);     
    void setRainbowVelocity(float rainbowVelocity);

    float maxRadius() const;

    bool isMoving() const;

    static const ivec2 pixelSize;
    static const float pixelAspect;
};