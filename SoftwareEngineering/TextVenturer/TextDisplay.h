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
    virtual ~TextDisplay();

    vec2 getCharPos(ivec2 pos) const;

    void write(int x, int y, const string & str);
    void write(ivec2 p, const string & str);

    void write(int x, int y, const byte c);
    void write(ivec2 p, const byte c);

    void draw(int x, int y, const AsciiArt & art);
    void draw(ivec2 p, const AsciiArt & art);

    void update(float deltaTime);
    void render();

    int getWidth() const;
    int getHeight() const;

    void setCursorVisible(bool visible);
    bool getCursorVisible() const;
    void resetCursorTime();

    void setCursorPos(ivec2 pos);
    ivec2 getCursorPos() const;

    string getLine(int y) const;

    byte getChar(int x, int y) const;
    byte getChar(ivec2 p) const;
	void clearline(int y);
	void clear();
};

