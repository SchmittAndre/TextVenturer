#version 420

uniform float aspect;

in vec2 vpos;
in vec4 vcolor;

out vec4 fcolor;

void main()
{
  fcolor = vcolor;
  
  gl_Position = vec4(vpos.x / aspect, vpos.y, 0, 1);
}