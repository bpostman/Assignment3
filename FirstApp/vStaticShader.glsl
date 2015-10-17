#version 150

in vec4 vPosition;
out vec4 color;
uniform vec4 vColor;

void main()
{
    gl_Position = vPosition;
	color = vColor;
	
	//Absolute values of positiom are used to give more colorful results
	//float r = abs(vPosition.x);		//red channel is absolute value of x
	//float g = abs(vPosition.y);		//green channel is absolute value of y
	//float b = abs(vPosition.x)+abs(vPosition.y)/2;		//blue channel is average of absolute values of x & y
	//color = vec4(r, g, b, 1);

}