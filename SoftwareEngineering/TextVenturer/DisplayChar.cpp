#include "stdafx.h"

#include "DisplayChar.h"
                      
const ivec2 DisplayChar::pixelSize = ivec2(11, 16);
const float DisplayChar::pixelAspect = (float)DisplayChar::pixelSize.x / DisplayChar::pixelSize.y;
                         
void DisplayChar::updateVAO()
{
    if (!vaoChanged)
        return;

    struct {
        vec2 pos;
        vec2 texcoord;
        Color color;
    } data[6];

    vec2 right = (scale * vec2(2, 0) * font->getWidth(c)).rotate(rotation);
    vec2 up = (scale * vec2(0, 1)).rotate(rotation);  

    data[0].pos = pos - right - up;
    data[0].color = color;
    data[0].texcoord = font->getTexCoord(c, vec2(0, 0));
    data[1].pos = pos + right - up;
    data[1].color = color;
    data[1].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 0));
    data[2].pos = pos + right + up;
    data[2].color = color;
    data[2].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 1));
    data[3].pos = pos + right + up;
    data[3].color = color;
    data[3].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 1));
    data[4].pos = pos - right + up;
    data[4].color = color;
    data[4].texcoord = font->getTexCoord(c, vec2(0, 1));
    data[5].pos = pos - right - up;
    data[5].color = color;
    data[5].texcoord = font->getTexCoord(c, vec2(0, 0));
    
    vao->setVertices(vaoOffset, 6, data);

    vaoChanged = false;
}

DisplayChar::DisplayChar(VAO* vao, BMPFont* font, int vaoOffset, vec2 defaultPos, float defaultScale)
{
    this->vao = vao;
    this->font = font;
    this->vaoOffset = vaoOffset;
    this->defaultPos = defaultPos;
    this->defaultScale = defaultScale;

    vaoChanged = true;

    c = 32; // "invisible" space character
   
    reset();
}

DisplayChar::DisplayChar(const DisplayChar & other)
{
    *this = other;
}

DisplayChar & DisplayChar::operator=(const DisplayChar & other)
{
    setChar(other.c);
    setColor(other.color);

    if (other.hasMoved())
        setPos(other.pos);
    
    setVelocity(other.velocity);
    setRotation(other.rotation);
    setAngularVelocity(other.angularVelocity);

    setShaking(other.shaking);
    setGravity(other.gravity);

    return *this;
}

void DisplayChar::update(float deltaTime)
{   
    if (gravity)
        velocity = velocity + vec2(0, -1) * deltaTime;
    if (pos.y < -10)
    {
        reset();
    }
    setPos(pos + velocity * deltaTime);
    setRotation(rotation + angularVelocity * deltaTime);
}

void DisplayChar::render()
{
    updateVAO();
}

void DisplayChar::reset(bool clearChar)
{
    setPos(defaultPos);
    setScale(defaultScale);
    setRotation(0);

    setShaking(false);
    setGravity(false);
    setVelocity(vec2(0, 0));
    setAngularVelocity(0);

    if (clearChar)
        setChar(' ');

    moved = false;
}

byte DisplayChar::getChar() const
{
    return c;
}

Color DisplayChar::getColor() const
{
    return color;
}

vec2 DisplayChar::getPos() const
{
    return pos;
}

vec2 DisplayChar::getScale() const
{
    return scale;
}

vec2 DisplayChar::getVelocity() const
{
    return velocity;
}

float DisplayChar::getRotation() const
{
    return rotation;
}

float DisplayChar::getAngularVelocity() const
{
    return angularVelocity;
}

bool DisplayChar::isShaking() const
{
    return shaking;
}

bool DisplayChar::hasGravity() const
{
    return gravity;
}

bool DisplayChar::hasMoved() const
{
    return moved;
}

void DisplayChar::setChar(byte c)
{
    if (this->c == c)
        return;
    this->c = c;
    vaoChanged = true;
}

void DisplayChar::setColor(Color color)
{
    if (this->color == color)
        return;
    this->color = color;
    vaoChanged = true;
}

void DisplayChar::setPos(vec2 pos)
{
    if (this->pos == pos)
        return;
    this->pos = pos;
    vaoChanged = true;
    moved = true;
}

void DisplayChar::setScale(vec2 scale)
{
    if (this->scale == scale)
        return;
    this->scale = scale;
    vaoChanged = true;
}

void DisplayChar::setScale(float scale)
{
    if (this->scale == vec2(scale, scale))
        return;
    this->scale = vec2(scale, scale);
    vaoChanged = true;
}

void DisplayChar::setVelocity(vec2 velocity)
{
    this->velocity = velocity;
}

void DisplayChar::setRotation(float rotation)
{
    if (this->rotation == rotation)
        return;
    this->rotation = rotation;
    vaoChanged = true;
}

void DisplayChar::setAngularVelocity(float angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

void DisplayChar::setShaking(bool shaking)
{
    this->shaking = shaking;
}

void DisplayChar::setGravity(bool gravity)
{
    this->gravity = gravity;
}
