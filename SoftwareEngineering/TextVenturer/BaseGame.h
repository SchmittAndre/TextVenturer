#pragma once
class BaseGame abstract
{
public:
    virtual void update() = 0;
    virtual void render() = 0; 
    virtual void resize(int width, int height) = 0;

    virtual void pressChar(byte c) {};
};

