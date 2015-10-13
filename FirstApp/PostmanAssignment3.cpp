#include "Angel.h"
#include <iostream>
using namespace std;

#define indexOffset(index) (void*)(index*sizeof(GLuint))

//Vertices for objects
vec4 vertices[] = {
	
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
};

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
/*
vec4 sideBarShapes[] = {
	//Triangle
	vec4(-0.8, 0.85, 0.0, 1.0),
	vec4(-0.95, 0.65, 0.0, 1.0),
	vec4(-0.65, 0.65, 0.0, 1.0),

	//Square
	vec4(-0.95, 0.35, 0.0, 1.0),
	vec4(-0.95, 0.15, 0.0, 1.0),
	vec4(-0.65, 0.15, 0.0, 1.0),
	vec4(-0.65, 0.35, 0.0, 1.0),
	vec4(-0.95, 0.35, 0.0, 1.0)
};
*/
//Counts of vertices for both names, used throughout program
static int elementsCount = sizeof(elements) / sizeof(GLuint);
static int operationMenu = 0;
static int colorMenu = 0;
static int menuID = 0;
static int operation = 0;
static float inputColor[4];
static GLuint shaders = 0;
static GLuint color = 0;

enum shapes { RECTANGLE, TRIANGLE, CIRCLE, FREE };
enum inputs { LIGHT_GREEN, DARK_GREEN, YELLOW, WHITE, BLACK, TRANSLATE, ROTATE, SCALE};

/*---------------------------------------------------------------*/


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
	//glGenBuffers(1, &ebo);
	
	//Store the vertex data in the vbo
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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


int main( int argc, char **argv )
{

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 512, 512 );
    
	glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Ben Postman - Assignment 3" );
	glewInit(); 

    init();
	initMenu();

	glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutMainLoop();
    
	return 0;
}