#pragma once

#include <vld.h>
#include <vector>
#include <Windows.h>

#include <glew.h>
#include <wglew.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream> 
#include <unordered_map>
#include <unordered_set>
#include <regex>

#include <atlbase.h>
#include <atlconv.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define PI M_PI

using namespace std;

typedef unordered_map<string, string> dictionary;
typedef unordered_set<string> tags;
typedef vector<string> strings;

#include "ErrorDialog.h"
#include "AliasList.h"

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
