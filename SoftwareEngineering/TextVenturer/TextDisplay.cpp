#include "stdafx.h"   

#include "DisplayChar.h"
#include "AsciiArt.h"

#include "TextDisplay.h"

const float TextDisplay::State::DefaultDelay = 0.02f;

TextDisplay::State::State()
{
    reset();
}

void TextDisplay::State::reset()
{
    offset = vec2(0, 0);
    scale = vec2(1, 1);
    rotation = 0;
    color = Color();
    shaking = 0;

    velocity = vec2(0, 0);
    acceleration = vec2(0, 0);
    angularVelocity = 0;
    rainbowVelocity = 0;

    rainbow = 0;
    offsetMovement = vec2(0, 0);
    delay = DefaultDelay;
}

void TextDisplay::State::processCommand(const std::string & command, const std::vector<float>& params)
{
    auto paramerror = [command, params]()
    {
        ErrorDialog("Parse Error", "Command \"" + command + "\" does not take \"" + std::to_string(params.size()) + "\" parameters!");
    };

    if (command == "offset")
    {
        if (params.size() == 0)
        {
            offset = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            offset = vec2(params[0], params[1]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "coffset")
    {
        if (params.size() == 0)
        {
            offset = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            offset = vec2(params[0] * DisplayChar::pixelAspect, params[1]) * 2;
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "scale")
    {
        if (params.size() == 0)
        {
            scale = vec2(1, 1);
        }
        else if (params.size() == 1)
        {
            scale = vec2(params[0], params[0]);
        }
        else if (params.size() == 2)
        {
            scale = vec2(params[0], params[1]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "rotation")
    {
        if (params.size() == 0)
        {
            rotation = 0;
        }
        else if (params.size() == 1)
        {
            rotation = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "rgb")
    {
        if (params.size() == 3)
        {
            color = Color(params[0], params[1], params[2]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "hsv")
    {
        if (params.size() == 3)
        {
            color = Color::HSV(params[0], params[1], params[2]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "shaking_on")
    {
        if (params.size() == 0)
        {
            shaking = 1;
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "shaking_off")
    {
        if (params.size() == 0)
        {
            shaking = 0;
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "shaking")
    {
        if (params.size() == 0)
        {
            shaking = 0;
        }
        if (params.size() == 1)
        {
            shaking = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "velocity")
    {
        if (params.size() == 0)
        {
            velocity = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            velocity = vec2(params[0], params[1]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "acceleration")
    {
        if (params.size() == 0)
        {
            acceleration = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            acceleration = vec2(params[0], params[1]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "rotation_speed")
    {
        if (params.size() == 0)
        {
            angularVelocity = 0;
        }
        else if (params.size() == 1)
        {
            angularVelocity = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "rainbow_speed")
    {
        if (params.size() == 0)
        {
            rainbowVelocity = 0;
        }
        else if (params.size() == 1)
        {
            rainbowVelocity = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "rainbow")
    {
        if (params.size() == 0)
        {
            rainbow = 0;
        }
        else if (params.size() == 1)
        {
            rainbow = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "offset_movement")
    {
        if (params.size() == 0)
        {
            offsetMovement = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            offsetMovement = vec2(params[0], params[1]);
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "coffset_movement")
    {
        if (params.size() == 0)
        {
            offsetMovement = vec2(0, 0);
        }
        else if (params.size() == 2)
        {
            offsetMovement = vec2(params[0] * DisplayChar::pixelAspect, params[1]) * 2;
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "delay")
    {
        if (params.size() == 0)
        {
            delay = DefaultDelay;
        }
        else if (params.size() == 1)
        {
            delay = params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "sleep")
    {
        if (params.size() == 1)
        {
            time += params[0];
        }
        else
        {
            paramerror();
        }
    }
    else if (command == "reset")
    {
        if (params.size() == 0)
        {
            reset();
        }
        else
        {
            paramerror();
        }
    }
    else
    {
        for (std::pair<std::string, Color> pair : Color::dictionary)
        {
            if (command == pair.first)
            {
                if (params.size() == 0)
                {
                    color = pair.second;
                }
                else
                {
                    paramerror();
                }
                return;
            }
        }

        ErrorDialog("Parse Error", "Unknown command: \"" + command + "\"!");
    }
}

void TextDisplay::State::nextChar()
{
    color = color.addRainbow(rainbow);
    offset += offsetMovement;
    time += delay;
}

TextDisplay::TextDisplay(Shader* textShader, BMPFont* font, UINT width, UINT height, float aspect)
{
    vao = new VAO(textShader);
    
    vao->generate((width * height + 1) * 6, buStreamDraw);
    vao->forceMaxSize();

    this->font = font;
    font->uniform(textShader, "font");

    this->width = width;
    this->height = height;
    text = new DisplayChar**[width];
    vec2 pos;
    float scale = 1.0f / height;
    for (UINT x = 0; x < width; x++)
    {
        text[x] = new DisplayChar*[height];
        for (UINT y = 0; y < height; y++)
        {
            pos = getCharPos(ivec2(x, y));
            text[x][y] = new DisplayChar(vao, font, (x * height + y) * 6, pos, scale, aspect);
        }
    }

    cursorChar = new DisplayChar(vao, font, width * height * 6, vec2(0, 0), scale, aspect);

    cursorVisible = true;

    subDataMaxChanges = 420;
}

TextDisplay::~TextDisplay()
{
    delete vao;
    for (size_t x = 0; x < width; x++)
    {
        for (size_t y = 0; y < height; y++)
            delete text[x][y];
        delete[] text[x];
    }
    delete[] text;
    delete cursorChar;
}

vec2 TextDisplay::getCharPos(ivec2 pos) const
{
    vec2 result;
    result.x = ((2.0f * pos.x - width + 1) / height) * DisplayChar::pixelAspect;
    result.y = 1 - (2.0f * pos.y + 1) / height;   
    return result;
}

void TextDisplay::write(int x, int y, const std::string & str)
{
    for (UINT p = 0; p < (UINT)str.length(); p++)
        write(x + p, y, str[p]);
}

void TextDisplay::write(int y, const std::string & str)
{
    write((getWidth() - (UINT)str.size()) / 2, y, str);
}

void TextDisplay::write(ivec2 p, const std::string & str)
{
    write(p.x, p.y, str);
}

void TextDisplay::write(int x, int y, const byte c, const State & state)
{
    if (isVisible(x, y))
    {
        text[x][y]->setChar(c);
        text[x][y]->setPos(text[x][y]->getDefaultPos() + state.offset / (float)height);
        text[x][y]->setScale(state.scale);
        text[x][y]->setRotation(state.rotation);
        text[x][y]->setColor(state.color);
        text[x][y]->setShaking(state.shaking);
        text[x][y]->setVelocity(state.velocity);
        text[x][y]->setAcceleration(state.acceleration);
        text[x][y]->setAngularVelocity(state.angularVelocity);
        text[x][y]->setRainbowVelocity(state.rainbowVelocity);
    }
}

void TextDisplay::write(ivec2 p, const byte c, const State & state)
{
    write(p.x, p.y, c, state);
}

void TextDisplay::writeStep(int & x, int y, std::string & str, State & state)
{
    if (str.size() == 0)
    {
        // std::string empty
        return;
    }

    // std::string not empty
    if (str[0] != '$')
    {
        // not a $
        // write char
        write(x, y, str[0], state);
        str = str.substr(1);
        state.nextChar();
        x++;
        return;
    }

    // a $ at start of line
    if (str.size() == 1)
    {
        // not a $ second but at end of std::string
        // write the $ since it would not fit to the syntax
        write(x, y, str[0], state);
        str = str.substr(1);
        state.nextChar();
        x++;
        return;
    }

    // not last char in std::string
    if (str[1] == '$')
    {
        // after $ is a second $
        // write $ and skip the next                
        write(x, y, str[0], state);
        str = str.substr(2);
        state.nextChar();
        x++;
        return;
    }      
    
    // after $ is not a second $
    // check if $___(___) without spaces matches
    std::smatch matches;
    if (!std::regex_search(str, matches, std::regex("\\$([^$ ]+?)\\(([^ ]*?)\\)"), std::regex_constants::match_continuous))
    {
        // doesn't fit the correct syntax
        // write the $
        write(x, y, str[0], state);
        str = str.substr(1);
        state.nextChar();
        x++;
        return;
    }            

    // fits the correct syntax, start parsing the command
    std::string command = matches[1];
    std::string paramstr = matches[2];
    transform(command.begin(), command.end(), command.begin(), tolower);
    std::vector<float> params;

    bool error = false;

    if (paramstr != "")
    {
        size_t pos, lastpos = 0;
        do
        {
            pos = paramstr.find(',', lastpos);
            float value;
            if (sscanf_s(paramstr.substr(lastpos, pos).c_str(), "%f", &value) == 0)
            {
                ErrorDialog("Parse Error", "Can't scan parameter list \"" + paramstr + "\" for command \"" + command + "\"!");
                error = true;
                break;
            }
            lastpos = pos + 1;
            params.push_back(value);
        } while (pos != std::string::npos);
    }
    if (!error)
    {
        state.processCommand(command, params);
    }

    str = str.substr(matches[0].length());
}

void TextDisplay::writeStep(ivec2 & p, std::string & str, State & state)
{
    return writeStep(p.x, p.y, str, state);
}

void TextDisplay::draw(int x, int y, const AsciiArt & art)
{
    for (UINT line = 0; line < art.getHeight(); line++)
        write(x, y + line, art[line]);
}

void TextDisplay::draw(ivec2 p, const AsciiArt & art)
{
    draw(p.x, p.y, art);
}

void TextDisplay::draw(int y, const AsciiArt & art)
{
    draw((getWidth() - art.getWidth()) / 2, y, art);
}

void TextDisplay::move(ivec2 src, uvec2 size, ivec2 dest)
{
    std::vector<std::vector<DisplayChar>> copy;
    for (UINT x = 0; x < width; x++)
    {
        copy.push_back(std::vector<DisplayChar>());
        for (UINT y = 0; y < height; y++)
        {
            copy[x].push_back(DisplayChar(*text[x][y]));
        }
    }

    for (UINT x = 0; x < size.x; x++)
        for (UINT y = 0; y < size.y; y++)
        {
            ivec2 s(src.x + x, src.y + y);
            ivec2 d(dest.x + x, dest.y + y);

            if (!isVisible(d))
                continue;

            if (!isVisible(s))
                text[d.x][d.y]->reset(true);
            else
                *(text[d.x][d.y]) = copy[s.x][s.y];                
        }
}

void TextDisplay::update(float deltaTime)
{
    cursorTime -= deltaTime;
    if (cursorTime <= 0)
        resetCursorTime();

    if (cursorVisible && cursorTime > 0.5)
        cursorChar->setChar('_');
    else
        cursorChar->setChar(' ');

    UINT changes = 0;

    if (cursorChar->update(deltaTime))
        changes++;

    for (UINT x = 0; x < width; x++)
    {
        for (UINT y = 0; y < height; y++)
        {
            if (text[x][y]->update(deltaTime))
            {
                changes++;
            }
        }
    }
    useSubData = changes < subDataMaxChanges;
}

void TextDisplay::render()
{
    if (!useSubData)
        vao->map(baWriteOnly);

    for (UINT x = 0; x < width; x++)
    {
        for (UINT y = 0; y < height; y++)
        {
            text[x][y]->render(useSubData);
        }
    }
    cursorChar->render(useSubData);

    if (!useSubData)
        vao->unmap();

    vao->render();
}

UINT TextDisplay::getWidth() const
{
    return width;
}

UINT TextDisplay::getHeight() const
{
    return height;
}

void TextDisplay::setCursorVisible(bool visible)
{
    cursorVisible = visible;
}

bool TextDisplay::getCursorVisible() const
{
    return cursorVisible;
}

void TextDisplay::resetCursorTime()
{
    cursorTime = 1;
}

void TextDisplay::setCursorPos(ivec2 pos) 
{
    cursorPos = pos;
    cursorChar->setPos(getCharPos(pos));
}

void TextDisplay::setCursorPos(int x, int y)
{
    setCursorPos(ivec2(x, y));
}

ivec2 TextDisplay::getCursorPos() const
{
    return cursorPos;
}

bool TextDisplay::isColumnVisible(int x) const
{
    return x >= 0 && (UINT)x < width;
}

bool TextDisplay::isLineVisible(int y) const
{
    return y >= 0 && (UINT)y < height;
}

bool TextDisplay::isVisible(int x, int y) const
{
    return isColumnVisible(x) && isLineVisible(y);
}

bool TextDisplay::isVisible(ivec2 p) const
{
    return isVisible(p.x, p.y);
}


std::string TextDisplay::getLine(UINT y, UINT offset, size_t count) const
{
    std::string result;
    UINT end = count == std::string::npos ? width : offset + (UINT)count;
    for (UINT x = offset; x < end; x++)
        result += getChar(x, y);
    return result;
}

DisplayChar * TextDisplay::getDisplayChar(int x, int y) const
{
    return isVisible(x, y) ? text[x][y] : NULL;
}

DisplayChar * TextDisplay::getDisplayChar(uvec2 p) const
{
    return getDisplayChar(p.x, p.y);
}

byte TextDisplay::getChar(int x, int y) const
{
    return isVisible(x, y) ? text[x][y]->getChar() : ' ';
}

byte TextDisplay::getChar(ivec2 p) const
{
    return getChar(p.x, p.y);
}

void TextDisplay::clearLine(int y, UINT offset, size_t count)
{
    if (isLineVisible(y))
    {
        size_t end = count == std::string::npos ? width : offset + count;
        for (size_t x = offset; x < end; x++)
            text[x][y]->reset(true);
    }
}

void TextDisplay::clear()
{
	for (size_t x = 0; x < width; x++)
		for (size_t y = 0; y < height; y++)
			text[x][y]->reset(true);
}
