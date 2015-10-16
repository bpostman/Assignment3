#include "Angel.h"
#include <iostream>
using namespace std;

#define indexOffset(index) (void*)(index*sizeof(GLuint))

//Vertices for objects created at compile time
vec4 staticVertices[] = {
	
	//Sidebar frame
	vec4(-1.0, 1.0, 0.0, 1.0),	//0
	vec4(-1.0, 0.5, 0.0, 1.0),	//1
	vec4(-0.6, 0.5, 0.0, 1.0),	//2
	vec4(-0.6, 1.0, 0.0, 1.0),	//3
	vec4(-1.0, 0.0, 0.0, 1.0),	//4
	vec4(-0.6, 0.0, 0.0, 1.0),	//5
	vec4(-1.0, -0.5, 0.0, 1.0),	//6
	vec4(-0.6, -0.5, 0.0, 1.0),	//7
	vec4(-1.0, -1.0, 0.0, 1.0),	//8
	vec4(-0.6, -1.0, 0.0, 1.0),	//9

	//Triangle
	vec4(-0.8, 0.85, 0.0, 1.0),		//10
	vec4(-0.95, 0.65, 0.0, 1.0),	//11
	vec4(-0.65, 0.65, 0.0, 1.0),	//12

	//Square
	vec4(-0.95, 0.35, 0.0, 1.0),	//13
	vec4(-0.95, 0.15, 0.0, 1.0),	//14
	vec4(-0.65, 0.15, 0.0, 1.0),	//15
	vec4(-0.65, 0.35, 0.0, 1.0)		//16

	//Circle and free draw created in init function
};

//Order of vertices, used for statically created objects
GLuint elements[] = {
	
	//Sidebar frame
	0, 1, 2, 3, 0,
	1, 4, 5, 2, 1,
	4, 6, 7, 5, 4,
	6, 8, 9, 7, 6,

	//Triangle
	10, 11, 12,

	//Square
	13, 14, 15, 16, 13
};

//IDs for menus and seections
static int operationMenu;
static int colorMenu;
static int menuID;
static int operation;
static int shape;
static float inputColor[4];

static GLuint shaders;
static GLuint color;

//Vertex counts and arrays for dynamically created objects
static const int circlePoints = 5000;
vec4 circleVertices[circlePoints];
static const int freePoints = 1000;
vec4 freeVertices[freePoints];

enum shapes { RECTANGLE, TRIANGLE, CIRCLE, FREE };
enum inputs { LIGHT_GREEN, DARK_GREEN, YELLOW, WHITE, BLACK, TRANSLATE, ROTATE, SCALE};

/*---------------------------------------------------------------*/
vec2 getPosition(int x1, int y1) {
	float xf = (float)x1;
	float yf = (float)y1;
	float x, y;
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	float halfWidth = (float)width / 2;
	float halfHeight = (float)height / 2;

	if (x1 < halfWidth)
		x = xf / halfWidth - 1;
	else if (x1 > halfWidth)
		x = (xf - halfWidth) / halfWidth;
	else
		x = 0;


	if (y1 < halfHeight)
		y = -(yf - halfHeight) / halfHeight;
	else if (y1 > halfHeight)
		y = yf / -halfHeight + 1;
	else
		y = 0;

	return vec2(x, y);

}

/*------------------------------------------------------------------*/


//Initialize necessary componenets and send data to the GPU
void init() {

	//Set background color to grey
	glClearColor(0.5, 0.5, 0.5, 1.0);

	// Create and bind a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate vertex buffer object and element buffer object
	GLuint buffers[2];
	glGenBuffers(2, buffers);

	//Create and store points for the circle on the sidebar
	float theta = 6.18 / circlePoints;
	for (int i = 0; i < circlePoints; i++) {
		float x = -0.8+0.15*cosf(theta*i);
		float y = -0.25+0.15*sinf(theta*i);
		circleVertices[i] = vec4(x, y, 0, 1);
	}
	
	//Store the vertex data in the vbo
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(staticVertices), staticVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices) + sizeof(freeVertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(staticVertices), staticVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices), sizeof(circleVertices), circleVertices);

	//Store the elements in the ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Load shaders
	shaders = InitShader("vShader.glsl", "fShader.glsl");
	color = glGetUniformLocation(shaders, "vColor");

	//Create location for first name position
	GLuint position = glGetAttribLocation(shaders, "vPosition");
	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(position);
	glUseProgram(shaders);
}

/*---------------------------------------------------------------*/

void drawRectangle() {

}

/*---------------------------------------------------------------*/

void drawTriangle() {

}

/*---------------------------------------------------------------*/

void drawCircle() {

}

/*---------------------------------------------------------------*/

void drawFree() {

}

/*---------------------------------------------------------------*/

//Callback function used by glut to display objects to the user
void display( void )
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Clear the display
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw first name
	glDrawElements(GL_LINE_STRIP, 20, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indexOffset(20));
	glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indexOffset(23));
	glDrawArrays(GL_POINTS, sizeof(staticVertices) / sizeof(vec4), circlePoints);
	glFlush();
}

/*---------------------------------------------------------------*/

//Handle keyboard input and redraw accordingly
void keyboard( unsigned char key, int x, int y )
{
	//Check which key is pressed and either exit or set shadingChoice accordingly
    switch ( key ) {
    case 27:	//ESC
		exit( EXIT_SUCCESS );
        break;
	default:
		break;
    }
}

/*---------------------------------------------------------------*/

void mouse(int button, int state, int x1, int y1) {

	//Calculate and store world-view coordinates
	vec2 temp = getPosition(x1, y1);
	float x = temp.x;
	float y = temp.y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//Check if click was on the shapes menu, then select correct shape
		if (x < -0.6) {
			if (y > 0.5)
				shape = RECTANGLE;
			else if (y > 0.0)
				shape = TRIANGLE;
			else if (y > -0.5)
				shape = CIRCLE;
			else if (y > -1.0)
				shape = FREE;
			else
				shape = -1;
		}

		//If click was outside of shapes menu, go to appropriate drawing function for currently selected shape
		else {
			switch (shape) {
			case RECTANGLE:
				drawRectangle();
				break;
			case TRIANGLE:
				drawTriangle();
				break;
			case CIRCLE:
				drawCircle();
				break;
			case FREE:
				drawFree();
				break;
			default:
				break;
			}
		}
	}
}

/*---------------------------------------------------------------*/
void menu(int choice) {

	switch (choice) {
	case TRANSLATE:
		operation = TRANSLATE;
		break;
	case ROTATE:
		operation = ROTATE;
		break;
	case SCALE:
		operation = SCALE;
		break;
	case LIGHT_GREEN:
		inputColor[0] = 0.0; inputColor[1] = 0.5; inputColor[2] = 0.0; inputColor[3] = 1.0;
		break;
	case DARK_GREEN:
		inputColor[0] = 0.0; inputColor[1] = 0.3; inputColor[2] = 0.0; inputColor[3] = 1.0;
		break;
	case YELLOW:
		inputColor[0] = 0.8; inputColor[1] = 0.8; inputColor[2] = 0.0; inputColor[3] = 1.0;
		break;
	case WHITE:
		inputColor[0] = 1.0; inputColor[1] = 1.0; inputColor[2] = 1.0; inputColor[3] = 1.0;
		break;
	case BLACK:
		inputColor[0] = 0.0; inputColor[1] = 0.0; inputColor[2] = 0.0; inputColor[3] = 1.0;
		break;
	default:
		break;
	}

	//Set uniform color based on menu input, send to shaders, post a redisplay
	glUniform4f(color, inputColor[0], inputColor[1], inputColor[2], inputColor[3]);
	glutPostRedisplay();

}


/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/

void initMenu() {
	
	//Set up menu for shape selection
	operationMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Translate", TRANSLATE);
	glutAddMenuEntry("Rotate", ROTATE);
	glutAddMenuEntry("Scale", SCALE);
	
	colorMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Light Green", LIGHT_GREEN);
	glutAddMenuEntry("Dark Green", DARK_GREEN);
	glutAddMenuEntry("Yellow", YELLOW);
	glutAddMenuEntry("White", WHITE);
	glutAddMenuEntry("Black", BLACK);
	
	glutCreateMenu(menu);
	glutAddSubMenu("Shapes", operationMenu);
	glutAddSubMenu("Colors", colorMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*---------------------------------------------------------------*/


int main( int argc, char **argv ) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(512, 512);
    
	glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Ben Postman - Assignment 3");
	glewInit(); 

    init();
	initMenu();

	//Handler functions
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

	glutMainLoop();
    
	return 0;
}