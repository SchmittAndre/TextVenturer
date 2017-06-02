#include "stdafx.h"
#include "BaseGame.h"

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
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("WIN32PROJECT");
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

void GLWindow::initGL()
{
    dc = GetDC(wnd);

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

    fbo = NULL;
}

void GLWindow::finalizeGL()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(rc);
    ReleaseDC(wnd, dc);
    delete fbo;
}

void GLWindow::showException(bool canContinue)
{
    pause();
    try
    {
        throw;
    }
    catch (const ENotImplemented & e)
    {
        std::string msg(e.what());
        e.debugOutput();
        MessageBoxA(wnd, msg.c_str(), "TextVenturer - Information", MB_OK | MB_ICONINFORMATION);
    }
    catch (const Exception & e)
    {
        std::string msg(e.what());  
        e.debugOutput(); 
        if (canContinue)
        {
            msg += "\r\n\r\nContinue and risk data corruption?";
            int result = MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OKCANCEL | MB_ICONERROR);
            if (result == IDCANCEL)
                stop();
        }
        else
        {
            MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OK | MB_ICONERROR);
            stop();
        }
    }
    catch (const std::exception & e)
    {
        std::string msg(e.what());
        if (canContinue)
        {
            msg += "\r\n\r\nContinue and risk data corruption?";
            int result = MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OKCANCEL | MB_ICONERROR);
            if (result == IDCANCEL)
                stop();
        }
        else
        {
            MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OK | MB_ICONERROR);
            stop();
        }
    }
    catch (...)
    {
        std::string msg("An error, not using std::exception, occured!");
        if (canContinue)
        {
            msg += "\r\n\r\nContinue and risk data corruption?";
            int result = MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OKCANCEL | MB_ICONERROR);
            if (result == IDCANCEL)
                stop();
        }
        else
        {
            MessageBoxA(wnd, msg.c_str(), "TextVenturer - Unhandeled Exception", MB_OK | MB_ICONERROR);
            stop();
        }
    }
    resume();
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

    initGL();
    samples = 1;

    paused = false;
}

GLWindow::~GLWindow()
{
}

LRESULT GLWindow::WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if FALSE
    std::stringstream debugString;
    debugString << "Message: " << msg;
    debugString.flags(debugString.flags() | std::stringstream::hex);
    debugString << " with " << wParam << " / " << lParam << std::endl;
    OutputDebugStringA(debugString.str().c_str());
#endif                    

    GLWindow* window = (GLWindow*)GetWindowLongPtr(wnd, GWLP_USERDATA);
      
    try
    {
        switch (msg)
        {
        case WM_DESTROY:
            window->finalizeGL();
            PostQuitMessage(0);
            return FALSE;
        case WM_CLOSE:
            DestroyWindow(wnd);
            return FALSE;
        case WM_PAINT:
            try
            {
                window->game->update();
                window->draw();
            }
            catch (...)
            {
                window->showException();
            }
            return FALSE;
        case WM_ERASEBKGND:
            // don't erase anything
            return FALSE;
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_KEYMENU:
                // pressing alt should not do that stupid keymenu and pause everything
                return FALSE;
            }
            break;
        }
        case WM_SIZE:
        {
            switch (wParam)
            {
            case SIZE_RESTORED:
            case SIZE_MAXIMIZED:
                GetClientRect(wnd, &window->clientRect);
                window->resume();

                window->width = window->clientRect.right - window->clientRect.left;
                window->height = window->clientRect.bottom - window->clientRect.top;
                glViewport(0, 0, window->width, window->height);
                window->game->resize(window->width, window->height);
                if (window->isMultisampled())
                    window->fbo->resize(window->width, window->height);
                break;
            case SIZE_MINIMIZED:
                window->pause();
                break;
            }
            break;
        }
        case WM_CHAR:
            if (wParam >= 32 && wParam <= 255 && wParam != 127)
                window->game->pressChar((byte)wParam);
            break;
        case WM_KEYDOWN:
            if ((wParam & ~0xFF) == 0)
                window->game->pressKey(static_cast<byte>(wParam));
            break;
        case WM_GETMINMAXINFO: 
        {
            RECT client;
            RECT window;
            GetClientRect(wnd, &client);
            GetWindowRect(wnd, &window);
            int diffx = (window.right - window.left) - (client.right - client.left);
            int diffy = (window.bottom - window.top) - (client.bottom - client.top);
            MINMAXINFO* minmax = reinterpret_cast<MINMAXINFO*>(lParam);
            minmax->ptMinTrackSize.x = GLWindow::defaultWidth / 2 + diffx;
            minmax->ptMinTrackSize.y = GLWindow::defaultHeight / 2 + diffy;
            break;
        }          
        case WM_NCHITTEST:
        {
            auto result = DefWindowProc(wnd, msg, wParam, lParam);
            if (result == HTCLIENT)
                result = HTCAPTION;
            return result;
        }   
        }
    }
    catch (...)
    {
        window->showException();
    }    

    return DefWindowProc(wnd, msg, wParam, lParam);
}

void GLWindow::start(BaseGame & game)
{
    this->game = &game;      

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
    errorTimeout = 0;
    MSG msg;         
    BOOL bRet = 0;
    while (bRet = GetMessage(&msg, NULL, 0, 0))
    {
        if (bRet == -1)
        {
            try
            {
                throw(Exception, getErrorString(GetLastError()));
            }
            catch (...)
            {
                showException();
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }          
}

void GLWindow::stop()
{
    DestroyWindow(wnd);
}

void GLWindow::setVSync(bool vsync) const
{
    FBO::bindScreen(width, height);
    wglSwapIntervalEXT(vsync ? 1 : 0);
}

void GLWindow::setCaption(std::wstring caption) const
{
    SetWindowText(wnd, caption.c_str());
}

void GLWindow::setMultisampling(bool multisampling)
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
            throw(ENotSupported, "Multi-Sampling");
        }
    }
    else if (!multisampling && isMultisampled())
    {
        // turn off
        delete fbo;
        fbo = NULL;
    }
}

void GLWindow::setSamples(int samples)
{
    if (this->samples == samples && isMultisampled())
        return;
    this->samples = samples;
    setMultisampling(true);
    if (samples < 1 || samples > maxSamples)
        throw(ENotSupported, "Multi-Sampling with " + std::to_string(samples) + " samples");
    fbo->setSamples(samples);
}

bool GLWindow::isMultisampled() const
{
    return fbo;
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
    if (paused)
        return;
    paused = true;
    ValidateRgn(wnd, NULL);
}

void GLWindow::resume()
{
    if (!paused)
        return;
    paused = false;
    InvalidateRgn(wnd, NULL, FALSE);
}

float GLWindow::getScale()
{
    return min(1, getAspect() / defaultAspect);
}

float GLWindow::getAspect()
{
    return (float)width / height;
}

void GLWindow::draw() 
{
    if (paused)
        return;
    
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

    errorTimeout -= game->getDeltaTime();
    if (errorTimeout < 0)
    {
        errorTimeout = 3;
        throwGLError();
    }  

    if (!SwapBuffers(dc))
    {
        throw(Exception, getErrorString(GetLastError()));
    }
}
