#include "stdafx.h"

#include "DisplayChar.h"
                      
const ivec2 DisplayChar::pixelSize = ivec2(11, 16);
const float DisplayChar::pixelAspect = (float)DisplayChar::pixelSize.x / DisplayChar::pixelSize.y;
                         
DisplayChar::ShakeData DisplayChar::ShakeData::operator*(float factor) const
{
    ShakeData result;

    result.posOffset = posOffset * factor;
    result.rotationOffset = rotationOffset * factor;
    result.scaleOffset = scaleOffset * factor;

    return result;
}

DisplayChar::ShakeData DisplayChar::ShakeData::operator+(const ShakeData & other) const
{
    ShakeData result;

    result.posOffset = posOffset + other.posOffset;
    result.rotationOffset = rotationOffset + other.rotationOffset;
    result.scaleOffset = scaleOffset + other.scaleOffset;

    return result;
}

void DisplayChar::updateVAO()
{
    if (!vaoChanged)
        return;

    Data data[6];
    getData(data);   
    
    vao->setVertices(vaoOffset, 6, data);
}

void DisplayChar::getData(Data data[6])
{
    vec2 p = pos + shakeDataVisible.posOffset;
    float r = rotation + shakeDataVisible.rotationOffset;
    vec2 s = scale + shakeDataVisible.scaleOffset;      

    vec2 right = (s * vec2(2, 0) * font->getWidth(c) * baseScale).rotate(r);
    vec2 up = (s * vec2(0, 1) * baseScale).rotate(r);

    float w = font->getWidth(c) * 2;
    data[0].pos = p - right - up;
    data[0].color = color;
    data[0].texcoord = font->getTexCoord(c, vec2(0, 0));
    data[1].pos = p + right - up;
    data[1].color = color;
    data[1].texcoord = font->getTexCoord(c, vec2(w, 0));
    data[2].pos = p + right + up;
    data[2].color = color;
    data[2].texcoord = font->getTexCoord(c, vec2(w, 1));
    data[3].pos = p + right + up;
    data[3].color = color;
    data[3].texcoord = font->getTexCoord(c, vec2(w, 1));
    data[4].pos = p - right + up;
    data[4].color = color;
    data[4].texcoord = font->getTexCoord(c, vec2(0, 1));
    data[5].pos = p - right - up;
    data[5].color = color;
    data[5].texcoord = font->getTexCoord(c, vec2(0, 0));
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

    shakeDataOld.posOffset = vec2(0, 0);
    shakeDataOld.rotationOffset = 0;
    shakeDataOld.scaleOffset = vec2(0, 0);

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

bool DisplayChar::update(float deltaTime)
{   
    velocity = velocity + acceleration * deltaTime;
    
    float r = maxRadius();
    if (pos.y < -1 - r || pos.y > 1 + r || pos.x < -aspect - r || pos.x > aspect + r)
        reset(true);
    
    setPos(pos + velocity * deltaTime);
    
    setRotation(rotation + angularVelocity * deltaTime);
    
    if (rainbowVelocity != 0)
        setColor(color.addRainbow(rainbowVelocity * deltaTime));

    if (shaking != 0)
    {
        shakeTimeout -= deltaTime * shaking * 12.5f;
        if (shakeTimeout <= 0)
        {
            shakeTimeout = 1;
            shakeDataOld = shakeDataNew;

            static const auto random = [](float lo, float hi) -> float { return (float)rand() / RAND_MAX * (hi - lo) + lo; };

            shakeDataNew.posOffset = vec2(random(-scale.x, +scale.x), random(-scale.y, +scale.y)) * baseScale * 0.1f;
            shakeDataNew.rotationOffset = random(-10, +10);
            static const float factor = 0.15f;
            static const vec2 limit(-factor, factor / (1.0f - factor));
            shakeDataNew.scaleOffset = vec2(random(limit.x, limit.y), random(limit.x, limit.y));
        }               

        shakeDataVisible = shakeDataOld * shakeTimeout + shakeDataNew * (1 - shakeTimeout);
        vaoChanged = true;
    }

    return vaoChanged;
}

void DisplayChar::render(bool useSubData)
{
    if (useSubData)
        updateVAO();
    else
        addToVAO();
    vaoChanged = false;
}

void DisplayChar::addToVAO()
{
    Data data[6];
    getData(data);
    vao->addVertices(6, data);
}

void DisplayChar::reset(bool clearChar)
{
    setPos(defaultPos);
    setScale(vec2(1, 1));
    setRotation(0);

    setShaking(0);
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

vec2 DisplayChar::getDefaultPos() const
{
    return defaultPos;
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
    if (shaking == 0)
    {
        shakeDataVisible.posOffset = vec2(0, 0);
        shakeDataVisible.rotationOffset = 0;
        shakeDataVisible.scaleOffset = vec2(0, 0);
    }
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
