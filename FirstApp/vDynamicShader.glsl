#version 150

in vec4 vFirstPosition;
uniform vec4 vFirstColor;
out vec4 firstColor;

void main()
{
    gl_Position = vFirstPosition;
	firstColor = vFirstColor;
}
