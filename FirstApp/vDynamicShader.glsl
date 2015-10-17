#version 150

in vec4 vDynamicPosition;
out vec4 color;
uniform vec4 vDynamicColor;
uniform mat4 projectionMatrix;

void main()
{
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex
	//printf("%f,%f,%f,%f", projectionMatrix[0][0], projectionMatrix[0][1], projectionMatrix[0][2], projectionMatrix[0][3]);
    gl_Position = vDynamicPosition;// * projectionMatrix;
	color = vDynamicColor;

}