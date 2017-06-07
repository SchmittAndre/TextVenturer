#include "stdafx.h"

#include "DisplayChar.h"
                      
const ivec2 DisplayChar::pixelSize = ivec2(11, 16);
const float DisplayChar::pixelAspect = (float)DisplayChar::pixelSize.x / DisplayChar::pixelSize.y;
                         
void DisplayChar::ShakeData::reset()
{
    posOffset = vec2(0, 0);
    rotationOffset = 0;
    scaleOffset = vec2(0, 0);
}

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
    vao.setVertices(vaoOffset, 6, data);
}

void DisplayChar::getData(Data (&data)[6])
{
    vec2 p = pos + shakeDataVisible.posOffset;
    float r = rotation + shakeDataVisible.rotationOffset;
    vec2 s = scale + shakeDataVisible.scaleOffset;      

    vec2 right = (s * vec2(2, 0) * font.getWidth(c) * baseScale).rotate(r);
    vec2 up = (s * vec2(0, 1) * baseScale).rotate(r);

    float w = font.getWidth(c) * 2;

    static const vec2 quadSide[] = {
        vec2(0, 0),
        vec2(1, 0),
        vec2(1, 1),
        vec2(1, 1),
        vec2(0, 1),
        vec2(0, 0)
    };

    vec2 lb = font.getTexCoord(c, vec2(0, 0)) + vec2(font.getPixelWidth(), font.getPixelWidth()) / 2;
    vec2 hb = font.getTexCoord(c, vec2(w, 1)) - vec2(font.getPixelWidth(), font.getPixelWidth()) / 2;
    for (byte s = 0; s < 6; s++)
    {
        vec2 middleQuadSide = quadSide[s] * 2 - 1;
        data[s].pos = p + right * middleQuadSide.x + up * middleQuadSide.y;
        data[s].color = color;
        data[s].texcoord = font.getTexCoord(c, vec2(quadSide[s].x * w, quadSide[s].y));
        data[s].borderlow = lb;
        data[s].borderhigh = hb;
    }
}

DisplayChar::DisplayChar(VAO & vao, BMPFont & font, int vaoOffset, vec2 defaultPos, float baseScale, float aspect)
    : vao(vao)
    , font(font)
    , vaoOffset(vaoOffset)
    , defaultPos(defaultPos)
    , baseScale(baseScale)
    , aspect(aspect)
    , vaoChanged(true)
    , c(' ')
    , dataOnly(false)
    , shakeTimeout(0)
{
    shakeDataNew.reset();
    reset();
}

DisplayChar::DisplayChar(const DisplayChar & other)
    : vao(other.vao)
    , font(other.font)
    , dataOnly(true)
{
    *this = other;
}

DisplayChar::DisplayChar(DisplayChar && other)  noexcept
    : vao(other.vao)
    , font(other.font)
    , aspect(other.aspect)
    , dataOnly(other.dataOnly)
    , vaoOffset(other.vaoOffset)
    , baseScale(other.baseScale)
    , defaultPos(other.defaultPos)
    , shakeTimeout(other.shakeTimeout)
    , shakeDataOld(other.shakeDataOld)
    , shakeDataNew(other.shakeDataNew)
    , shakeDataVisible(other.shakeDataVisible)
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

            auto random = [](float lo, float hi) { return (float)rand() / RAND_MAX * (hi - lo) + lo; };

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
    if (dataOnly)
        throw(EDisplayCharDataOnly);

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
    vao.addVertices(6, data);
}

void DisplayChar::reset(bool clearChar)
{
    setPos(defaultPos);
    setScale(vec2(1, 1));
    setRotation(0);
    setColor(Color(1, 1, 1));

    setShaking(0);
    setAcceleration(vec2(0, 0));
    setVelocity(vec2(0, 0));
    setAngularVelocity(0);
    setRainbowVelocity(0);

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
    if (this->shaking == shaking)
        return;
    if (this->shaking == 0)
    {
        shakeTimeout = 0;
        shakeDataNew.reset();
    }
    this->shaking = shaking;
    if (shaking == 0)
        shakeDataVisible.reset();
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

EDisplayCharDataOnly::EDisplayCharDataOnly()
    : Exception("Display char is not renderable")
{
}
