#include "stdafx.h"
#include "BaseGame.h"

#include "Window.h"

const int GLWindow::width = 660;
const int GLWindow::height = GLWindow::width * 4 / 5;

ATOM GLWindow::myRegisterClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
                          
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("WIN32PROJECT");
    wcex.hIconSm = NULL;

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
}

GLWindow::GLWindow(HINSTANCE instance, LPCTSTR title)
{
    this->instance = instance;
    UINT result = myRegisterClass();
    wnd = CreateWindow(
        _T("WIN32PROJECT"), 
        title, 
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 
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
    SetWindowLong(wnd, GWL_USERDATA, (LONG)this);

    dc = GetDC(wnd);
    initGL();
    setVSync(true);
}

GLWindow::~GLWindow()
{
    wglMakeCurrent(dc, NULL);
    wglDeleteContext(rc);
    ReleaseDC(wnd, dc);
    DestroyWindow(wnd);      
}

LRESULT GLWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GLWindow* window = (GLWindow*)GetWindowLong(hWnd, GWL_USERDATA);

    switch (msg)
    {
    case WM_CLOSE:
        //ErrorDialog("Aha!", "I detected you want to close me!");
        PostQuitMessage(0);
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
    case WM_SIZE:
    {
        RECT r;
        GetClientRect(hWnd, &r);
        int width = r.right - r.left;
        int height = r.bottom - r.top;
        glViewport(0, 0, width, height);
        window->game->resize(width, height);
        return FALSE;
    }
    case WM_CHAR:
    {
        if (wParam >= 32 && wParam <= 256 || wParam == VK_BACK || wParam == VK_RETURN)
            window->game->pressChar((byte)wParam);
    }
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }   
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
    
    //game->update();
    //UpdateWindow(wnd);

    // Main Loop
    MSG msg;
    while (true)
    {
        while (PeekMessage(&msg, wnd, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        game->update();
        draw();
    }
}

void GLWindow::stop()
{
    PostMessage(wnd, WM_CLOSE, 0, 0);
}

void GLWindow::setVSync(bool vsync)
{
    wglSwapIntervalEXT(vsync ? 1 : 0);
}

void GLWindow::draw()
{         
    glClear(amColorDepth);

    game->render();

    SwapBuffers(dc);
}
