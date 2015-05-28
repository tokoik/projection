#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2DRect image;

layout (location = 0) out vec4 fc;

void main()
{
  fc = texture(image, gl_FragCoord.xy);
}
