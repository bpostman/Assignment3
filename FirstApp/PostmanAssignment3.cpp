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

	//Rectangle
	vec4(-0.95, 0.35, 0.0, 1.0),	//13
	vec4(-0.95, 0.15, 0.0, 1.0),	//14
	vec4(-0.65, 0.15, 0.0, 1.0),	//15
	vec4(-0.65, 0.35, 0.0, 1.0),	//16
	
	//Selection rectangle
	vec4(0.8, 0.9, 0.0, 1.0),		//17
	vec4(0.8, -0.9, 0.0, 1.0),		//18
	vec4(0.9, -0.9, 0.0, 1.0),		//19
	vec4(0.9, 0.9, 0.0, 1.0),		//20

	//Free Draw
	vec4(-0.68, -0.77, 0.0, 1.0),	//21
	vec4(-0.75, -0.68, 0.0, 1.0),	//22
	vec4(-0.82, -0.79, 0.0, 1.0),	//23
	vec4(-0.68, -0.85, 0.0, 1.0),	//24
	vec4(-0.88, -0.90, 0.0, 1.0),	//25


	//Circle created in init function
};

//Order of vertices, used for statically created objects
GLuint elements[] = {
	
	//Sidebar frame
	0, 1, 2, 3, 0,
	1, 4, 5, 2, 1,
	4, 6, 7, 5, 4,
	6, 8, 9, 7, 6,

	//Selection Rectangle
	17, 18, 19, 20, 17,

	//Triangle
	10, 11, 12,

	//Rectangle
	13, 14, 15, 16, 13,

	//Free Draw
	21, 22, 23, 24, 25

};

//IDs for menus and selections
static int operationMenu;
static int colorMenu;
static int menuID;
static int operation;
static int shape = -1;
static float inputColor[4];

//Stores and tracks user input
static int clickCount = 0;
vec4 tVertices[3];
vec4 rVertices[5];	//5 to make the loop
vec4 cVertices[2];	//Holds the center and point on circumference
vec4 fVertices[5];

//Some global variabls
static GLuint staticShaders;
static GLuint dynamicShaders;
static GLuint staticColor;
static GLuint dynamicColor;
enum shapes { RECTANGLE, TRIANGLE, CIRCLE, FREE };
enum inputs { LIGHT_GREEN, DARK_GREEN, YELLOW, WHITE, BLACK, TRANSLATE, ROTATE, SCALE };

//Vertex counts and arrays for dynamically created objects
static const int numPoints = 5000;
vec4 circleVertices[numPoints];
vec4 freeVertices[numPoints];

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

	//Create and store points for the circle on the sidebar
	float theta = 6.18 / numPoints;
	for (int i = 0; i < numPoints; i++) {
		float x = -0.8 + 0.15*cosf(theta*i);
		float y = -0.25 + 0.15*sinf(theta*i);
		circleVertices[i] = vec4(x, y, 0, 1);
	}

	//Store the vertex data in the vbo
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices) + sizeof(freeVertices) + sizeof(rVertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(staticVertices), staticVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices), sizeof(circleVertices), circleVertices);

	//Store the elements in the ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Load shaders and store position and color for static objects
	staticShaders = InitShader("vStaticShader.glsl", "fStaticShader.glsl");
	staticColor = glGetUniformLocation(staticShaders, "vStaticColor");
	GLuint staticPosition = glGetAttribLocation(staticShaders, "vStaticPosition");
	glVertexAttribPointer(staticPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(staticPosition);
	
	// Load shaders and store position and color for dynamic objects
	dynamicShaders = InitShader("vDynamicShader.glsl", "fDynamicShader.glsl");
	dynamicColor = glGetUniformLocation(dynamicShaders, "vDynamicColor");
	GLuint dynamicPosition = glGetAttribLocation(dynamicShaders, "vDynamicPosition");
	glVertexAttribPointer(dynamicPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(dynamicPosition);
}

/*---------------------------------------------------------------*/

//Callback function used by glut to display objects to the user
void display(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Clear the display
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw static objects
	glUseProgram(staticShaders);
	glDrawElements(GL_LINE_STRIP, 20, GL_UNSIGNED_INT, 0);	//Sidebar frame
	glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indexOffset(20));		//Selection Rectangle
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indexOffset(25));		//Triangle
	glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indexOffset(28));		//Rectangle
	glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, indexOffset(33));		//Free Draw
	glDrawArrays(GL_POINTS, sizeof(staticVertices) / sizeof(vec4), numPoints);
	glFlush();

	//Draw dynamic objects
	glUseProgram(dynamicShaders);
	int first = (sizeof(staticVertices) + sizeof(circleVertices)) / sizeof(vec4);
	if (shape == TRIANGLE)
		glDrawArrays(GL_TRIANGLES, first, 3);
	else if (shape == RECTANGLE)
		glDrawArrays(GL_LINE_STRIP, first, 5);
	else if (shape == CIRCLE)
		glDrawArrays(GL_POINTS, first, numPoints);
	glutSwapBuffers();
}

/*---------------------------------------------------------------*/

//Converts window coordinates to object coordinates
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

/*---------------------------------------------------------------*/

void drawTriangle(float x, float y) {
	if (clickCount == 1) {
		tVertices[0] = vec4(x, y, 0, 1);
	}
	else if (clickCount == 2) {
		tVertices[1] = vec4(x, y, 0, 1);
	}
	
	else if (clickCount == 3) {
		tVertices[2] = vec4(x, y, 0, 1);
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(tVertices), tVertices);
		glutPostRedisplay();
		
		//Reset data
		clickCount = 0;
		tVertices[0] = NULL;
		tVertices[1] = NULL;
		tVertices[2] = NULL;
	}
}

/*---------------------------------------------------------------*/

void drawRectangle(float x, float y) {
	if (clickCount == 1) {
		//Clear out data from old shape
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(rVertices), NULL);
		//glutPostRedisplay();
		rVertices[0] = vec4(x, y, 0, 1);
	}
	
	else if (clickCount == 2) {
		rVertices[2] = vec4(x, y, 0, 1);
		rVertices[1] = vec4(rVertices[0].x, rVertices[2].y, 0, 1);
		rVertices[3] = vec4(rVertices[2].x, rVertices[0].y, 0, 1);
		rVertices[4] = vec4(rVertices[0]);
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(rVertices), rVertices);
		glutPostRedisplay();
		
		//Reset data
		clickCount = 0;
		rVertices[0] = NULL;
		rVertices[1] = NULL;
		rVertices[2] = NULL;
		rVertices[3] = NULL;
		rVertices[5] = NULL;
	}
}

/*---------------------------------------------------------------*/

void drawCircle(float x, float y) {

	if (clickCount == 1) {
		cVertices[0] = vec4(x, y, 0, 1);
	}
	else if (clickCount ==2) {
		cVertices[1] = vec4(x, y, 0, 1);
		vec4 points[numPoints];
		
		//Inputs used to draw circle
		float radius = sqrt(pow(cVertices[1].x - cVertices[0].x, 2) + pow(cVertices[1].y - cVertices[0].y, 2));
		float theta = 6.18 / numPoints;

		//Calculate points for circle
		for (int i = 0; i < numPoints; i++) {
			float x = cVertices[0].x + radius*cosf(theta*i);
			float y = cVertices[0].y + radius*sinf(theta*i);

			//Don't draw point if it goes into sidebar or selection rectangle
			if (x > -0.6 && x < 0.8) {
				points[i] = vec4(x, y, 0, 1);
			}
		}
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(points), points);
		glutPostRedisplay();

		//Reset data
		clickCount = 0;
		cVertices[0] = NULL;
		cVertices[1] = NULL;
	}
}

/*---------------------------------------------------------------*/

void drawFree(float x, float y) {
	//FUCK FUCK FUCK SHIT SHIT
}

/*---------------------------------------------------------------*/



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

void mouseDrag(int x1, int y1) {
	vec2 temp = getPosition(x1, y1);
	float x = temp.x;
	float y = temp.y;
	
	//If free draw is selected, go right to drawFree()
	if (shape == FREE) {
		drawFree(x, y);
	}
	
	//Circle has no drag functionality
	else if (shape == CIRCLE) {
		return;
	}

	//If it's the first click, revert to normal behavior
	if (clickCount == 0) {
		return;
	}

	else if (clickCount == 1) {
		//If only the second click and TRIANGLE is selected, revert to normal behavior
		if (shape == TRIANGLE) {
			return;
		}

		//If it's the second click for the rectangle, use drag functionality
		else if (shape == RECTANGLE) {
			rVertices[2] = vec4(x, y, 0, 1);
			rVertices[1] = vec4(rVertices[0].x, rVertices[2].y, 0, 1);
			rVertices[3] = vec4(rVertices[2].x, rVertices[0].y, 0, 1);
			rVertices[4] = vec4(rVertices[0]);
			
			//Store data in VBO and post a redisplay
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(rVertices), rVertices);
			glutPostRedisplay();
		}
	}

		//If it's the third click and TRIANGLE is selected, use drag functionality
		else if (clickCount == 2 && shape == TRIANGLE) {
			tVertices[2] = vec4(x, y, 0, 1);

			//Store data in VBO and post a redisplay
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), sizeof(tVertices), tVertices);
			glutPostRedisplay();
		}
	
}

/*---------------------------------------------------------------*/


void mouse(int button, int state, int x1, int y1) {
	//Calculate and store world-view coordinates
	vec2 temp = getPosition(x1, y1);
	float x = temp.x;
	float y = temp.y;
	if (clickCount == 0) {
		//Clear out data from old shape
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(staticVertices) + sizeof(circleVertices), 5 * sizeof(vec4), '\0' );
		glutPostRedisplay();
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		//Check if click was on the shapes menu, then select correct shape
		if (x < -0.6) {
			if (y > 0.5) {
				clickCount = 0;
				shape = TRIANGLE;
			}
			else if (y > 0.0) {
				clickCount = 0;
				shape = RECTANGLE;
			}
			else if (y > -0.5) {
				clickCount = 0;
				shape = CIRCLE;
			}
			else if (y > -1.0) {
				clickCount = 0;
				shape = FREE;
			}
		}

		//If click was outside of shapes menu, incremenet clickCount and go to appropriate drawing function for currently selected shape
		else {
			switch (shape) {
			case TRIANGLE:
				clickCount++;
				drawTriangle(x, y);
				break;
			case RECTANGLE:
				clickCount++;
				drawRectangle(x, y);
				break;
			case CIRCLE:
				clickCount++;
				drawCircle(x, y);
				break;
			case FREE:
				clickCount++;
				drawFree(x, y);
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

	//Set uniform color for static objects based on menu input, send to shaders
	glUseProgram(staticShaders);
	glUniform4f(staticColor, inputColor[0], inputColor[1], inputColor[2], inputColor[3]);
	
	//Set uniform color for dynamic objects based on menu input, send to shaders
	glUseProgram(dynamicShaders);
	glUniform4f(dynamicColor, inputColor[0], inputColor[1], inputColor[2], inputColor[3]);
	
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
	glutAddSubMenu("Operation", operationMenu);
	glutAddSubMenu("Color", colorMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*---------------------------------------------------------------*/


int main( int argc, char **argv ) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA |GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    
	glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Ben Postman - Assignment 3");
	glewInit(); 

    init();
	initMenu();

	//Handler functions
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);
	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

	glutMainLoop();
    
	return 0;
}
