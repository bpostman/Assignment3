#version 150

in vec4 vDynamicPosition;
out vec4 color;
uniform vec4 vDynamicColor;
uniform mat4 modelView = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
	gl_Position = projectionMatrix * modelView * vDynamicPosition;
	color = vDynamicColor;

}