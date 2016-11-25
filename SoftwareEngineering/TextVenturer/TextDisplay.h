#pragma once

class DisplayChar;
class AsciiArt;

class TextDisplay
{
private:
    VAO* vao;

    DisplayChar*** text;
    DisplayChar* cursorChar;

    ivec2 cursorPos;
    bool cursorVisible;

    float cursorTime;

    BMPFont* font;

    int width;
    int height;

public:
    TextDisplay(Shader* textShader, BMPFont* font, int width, int height);
    ~TextDisplay();

    vec2 getCharPos(ivec2 pos);

    void write(int x, int y, const string & str);
    void write(ivec2 p, const string & str);

    void write(int x, int y, const byte c);
    void write(ivec2 p, const byte c);

    void draw(int x, int y, const AsciiArt & art);
    void draw(ivec2 p, const AsciiArt & art);

    void update(float deltaTime);
    void render();

    int getWidth();
    int getHeight();

    void setCursorVisible(bool visible);
    bool getCursorVisible();
    void resetCursorTime();

    void setCursorPos(ivec2 pos);
    ivec2 getCursorPos();

    byte getChar(int x, int y);
	string getLine(int y);
    byte getChar(ivec2 p);
	void clearline(int y);
	void clear();
};

