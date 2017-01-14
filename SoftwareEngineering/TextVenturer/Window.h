#pragma once

class BaseGame;

class GLWindow
{
private:
    // Application Handle
    HINSTANCE instance; 

    // Window Handle
    HWND wnd; 
              
    // Device Context
    HDC dc;   

    // Rendering Context
    HGLRC rc; 

    // Pointer to GameObject with render and update function, passed in start function
    BaseGame* game; 

    // function to register our FormClass
    ATOM myRegisterClass(); 

    // init everything OpenGL related, so we can render onto the form
    void initGL(); 

    // Callback function for windows messages for our window
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // clears, calls the game render function and swaps the buffers
    void draw() const;

    // a flag, that forces the game loop to exit
    bool gameShouldStop;

public:
    GLWindow(HINSTANCE hInst, LPCTSTR title);
    virtual ~GLWindow();

    // show the window and start the game
    void start(BaseGame* game);

    // stop the program
    void stop();

    void setVSync(bool vsync) const;

    const static int width;
    const static int height;

};

