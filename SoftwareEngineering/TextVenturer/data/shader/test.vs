#version 140

uniform float aspect;

in vec2 vpos;
in vec2 vtexcoord;
in vec4 vcolor;

out vec2 ftexcoord;
out vec4 fcolor;

void main()
{
  fcolor = vcolor;
  ftexcoord = vtexcoord;
  
  gl_Position = vec4(vpos.x / aspect, vpos.y, 0, 1);
}