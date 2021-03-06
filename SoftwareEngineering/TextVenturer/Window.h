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
    void finalizeGL();

    // Callback function for windows messages for our window
    static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // clears, calls the game render function and swaps the buffers
    void draw();

    // an FBO and multisampling
    FBO * fbo;     
    int samples;

    GLint maxSamples;

    bool paused;
    int width;
    int height;

    RECT clientRect;

    float errorTimeout;

    std::mutex exceptionLock;

public:
    GLWindow(HINSTANCE hInst, LPCTSTR title);
    virtual ~GLWindow();

    // show the window and start the game
    void start(BaseGame & game);

    // stop the program
    void stop();

    void setVSync(bool vsync) const;
    void setCaption(std::wstring caption) const;

    void setMultisampling(bool multisampling);
    void setSamples(int samples);
    bool isMultisampled() const;
    int getSamples() const;
    int getMaxSamples() const;

    void pause();
    void resume();

    float getScale();
    float getAspect();

    void showException(bool canContinue = true);

    const static int defaultWidth;
    const static int defaultHeight;
    const static float defaultAspect;
};
