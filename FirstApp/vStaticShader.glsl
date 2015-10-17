#version 150

in vec4 vStaticPosition;
out vec4 color;
uniform vec4 vStaticColor;

void main()
{
    gl_Position = vStaticPosition;
	color = vStaticColor;
}