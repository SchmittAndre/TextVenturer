#include "stdafx.h"

#include "DisplayChar.h"

DisplayChar::DisplayChar(VAO* vao, int vaoOffset)
{
    this->vao = vao;
    this->vaoOffset = vaoOffset;
}

void DisplayChar::update()
{   
    struct {
        vec2 pos;
        Color color;
    } data[6];

    int i = 0;
    data[i].pos = vec2(0, 0);
    data[i++].color = Color(0, 0, 1);
    data[i].pos = vec2(0, 1);
    data[i++].color = Color(0, 1, 1);
    data[i].pos = vec2(1, 1);
    data[i++].color = Color(1, 1, 1);
    data[i].pos = vec2(1, 1);
    data[i++].color = Color(1, 1, 1);
    data[i].pos = vec2(1, 0);
    data[i++].color = Color(1, 0, 1);
    data[i].pos = vec2(0, 0);
    data[i++].color = Color(0, 0, 1);

    vao->setVertices(0, 6, data);
}

// letter = 11x16
// screen = x
// width in pixel = x
