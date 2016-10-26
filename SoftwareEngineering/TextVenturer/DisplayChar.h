#pragma once
class DisplayChar
{
private:
    // VAO
    VAO* vao;       // VAO, that the char is saved in
    int vaoOffset;  // offset in the VAO where exactly the char is saved

    // display
    byte c;         // displayed char
    vec2 pos;       // position
    vec2 scale;     // scaling
    float rotation; // rotation in degree
    Color color;    // color

    // properties
    bool shaking;           // random rotation and scale each frame
    bool gravity;           // constant downward acceleration
    vec2 velocity;          // speed at which the char is moving
    float angularVelocity;  // speed at which the char is rotation in degree/sec

public:
    DisplayChar(VAO* vao, int vaoOffset);

    void update();
};

