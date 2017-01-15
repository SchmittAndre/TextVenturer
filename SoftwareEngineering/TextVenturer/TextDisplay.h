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

    size_t width;
    size_t height;

public:
    TextDisplay(Shader* textShader, BMPFont* font, size_t width, size_t height);
    virtual ~TextDisplay();

    vec2 getCharPos(ivec2 pos) const;

    void write(int x, int y, const string & str);
    void write(ivec2 p, const string & str);

    void write(int x, int y, const byte c);
    void write(ivec2 p, const byte c);

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

