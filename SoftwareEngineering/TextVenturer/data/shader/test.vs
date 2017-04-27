#version 140

uniform float aspect;
uniform float scale;

in vec2 vpos;
in vec2 vtexcoord;
in vec4 vcolor;
in vec2 vborderlow;
in vec2 vborderhigh;

out vec2 ftexcoord;
out vec4 fcolor;
flat out vec2 fborderlow;
flat out vec2 fborderhigh;

void main()
{
  fcolor = vcolor;
  ftexcoord = vtexcoord;
  fborderlow = vborderlow;
  fborderhigh = vborderhigh;
  
  gl_Position = vec4(vpos.x / aspect * scale, vpos.y * scale, 0, 1);
}