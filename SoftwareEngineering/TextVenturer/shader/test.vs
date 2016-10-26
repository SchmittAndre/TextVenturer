#version 420

in vec2 vpos;
in vec4 vcolor;

out vec4 fcolor;

void main()
{
  fcolor = vcolor;
  
  gl_Position = vec4(vpos, 0, 1);
}