#version 140


uniform sampler2D font;

in vec4 fcolo;
in vec2 ftexcoord;

out vec4 outcolor;

void main()
{
  outcolor = texture(font, ftexcoord) * fcolo;
  if (outcolor.a == 0)
    gl_FragDepth = 1;
  else
    gl_FragDepth = gl_FragCoord.z;
}