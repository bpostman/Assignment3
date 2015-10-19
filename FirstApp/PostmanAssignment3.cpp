#include "Angel.h"

//Macro for finding offset used in elements aray
#define indexOffset(index) (void*)(index*sizeof(GLuint))

//Vertices for objects created at compile time
vec4 staticVertices[] = {
	//Sidebar frame
	vec4(-1.0f, 1.0f, 0.0f, 1.0f),	//0
	vec4(-1.0f, 0.5f, 0.0f, 1.0f),	//1
	vec4(-0.6f, 0.5f, 0.0f, 1.0f),	//2
	vec4(-0.6f, 1.0f, 0.0f, 1.0f),	//3
	vec4(-1.0f, 0.0f, 0.0f, 1.0f),	//4
	vec4(-0.6f, 0.0f, 0.0f, 1.0f),	//5
	vec4(-1.0f, -0.5f, 0.0f, 1.0f),	//6
	vec4(-0.6f, -0.5f, 0.0f, 1.0f),	//7
	vec4(-1.0f, -1.0f, 0.0f, 1.0f),	//8
	vec4(-0.6f, -1.0f, 0.0f, 1.0f),	//9

	//Triangle
	vec4(-0.8f, 0.85f, 0.0f, 1.0f),		//10
	vec4(-0.95f, 0.65f, 0.0f, 1.0f),	//11
	vec4(-0.65f, 0.65f, 0.0f, 1.0f),	//12

	//Rectangle
	vec4(-0.95f, 0.35f, 0.0f, 1.0f),	//13
	vec4(-0.95f, 0.15f, 0.0f, 1.0f),	//14
	vec4(-0.65f, 0.15f, 0.0f, 1.0f),	//15
	vec4(-0.65f, 0.35f, 0.0f, 1.0f),	//16
	
	//Selection rectangle
	vec4(0.8f, 0.96f, 0.0f, 1.0f),		//17
	vec4(0.8f, -0.99f, 0.0f, 1.0f),		//18
	vec4(0.9f, -0.99f, 0.0f, 1.0f),		//19
	vec4(0.9f, 0.96f, 0.0f, 1.0f),		//20

	//Free Draw
	vec4(-0.68f, -0.77f, 0.0f, 1.0f),	//21
	vec4(-0.75f, -0.68f, 0.0f, 1.0f),	//22
	vec4(-0.82f, -0.79f, 0.0f, 1.0f),	//23
	vec4(-0.68f, -0.85f, 0.0f, 1.0f),	//24
	vec4(-0.88f, -0.90f, 0.0f, 1.0f),	//25

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

//Number of points used for drawing the two circles and for free draw
const int numPoints = 5000;

//Hold the points for the circle on the sidebar
vec4 circleVertices[numPoints];

//Shortcut for the size of the statically defined objects
int staticSize;
float zoom = 1.0f;
float aspectRatio; 

//Indentifiers used for communicating with shaders
GLuint staticShaders;
GLuint dynamicShaders;
GLuint staticColor;
GLuint dynamicColor;

//Used to index the points used in free draw
int index = 0;

//Enums for user input
enum shapes { RECTANGLE, TRIANGLE, CIRCLE, FREE };
enum inputs { LIGHT_GREEN, DARK_GREEN, YELLOW, WHITE, BLACK, TRANSLATE, ROTATE, SCALE };

//IDs for menus and selections
int operationMenu;
int colorMenu;
int menuID;
int transformation = -1;
int shape = -1;
float inputColor[4];

//Stores and tracks user input
int clickCount = 0;
vec4 tVertices[3];
vec4 rVertices[5];	//5 to make the loop
vec4 cVertices[2];	//Holds the center and point on circumference, actual points generated in function
vec4 fVertices[numPoints];


/*---------------------------------------------------------------*/


//Initialize necessary componenets and send data to the GPU
void init() {

	//Set background color to grey
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	//Set the aspect ratio, used for transformations
	aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	
	// Create and bind a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate vertex buffer object and element buffer object
	GLuint buffers[2];
	glGenBuffers(2, buffers);

	//Create and store points for the circle on the sidebar
	float theta = 6.18f / numPoints;
	for (int i = 0; i < numPoints; i++) {
		float x = -0.8f + 0.15f*cosf(theta*i);
		float y = -0.25f + 0.15f*sinf(theta*i);
		circleVertices[i] = vec4(x, y, 0.0f, 1.0f);
	}

	staticSize = sizeof(staticVertices) + sizeof(circleVertices);

	//Store the vertex data in the vbo
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, staticSize + sizeof(fVertices), NULL, GL_STATIC_DRAW);
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

	//Reset data for transformations
	transformation = -1;
	glUniformMatrix4fv(glGetUniformLocation(dynamicShaders, "modelView"), 1, GL_TRUE, identity());
	glutPostRedisplay();
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
	int first = (staticSize / sizeof(vec4));
	if (shape == TRIANGLE)
		glDrawArrays(GL_TRIANGLES, first, 3);
	else if (shape == RECTANGLE)
		glDrawArrays(GL_LINE_STRIP, first, 5);
	else if (shape == CIRCLE)
		glDrawArrays(GL_POINTS, first, numPoints);
	else if (shape == FREE)
		glDrawArrays(GL_LINE_STRIP, first, numPoints);
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
	float halfWidth = (float)width / 2.0f;
	float halfHeight = (float)height / 2.0f;

	if (x1 < halfWidth)
		x = xf / halfWidth - 1.0f;
	else if (x1 > halfWidth)
		x = (xf - halfWidth) / halfWidth;
	else
		x = 0.0f;

	if (y1 < halfHeight)
		y = -(yf - halfHeight) / halfHeight;
	else if (y1 > halfHeight)
		y = yf / -halfHeight + 1.0f;
	else
		y = 0.0f;
	return vec2(x, y);
}


/*---------------------------------------------------------------*/


void drawTriangle(float x, float y) {

	if (clickCount == 1) {
		tVertices[0] = vec4(x, y, 0.0f, 1.0f);
	}
	else if (clickCount == 2) {
		tVertices[1] = vec4(x, y, 0.0f, 1.0f);
	}
	
	else if (clickCount == 3) {
		tVertices[2] = vec4(x, y, 0.0f, 1.0f);
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(tVertices), tVertices);
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
		rVertices[0] = vec4(x, y, 0.0f, 1.0f);
	}
	
	else if (clickCount == 2) {
		rVertices[2] = vec4(x, y, 0.0f, 1.0f);
		rVertices[1] = vec4(rVertices[0].x, rVertices[2].y, 0.0f, 1.0f);
		rVertices[3] = vec4(rVertices[2].x, rVertices[0].y, 0.0f, 1.0f);
		rVertices[4] = vec4(rVertices[0]);
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(rVertices), rVertices);
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
		cVertices[0] = vec4(x, y, 0.0f, 1.0f);
	}
	else if (clickCount ==2) {
		cVertices[1] = vec4(x, y, 0.0f, 1.0f);
		vec4 points[numPoints];
		
		//Inputs used to draw circle
		float radius = sqrt(pow(cVertices[1].x - cVertices[0].x, 2.0f) + pow(cVertices[1].y - cVertices[0].y, 2.0f));
		float theta = 6.18f / numPoints;

		//Calculate points for circle
		for (int i = 0; i < numPoints; i++) {
			float x = cVertices[0].x + radius*cosf(theta*i);
			float y = cVertices[0].y + radius*sinf(theta*i);

			//Don't draw point if it goes into sidebar or selection rectangle
			if (x > -0.6f && x < 0.8f) {
				points[i] = vec4(x, y, 0.0f, 1.0f);
			}
		}
		
		//Store data in VBO and post a redisplay
		glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(points), points);
		glutPostRedisplay();

		//Reset data
		clickCount = 0;
		cVertices[0] = NULL;
		cVertices[1] = NULL;
	}
}


/*---------------------------------------------------------------*/


void drawFree(float x, float y) {
	if (x > -0.6f && x < 0.8f) {
		fVertices[index++] = vec4(x, y, 0.0f, 1.0f);
		glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(fVertices), fVertices);
		glutPostRedisplay();
	}
}


/*---------------------------------------------------------------*/


void translate(float x, float y) {
	mat4 modelView = Translate(vec4(x, y, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(dynamicShaders, "modelView"), 1, GL_TRUE, modelView);
	glutPostRedisplay();
}


/*---------------------------------------------------------------*/


void rotate(int y) {
	
	float theta = (float)y * 360.0f / (float)glutGet(GLUT_WINDOW_HEIGHT);
	float centerX, centerY;

	//Find the current center of the shape
	if (shape == TRIANGLE) {
		centerX = (tVertices[0].x + tVertices[1].x + tVertices[2].x) / 3.0f;
		centerY = (tVertices[0].y + tVertices[1].y + tVertices[2].y) / 3.0f;
	}
	
	else if (shape == RECTANGLE) {
		if (rVertices[0].x > rVertices[2].x) {
			centerX = rVertices[2].x + (rVertices[0].x - rVertices[2].x) / 2.0f;
		}
		else {
			centerX = rVertices[0].x + (rVertices[2].x - rVertices[0].x) / 2.0f;
		}
		if (rVertices[0].y > rVertices[2].y) {
			centerY = rVertices[2].y + (rVertices[0].y - rVertices[2].y) / 2.0f;
		}
		else {
			centerY = rVertices[0].y + (rVertices[2].y - rVertices[0].y) / 2.0f;
		}
	}

	else if (shape == CIRCLE) {
		centerX = cVertices[0].x;
		centerY = cVertices[0].y;
	}

	else if (shape == FREE) {
		int centerPoint = sizeof(fVertices) / sizeof(vec4);
		centerX = fVertices[centerPoint].x;
		centerY = fVertices[centerPoint].y;
	}

	//Matrix to rotate by theta
	mat4 rotate = RotateZ(theta);
	//Matrix to move triangle back to original position
	mat4 translate1 = Translate(vec4(centerX, centerY, 0.0f, 1.0f));
	//Matrix to move triangle to center
	mat4 translate2 = Translate(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	//Move object to center, rotate about Z axis, move back to original position
	mat4 modelView = translate1 * rotate * translate2;

	//Send matrices to the shaders and post a redisplay
	glUniformMatrix4fv(glGetUniformLocation(dynamicShaders, "modelView"), 1, GL_TRUE, modelView);
	glutPostRedisplay();
}


/*---------------------------------------------------------------*/


void scale(int y) {
	float scale = (float)y * 10.0f / (float)glutGet(GLUT_WINDOW_HEIGHT);

	mat4 modelView = Scale(scale, scale, scale);
	
	//Send matrices to the shaders and post a redisplay
	glUniformMatrix4fv(glGetUniformLocation(dynamicShaders, "modelView"), 1, GL_TRUE, modelView);
	glutPostRedisplay();
}


/*---------------------------------------------------------------*/


//Handle a click on shape menu or selection rectangle
void menuClick(float x, float y, int x1, int y1) {

	if (x < -0.6f) {
		init();
		if (y > 0.5f) {
			clickCount = 0;
			shape = TRIANGLE;
		}
		else if (y > 0.0f) {
			clickCount = 0;
			shape = RECTANGLE;
		}
		else if (y > -0.5f) {
			clickCount = 0;
			shape = CIRCLE;
		}
		else if (y > -1.0f) {
			clickCount = 0;
			shape = FREE;
		}
	}

	else if (x > 0.8f) {
		if (transformation == ROTATE) {
			rotate(y1);
		}
		else if (transformation == SCALE) {
			scale(y1);
		}
	}
}


/*---------------------------------------------------------------*/


//Handle when a click is in the main part of the screen, or the "canvas"
void canvasClick(float x, float y) {
	if (transformation == TRANSLATE) {
		translate(x, y);
	}

	else {

		//Reset the transformation matrix
		glUniformMatrix4fv(glGetUniformLocation(dynamicShaders, "modelView"), 1, GL_TRUE, identity());

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
			if (x > -0.6f && x < 0.8f) {
				rVertices[2] = vec4(x, y, 0.0f, 1.0f);
				rVertices[1] = vec4(rVertices[0].x, rVertices[2].y, 0.0f, 1.0f);
				rVertices[3] = vec4(rVertices[2].x, rVertices[0].y, 0.0f, 1.0f);
				rVertices[4] = vec4(rVertices[0]);

				//Store data in VBO and post a redisplay
				glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(rVertices), rVertices);
				glutPostRedisplay();
			}
		}
	}

		//If it's the third click and TRIANGLE is selected, use drag functionality
		else if (clickCount == 2 && shape == TRIANGLE) {
			if (x > -0.6f && x < 0.8f) {
				tVertices[2] = vec4(x, y, 0.0f, 1.0f);

				//Store data in VBO and post a redisplay
				glBufferSubData(GL_ARRAY_BUFFER, staticSize, sizeof(tVertices), tVertices);
				glutPostRedisplay();
			}
		}
}


/*---------------------------------------------------------------*/


void mouse(int button, int state, int x1, int y1) {
	//Calculate and store world-view coordinates
	vec2 temp = getPosition(x1, y1);
	float x = temp.x;
	float y = temp.y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		//Check if click was on the shapes menu or selection rectangle
		if (x < -0.6f || x > 0.8f) {
			menuClick(x, y, x1, y1);
		}
		//If click was outside of shapes menu
		else {
			canvasClick(x, y);
		}
	}
}


/*---------------------------------------------------------------*/


void menu(int choice) {

	switch (choice) {
	case TRANSLATE:
		transformation = TRANSLATE;
		break;
	case ROTATE:
		transformation = ROTATE;
		break;
	case SCALE:
		transformation = SCALE;
		break;
	case LIGHT_GREEN:
		inputColor[0] = 0.0f; inputColor[1] = 0.5f; inputColor[2] = 0.0f; inputColor[3] = 1.0f;
		break;
	case DARK_GREEN:
		inputColor[0] = 0.0f; inputColor[1] = 0.3f; inputColor[2] = 0.0f; inputColor[3] = 1.0f;
		break;
	case YELLOW:
		inputColor[0] = 0.8f; inputColor[1] = 0.8f; inputColor[2] = 0.0f; inputColor[3] = 1.0f;
		break;
	case WHITE:
		inputColor[0] = 1.0f; inputColor[1] = 1.0f; inputColor[2] = 1.0f; inputColor[3] = 1.0f;
		break;
	case BLACK:
		inputColor[0] = 0.0f; inputColor[1] = 0.0f; inputColor[2] = 0.0f; inputColor[3] = 1.0f;
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