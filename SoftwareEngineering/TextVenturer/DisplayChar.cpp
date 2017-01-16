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

    float r = rotation;
    vec2 s = scale;
    vec2 p = pos;
    if (shaking > 0)
    {
        int interval = (int)floor(80 / shaking + 0.5f);
        DWORD seed = GetTickCount() / interval * vaoOffset + vaoOffset;
        float passed = fmod((float)GetTickCount() / interval, 1.0f);
        default_random_engine random(seed);
        default_random_engine next(seed + vaoOffset);
        uniform_real_distribution<float> rDistribute(-10.0f, 10.0f);
        r += rDistribute(random) * (1 - passed);
        r += rDistribute(next) * passed;
        uniform_real_distribution<float> sDistribute(0.85f, 1.0f / 0.85f);
        vec2 tmp = s;
        s = vec2(tmp.x * sDistribute(random), tmp.y * sDistribute(random)) * (1 - passed);
        s += vec2(tmp.x * sDistribute(next), tmp.y * sDistribute(next)) * passed;
        uniform_real_distribution<float> pxDistribute(-s.x, s.x);
        uniform_real_distribution<float> pyDistribute(-s.y, s.y );
        p += vec2(pxDistribute(random), pyDistribute(random)) * baseScale * 0.1f * (1 - passed);
        p += vec2(pxDistribute(next), pyDistribute(next)) * baseScale * 0.1f * passed;
    }

    vec2 right = (s * vec2(2, 0) * font->getWidth(c) * baseScale).rotate(r);
    vec2 up = (s * vec2(0, 1) * baseScale).rotate(r);

    data[0].pos = p - right - up;
    data[0].color = color;
    data[0].texcoord = font->getTexCoord(c, vec2(0, 0));
    data[1].pos = p + right - up;
    data[1].color = color;
    data[1].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 0));
    data[2].pos = p + right + up;
    data[2].color = color;
    data[2].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 1));
    data[3].pos = p + right + up;
    data[3].color = color;
    data[3].texcoord = font->getTexCoord(c, vec2(font->getWidth(c) * 2, 1));
    data[4].pos = p - right + up;
    data[4].color = color;
    data[4].texcoord = font->getTexCoord(c, vec2(0, 1));
    data[5].pos = p - right - up;
    data[5].color = color;
    data[5].texcoord = font->getTexCoord(c, vec2(0, 0));
    
    vao->setVertices(vaoOffset, 6, data);

    vaoChanged = false;
}

DisplayChar::DisplayChar(VAO* vao, BMPFont* font, int vaoOffset, vec2 defaultPos, float baseScale, float aspect)
{
    this->vao = vao;
    this->font = font;
    this->vaoOffset = vaoOffset;
    this->defaultPos = defaultPos;
    this->baseScale = baseScale;
    this->aspect = aspect;

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
    if (other.isMoving())
        setPos(other.pos);
    else
        setPos(defaultPos + other.pos - other.defaultPos);
    setScale(other.scale);
    setRotation(other.rotation);
    setColor(other.color);

    setShaking(other.shaking);
    setVelocity(other.velocity);
    setAcceleration(other.acceleration);
    setAngularVelocity(other.angularVelocity);
    setRainbowVelocity(other.rainbowVelocity);      

    return *this;
}

void DisplayChar::update(float deltaTime)
{   
    velocity = velocity + acceleration * deltaTime;
    
    float r = maxRadius();
    if (pos.y < -1 - r || pos.y > 1 + r || pos.x < -aspect - r || pos.x > aspect + r)
        reset(true);
    
    setPos(pos + velocity * deltaTime);
    
    setRotation(rotation + angularVelocity * deltaTime);
    
    if (rainbowVelocity != 0)
        setColor(color.addRainbow(rainbowVelocity));

    if (shaking)
        vaoChanged = true;
}

void DisplayChar::render()
{
    updateVAO();
}

void DisplayChar::reset(bool clearChar)
{
    setPos(defaultPos);
    setScale(vec2(1, 1));
    setRotation(0);

    setShaking(false);
    setAcceleration(vec2(0, 0));
    setVelocity(vec2(0, 0));
    setAngularVelocity(0);

    if (clearChar)
        setChar(' ');
}

byte DisplayChar::getChar() const
{
    return c;
}

vec2 DisplayChar::getPos() const
{
    return pos;
}

vec2 DisplayChar::getScale() const
{
    return scale;
}

float DisplayChar::getRotation() const
{
    return rotation;
}

Color DisplayChar::getColor() const
{
    return color;
}

float DisplayChar::getShaking() const
{
    return shaking;
}

vec2 DisplayChar::getVelocity() const
{
    return velocity;
}

vec2 DisplayChar::getAcceleration() const
{
    return acceleration;
}

float DisplayChar::getAngularVelocity() const
{
    return angularVelocity;
}

float DisplayChar::getRainowSpeed() const
{
    return rainbowVelocity;
}    

void DisplayChar::setChar(byte c)
{
    if (this->c == c)
        return;
    this->c = c;
    vaoChanged = true;
}

void DisplayChar::setPos(vec2 pos)
{
    if (this->pos == pos)
        return;
    this->pos = pos;
    vaoChanged = true;
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

void DisplayChar::setRotation(float rotation)
{
    if (this->rotation == rotation)
        return;
    this->rotation = rotation;
    vaoChanged = true;
}

void DisplayChar::setColor(Color color)
{
    if (this->color == color)
        return;
    this->color = color;
    vaoChanged = true;
}

void DisplayChar::setShaking(float shaking)
{
    this->shaking = shaking;
    vaoChanged = true;
}

void DisplayChar::setVelocity(vec2 velocity)
{
    this->velocity = velocity;
}

void DisplayChar::setAcceleration(vec2 acceleration)
{
    this->acceleration = acceleration;
}

void DisplayChar::setAngularVelocity(float angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

void DisplayChar::setRainbowVelocity(float rainbowVelocity)
{
    this->rainbowVelocity = rainbowVelocity;
}

bool DisplayChar::isMoving() const
{
    return velocity != vec2(0, 0) || acceleration != vec2(0, 0);
}

float DisplayChar::maxRadius()const
{
    vec2 v = vec2(1.0f, pixelAspect) * scale;
    return baseScale * sqrt((float)v.x * v.x + v.y * v.y);
}
