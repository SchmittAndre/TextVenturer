#pragma once

class BaseGame abstract
{
private:
    float deltaTime;
    float fps;
    LARGE_INTEGER lastTime, frequency;

    void updateDeltaTime();

protected:
    virtual void update(float deltaTime) = 0;

public:
    BaseGame();

    float getDeltaTime() const;
    float getFPS() const;
    float getRawFPS() const;

    void update();
    virtual void render() = 0;
    virtual void resize(int width, int height) = 0;

    virtual void pressChar(byte c) {};
    virtual void pressKey(byte key) {};
};

