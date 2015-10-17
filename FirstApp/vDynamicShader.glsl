#version 150

in vec4 vDynamicPosition;
out vec4 color;
uniform vec4 vDynamicColor;

void main()
{
    gl_Position = vDynamicPosition;
	color = vDynamicColor;

}