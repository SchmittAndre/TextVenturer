#version 140


uniform sampler2D font;

in vec4 fcolor;
in vec2 ftexcoord;
flat in vec2 fborderlow;
flat in vec2 fborderhigh;

out vec4 outcolor;

void main()
{  
  outcolor = texture(font, clamp(ftexcoord, fborderlow, fborderhigh)) * fcolor;
  if (outcolor.a == 0)
    gl_FragDepth = 1;
  else
    gl_FragDepth = gl_FragCoord.z;
}