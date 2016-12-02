// Two-Dimensional Sierpinski Gasket       
// Generated using randomly selected vertices and bisection
#include <iostream>
#include <string>
#include "glew.h"
#include "glui.h"
#include "freeglut.h"
#include "Angel.h"

using namespace std;
using namespace Angel;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

int window_id;
int window_width = 800 + 217; //217px is for the glui window
int window_height = 800;
int vx, vy, vw, vh;
GLUI *glui_subwindow;

const int NumVertices = 36*2; //(6 faces)(2 triangles/face)(3 vertices/triangle)*2 cubes
float radianConstant = 0.0174533;
point4 points[NumVertices];
color4 colors[NumVertices];

int rotationType = 0;
GLint rotationSpeedCenter = 1, rotationSpeedOrbiting = 1;
int animationSpeed = 1, orbitSpeed = 1;
int counter = 0;
float orbitRad = 0.55, scaleCoeff = 1.0;
vec4 secondaryCubePos = vec4(orbitRad, orbitRad, 0.0, 1.0);
bool centerRotating = true, outerRotating = true, orbitalRotation = true, animationRunning = true;

mat4 scaleMat = mat4(scaleCoeff, 0.0, 0.0, 0.0, 
					0.0, scaleCoeff, 0.0, 0.0,
					0.0, 0.0, scaleCoeff, 0.0,
					0.0, 0.0, 0.0, 1);

float translateX = 0.0, translateY = 0.0;
mat4 translateMat = mat4(1.0, 0.0, 0.0, translateX,
						0.0, 1.0, 0.0, translateY,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0);

mat4 rotationMat = mat4(1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0);

point4 cubeVertices[24] = {
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),

	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),

	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0),

	point4(0.5,  0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),

	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0),

	point4(-0.5,  0.5, -0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0)
};

color4 cubeColors[24] = {
	color4(1.0, 0.0, 0.0, 1.0), //red
	color4(1.0, 0.0, 0.0, 1.0),
	color4(1.0, 0.0, 0.0, 1.0),
	color4(1.0, 0.0, 0.0, 1.0),

	color4(0.0, 1.0, 0.0, 1.0), //green
	color4(0.0, 1.0, 0.0, 1.0),
	color4(0.0, 1.0, 0.0, 1.0),
	color4(0.0, 1.0, 0.0, 1.0),

	color4(0.0, 0.0, 1.0, 1.0), //blue
	color4(0.0, 0.0, 1.0, 1.0),
	color4(0.0, 0.0, 1.0, 1.0),
	color4(0.0, 0.0, 1.0, 1.0),

	color4(1.0, 1.0, 0.0, 1.0), //yellow
	color4(1.0, 1.0, 0.0, 1.0),
	color4(1.0, 1.0, 0.0, 1.0),
	color4(1.0, 1.0, 0.0, 1.0),

	color4(0.0, 1.0, 1.0, 1.0), //cyan
	color4(0.0, 1.0, 1.0, 1.0),
	color4(0.0, 1.0, 1.0, 1.0),
	color4(0.0, 1.0, 1.0, 1.0),

	color4(1.0, 0.0, 1.0, 1.0), //magenta
	color4(1.0, 0.0, 1.0, 1.0),
	color4(1.0, 0.0, 1.0, 1.0),
	color4(1.0, 0.0, 1.0, 1.0)
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      centerCubeAxis = Xaxis;
int      orbitingCubeAxis = Xaxis;
GLfloat  ThetaCenter[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  ThetaOrbiting[NumAxes] = { 0.0, 0.0, 0.0 };

				//----------------------------------------------------------------------------

				// quad generates two triangles for each face and assigns colors
				//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d, mat4 scaleMatrice, mat4 translateMat, mat4 rotateMat)
{
	colors[Index] = cubeColors[a]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[a]; Index++;
	colors[Index] = cubeColors[b]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[b]; Index++;
	colors[Index] = cubeColors[c]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[c]; Index++;
	colors[Index] = cubeColors[a]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[a]; Index++;
	colors[Index] = cubeColors[c]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[c]; Index++;
	colors[Index] = cubeColors[d]; points[Index] = translateMat*rotateMat*scaleMatrice*cubeVertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube(mat4 scaleMat, mat4 translateMat, mat4 rotateMat)
{
	quad(0, 1, 2, 3, scaleMat, translateMat, rotateMat);
	quad(4, 5, 6, 7, scaleMat, translateMat, rotateMat);
	quad(8, 9, 10, 11, scaleMat, translateMat, rotateMat);
	quad(12, 13, 14, 15, scaleMat, translateMat, rotateMat);
	quad(16, 17, 18, 19, scaleMat, translateMat, rotateMat);
	quad(20, 21, 22, 23, scaleMat, translateMat, rotateMat);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init(void)
{
	colorcube(scaleMat, translateMat, rotationMat);
	colorcube(scaleMat, translateMat, rotationMat);
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
mat4
generateRotationMatrix(GLfloat Theta[3]) {

	vec3 angles = vec3(Theta[0] * radianConstant, Theta[1] * radianConstant, Theta[2] * radianConstant);
	vec3 c = vec3(cos(angles.x), cos(angles.y), cos(angles.z));
	vec3 s = vec3(sin(angles.x), sin(angles.y), sin(angles.z));

	mat4 rx = mat4(1.0, 0.0, 0.0, 0.0,
		0.0, c.x, s.x, 0.0,
		0.0, -s.x, c.x, 0.0,
		0.0, 0.0, 0.0, 1.0);

	mat4 ry = mat4(c.y, 0.0, -s.y, 0.0,
		0.0, 1.0, 0.0, 0.0,
		s.y, 0.0, c.y, 0.0,
		0.0, 0.0, 0.0, 1.0);

	mat4 rz = mat4(c.z, -s.z, 0.0, 0.0,
		s.z, c.z, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
	return rx*ry*rz;
}

//----------------------------------------------------------------------------
void
incrementCounters(){
	if (centerRotating == true)
	{
		ThetaCenter[centerCubeAxis] += 0.5*rotationSpeedCenter;

		if (ThetaCenter[centerCubeAxis] > 360.0) {
			ThetaCenter[centerCubeAxis] -= 360.0;
		}
	}
	if (outerRotating == true)
	{
		ThetaOrbiting[orbitingCubeAxis] += 0.5*rotationSpeedOrbiting;

		if (ThetaOrbiting[orbitingCubeAxis] > 360.0) {
			ThetaOrbiting[orbitingCubeAxis] -= 360.0;
		}
	}
	if (animationRunning == true)
	{
		counter += 25 * animationSpeed;

	}
	if (orbitalRotation == true)
	{
		mat4 rz = mat4(cos(orbitSpeed*0.01), -sin(orbitSpeed*0.01), 0.0, 0.0,
			sin(orbitSpeed*0.01), cos(orbitSpeed*0.01), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);
		secondaryCubePos = rz*secondaryCubePos;
	}
}

mat4
generateScaleMatrix(float scaleCoefficient) {
	return mat4(scaleCoefficient, 0.0, 0.0, 0.0,
		0.0, scaleCoefficient, 0.0, 0.0,
		0.0, 0.0, scaleCoefficient, 0.0,
		0.0, 0.0, 0.0, 1.0);

}

//----------------------------------------------------------------------------
void
display(void)
{
	incrementCounters();

	mat4 rotationMatCenter = generateRotationMatrix(ThetaCenter);
	mat4 rotationMatOrbiting = generateRotationMatrix(ThetaOrbiting);

	scaleCoeff = (sin(counter*0.003) + 2.5)/6;
	scaleMat = generateScaleMatrix(scaleCoeff);

	Index = 0;
	colorcube(scaleMat, translateMat, rotationMatCenter);

	scaleMat = generateScaleMatrix(0.25);
	
	mat4 translateMat = mat4(1.0, 0.0, 0.0, secondaryCubePos.x,
							0.0, 1.0, 0.0, secondaryCubePos.y,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);

	colorcube(scaleMat, translateMat, rotationMatOrbiting);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices); //draw the center cube
	glDrawArrays(GL_TRIANGLES, NumVertices/2, NumVertices); //draw the center cube

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
rotationController(int state)
{
	if (state == 0)
	{
		printf("rotation1: %d \n", rotationType);
		switch (rotationType) {
			case 0:    centerCubeAxis = Xaxis;  break;
			case 1:  centerCubeAxis = Yaxis;  break;
			case 2:   centerCubeAxis = Zaxis;  break;
			}
	}
	if (state == 1)
	{
		printf("rotation2: %d \n", rotationType);
		switch (rotationType) {
			case 0:    orbitingCubeAxis = Xaxis;  break;
			case 1:  orbitingCubeAxis = Yaxis;  break;
			case 2:   orbitingCubeAxis = Zaxis;  break;
			}
	}
}

//----------------------------------------------------------------------------

void
stopRotation(int state)
{
	switch (state) {
	case 0: centerRotating = !centerRotating; break;
	case 1: outerRotating = !outerRotating; break;
	case 2:	orbitalRotation = !orbitalRotation; break;
	case 3:	animationRunning = !animationRunning; break;
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	if (glutGetWindow() != window_id)
		glutSetWindow(window_id);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	GLUI_Master.get_viewport_area(&vx, &vy, &vw, &vh);
	glViewport(vx, vy, vw, vh);
	glutPostRedisplay();
}

//-----------------------------SETTING UP GLUI-----------------------------------------
void
setGluiUp() {
	GLUI_Master.set_glutIdleFunc(idle);
	GLUI_Master.set_glutReshapeFunc(reshape);

	glui_subwindow = GLUI_Master.create_glui_subwindow(window_id, GLUI_SUBWINDOW_RIGHT);
	glui_subwindow->set_main_gfx_window(window_id);

	GLUI_RadioGroup *radio;
	GLUI_Spinner *spinner;
	GLUI_Panel *obj_panel1 = new GLUI_Panel(glui_subwindow, "Center Cube");
	radio = new GLUI_RadioGroup(obj_panel1, &rotationType, 0, rotationController);
	new GLUI_RadioButton(radio, "Rotate X");
	new GLUI_RadioButton(radio, "Rotate Y");
	new GLUI_RadioButton(radio, "Rotate Z");
	spinner = new GLUI_Spinner(obj_panel1, "Rotation Speed:", &rotationSpeedCenter, 5);
	spinner->set_int_limits(-5, 5);
	glui_subwindow->add_button_to_panel(obj_panel1, "Start/Stop Rotation", 0, stopRotation);
	glui_subwindow->add_separator_to_panel(obj_panel1);
	spinner = new GLUI_Spinner(obj_panel1, "Animation Speed:", &animationSpeed, 5);
	spinner->set_int_limits(-5, 5);
	glui_subwindow->add_button_to_panel(obj_panel1, "Start/Stop Animation", 3, stopRotation);

	glui_subwindow->add_separator();

	GLUI_RadioGroup *radio2;
	GLUI_Spinner *spinner2;
	GLUI_Panel *obj_panel2 = new GLUI_Panel(glui_subwindow, "Outer Cube");
	radio2 = new GLUI_RadioGroup(obj_panel2, &rotationType, 1, rotationController);
	new GLUI_RadioButton(radio2, "Rotate X");
	new GLUI_RadioButton(radio2, "Rotate Y");
	new GLUI_RadioButton(radio2, "Rotate Z");
	spinner = new GLUI_Spinner(obj_panel2, "Rotation Speed:", &rotationSpeedOrbiting, 5);
	spinner->set_int_limits(-5, 5);
	glui_subwindow->add_button_to_panel(obj_panel2, "Start/Stop Rotation", 1, stopRotation);
	glui_subwindow->add_separator_to_panel(obj_panel2);
	spinner = new GLUI_Spinner(obj_panel2, "Orbital Rotation Speed:", &orbitSpeed, 5);
	spinner->set_int_limits(-5, 5);
	glui_subwindow->add_button_to_panel(obj_panel2, "Start/Stop Orbital Rotation", 2, stopRotation);
}

//--------------------------DEBUG---------------------------------------
void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {

	cout << "---------------------opengl-callback-start------------" << endl;
	cout << "message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;

	cout << "id: " << id << endl;
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
}

//-------------------------------MAIN-----------------------------------
int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);

	// no version check cause of the glui library

	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE
#if _DEBUG		
		| GLUT_DEBUG
#endif
	);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	window_id = glutCreateWindow("Experiment 3 - Cem Aslan 21426639");
	glewExperimental = GL_TRUE;

	glewInit();

#if _DEBUG
	if (glDebugMessageCallback) {
		cout << "Register OpenGL debug callback " << endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
	else
		cout << "glDebugMessageCallback not available" << endl;
#endif

	cout << "OpenGL initialized: OpenGL version: " << glGetString(GL_VERSION) << " GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;


	init();

	setGluiUp();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}

