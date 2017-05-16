#include "stdafx.h"
#include "BaseGame.h"
#include "resource1.h"

#include "Window.h"

const int GLWindow::defaultWidth = 660;
const int GLWindow::defaultHeight = GLWindow::defaultWidth * 4 / 5;
const float GLWindow::defaultAspect = (float)GLWindow::defaultWidth / GLWindow::defaultHeight;

ATOM GLWindow::myRegisterClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
                          
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(MAINICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("WIN32PROJECT");
    wcex.hIconSm = wcex.hIcon;

    return RegisterClassEx(&wcex);
}

void GLWindow::initGL()
{                     
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    switch (GetObjectType(dc))
    {
    case OBJ_MEMDC:
    case OBJ_ENHMETADC:
    case OBJ_METADC:
        pfd.dwFlags |= PFD_DRAW_TO_BITMAP;
        break;
    default:
        pfd.dwFlags |= PFD_DRAW_TO_WINDOW;
    }

    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 0;
    pfd.cAccumBits = 0;
    pfd.cAuxBuffers = 0;

    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(dc, &pfd);

    if (GetPixelFormat(dc) != pixelFormat)
        SetPixelFormat(dc, pixelFormat, &pfd);

    DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    HGLRC legacyRC = wglCreateContext(dc);
    wglMakeCurrent(dc, legacyRC);

    glewInit();

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,  
        0};

    rc = wglCreateContextAttribsARB(dc, NULL, attribs);

    wglDeleteContext(legacyRC);

    wglMakeCurrent(dc, rc);

    // activate blending with best blending func to get nice transparency (which we probably won't use though)
    glEnable(GL_BLEND);
    glBlendFunc(bfsSrcAlpha, bfdOneMinusSrcAlpha);

    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
}

GLWindow::GLWindow(HINSTANCE instance, LPCTSTR title)
{
    width = defaultWidth;
    height = defaultHeight;

    this->instance = instance;
    UINT result = myRegisterClass();
    wnd = CreateWindow(
        _T("WIN32PROJECT"), 
        title, 
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        width, 
        height, 
        NULL, 
        NULL, 
        instance, 
        NULL
    );

    // since the WndProc must be a static Callback function, it doesn't have access to the GLWindow object
    // GWL_USERDATA is used to store the GLWindow object pointer
    SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)this);

    dc = GetDC(wnd);
    initGL();
    samples = 1;
    fbo = NULL;

    paused = false;
}

GLWindow::~GLWindow()
{
    delete fbo;
    wglMakeCurrent(dc, NULL);
    wglDeleteContext(rc);
    ReleaseDC(wnd, dc);
    DestroyWindow(wnd);      
}

LRESULT GLWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GLWindow* window = (GLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CLOSE:
        // ErrorDialog("Aha!", "I detected you want to close me!");
        // PostQuitMessage(0);
        // this function should work, but for some reason the PeekMessage doesn't react sometimes
        // since WM_CLOSE is sent though, we are just setting a flag here that should in theory fix the issue
        window->stop();
        return FALSE;
    case WM_PAINT:
    {
        // just draw our scene with OpenGL
        window->draw();
        RECT r;
        GetClientRect(hWnd, &r);
        ValidateRect(hWnd, &r);
        return FALSE;
    }
    case WM_ERASEBKGND:
        // don't erase anything
        return TRUE;
    case WM_SYSCOMMAND:
    {
        // pressing alt should not do that stupid keymenu and pause everything
        switch (wParam)
        {
        case SC_KEYMENU:
            return FALSE;
        case SC_MINIMIZE:
            window->pause();
            break;
        case SC_RESTORE:
            window->resume();
            break;
        }                     
        break;
    }
    case WM_SIZE:
    {
        RECT r;
        GetClientRect(hWnd, &r);
        window->width = r.right - r.left;
        window->height = r.bottom - r.top;
        glViewport(0, 0, window->width, window->height);
        window->game->resize(window->width, window->height);
        if (window->isMultisampled())
            window->fbo->resize(window->width, window->height);
        return FALSE;
    }
    case WM_CHAR:
        if (wParam >= 32 && wParam <= 255 && wParam != 127)
            window->game->pressChar((byte)wParam);
        break;
    case WM_KEYDOWN:
        window->game->pressKey((byte)wParam);
        break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minmax = reinterpret_cast<MINMAXINFO*>(lParam);
        minmax->ptMinTrackSize.x = 400;
        minmax->ptMinTrackSize.y = 320;
        break; 
    }
    }   
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void GLWindow::start(BaseGame* game)
{
    this->game = game;      

    // make inner size to defined width and height
    RECT client, window;
    GetClientRect(wnd, &client);
    GetWindowRect(wnd, &window);
    int wdiff = (window.right - window.left) - (client.right - client.left);
    int hdiff = (window.bottom - window.top) - (client.bottom - client.top);
    int newWidth = width + wdiff;
    int newHeight = height + hdiff;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(
        wnd,
        NULL,
        (screenWidth - newWidth) / 2,
        (screenHeight - newHeight) / 2,
        newWidth,
        newHeight,
        SWP_NOZORDER | SWP_NOREDRAW
        );
    
    ShowWindow(wnd, SW_SHOW);

    // Main Loop
    MSG msg;
    while (!gameShouldStop)
    {
        if (paused)
        {
            // Don't want full CPU usage while doing nothing, except for waiting for a message
            // Can't accomplish this wait, using GetMessage, because that does nothing, as long as the window is minimized
            Sleep(1);
        }
        else
        {
            // first render the first scene, in case a big batch of messages has to get handled
            game->update();
            draw();
        }

        // then check for one incoming messages and process it (all causes lag in some cases)      
        if (PeekMessage(&msg, wnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        
        // repeat ~
    }
}

void GLWindow::stop()
{
    gameShouldStop = true;
    paused = false;
}

void GLWindow::setVSync(bool vsync) const
{
    FBO::bindScreen(width, height);
    wglSwapIntervalEXT(vsync ? 1 : 0);
}

void GLWindow::setCaption(std::string caption) const
{
    SetWindowText(wnd, CA2CT(caption.c_str()));
}

bool GLWindow::setMultisampling(bool multisampling)
{
    if (multisampling && !isMultisampled())
    {
        // turn on 
        fbo = new FBO(width, height);
        fbo->enableRenderBufferMS(fbaColor, pfRGBA, samples);
        fbo->enableRenderBufferMS(fbaDepth, pfDepthComponent, samples);
        if (!fbo->finish())
        {
            delete fbo;
            fbo = NULL;
            return false;
        }
    }
    else if (!multisampling && isMultisampled())
    {
        // turn off    
        delete fbo;
        fbo = NULL;
    }
    return true;
}

bool GLWindow::setSamples(int samples)
{
    if (this->samples == samples && isMultisampled())
        return true;
    this->samples = samples;
    if (!setMultisampling(true))
        return false;
    else
    {
        if (samples < 1 || samples > maxSamples)
            return false;
        fbo->setSamples(samples);
        return true;
    }
}

bool GLWindow::isMultisampled() const
{
    return fbo != NULL;
}

int GLWindow::getSamples() const
{
    return isMultisampled() ? samples : 0;
}

int GLWindow::getMaxSamples() const
{
    return maxSamples;
}

void GLWindow::pause()
{
    paused = true;
}

void GLWindow::resume()
{
    paused = false;
}

float GLWindow::getScale()
{
    return min(1, getAspect() / defaultAspect);
}

float GLWindow::getAspect()
{
    return (float)width / height;
}

void GLWindow::draw() const
{         
    if (isMultisampled())
    {
        fbo->bind();
        glClear(amColorDepth);
        game->render();
        fbo->copyToScreen(amColor);
    }
    else
    {
        FBO::bindScreen(width, height);
        glClear(amColorDepth);
        game->render();
    }
    SwapBuffers(dc);
}
