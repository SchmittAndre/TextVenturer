#pragma once

#include <vld.h>

#include <Windows.h>
#include <string.h>

#include <glew.h>
#include <wglew.h>

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream> 

#include <atlbase.h>
#include <atlconv.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define PI M_PI

using namespace std;

#include "ErrorDialog.h"

#include "vec2.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Color.h"

#include "lodePNG.h"
#include "GLEnums.h"
#include "Shader.h"
#include "VAO.h"
#include "TextureData.h"
#include "Texture.h"
#include "BMPFont.h"