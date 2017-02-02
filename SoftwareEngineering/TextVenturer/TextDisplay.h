#pragma once

#include "DisplayChar.h"
#include "AsciiArt.h"

class TextDisplay
{
public:     
    struct State
    {               
        // values
        vec2 offset;            // an offset, where the char is placed
        vec2 scale;             // the scale of the char
        float rotation;         // the rotation of the char in degrees      
        Color color;            // the color of the char
        float shaking;          // how much the char is chaking

        // value changers
        vec2 velocity;          // an initial motion of a char
        vec2 acceleration;      // a constant acceleration, for example gravity
        float angularVelocity;  // the speed, at which the char is rotating
        float rainbowVelocity;  // the speed, at which the char is changing color

        // between char
        float rainbow;          // the amount, that the color changes per char
        vec2 offsetMovement;    // how much the offset changes per char
        float delay;            // delay between two chars    

        float time;             // when this reaches zero it gets set to delay

        State();
        void reset();
        void processCommand(const string & command, const vector<float> & params);
        void nextChar();

        static const float DefaultDelay;
    };

private:
    VAO* vao;

    DisplayChar*** text;
    DisplayChar* cursorChar;

    ivec2 cursorPos;
    bool cursorVisible;

    float cursorTime;

    BMPFont* font;

    size_t width;
    size_t height;

    size_t subDataMaxChanges;
    bool useSubData;

public:
    TextDisplay(Shader* textShader, BMPFont* font, size_t width, size_t height, float aspect);
    virtual ~TextDisplay();

    vec2 getCharPos(ivec2 pos) const;

    void write(int x, int y, const string & str);
    void write(ivec2 p, const string & str);

    void write(int x, int y, const byte c, const State & state = State());
    void write(ivec2 p, const byte c, const State & state = State());

    void writeStep(int & x, int y, string & str, State & state);
    void writeStep(ivec2 & p, string & str, State & state);

    void draw(int x, int y, const AsciiArt & art);
    void draw(ivec2 p, const AsciiArt & art);

    void move(ivec2 src, uvec2 size, ivec2 dest);

    void update(float deltaTime);
    void render();

    size_t getWidth() const;
    size_t getHeight() const;

    void setCursorVisible(bool visible);
    bool getCursorVisible() const;
    void resetCursorTime();

    void setCursorPos(int x, int y);
    void setCursorPos(ivec2 pos);
    ivec2 getCursorPos() const;

    bool isColumnVisible(int x) const;
    bool isLineVisible(int y) const;
    bool isVisible(int x, int y) const;
    bool isVisible(ivec2 p) const;

    string getLine(size_t y, size_t offset = 0, size_t count = string::npos) const;

    byte getChar(int x, int y) const;
    byte getChar(ivec2 p) const;

	void clearLine(int y, size_t offset = 0, size_t count = string::npos);
	void clear();
};

