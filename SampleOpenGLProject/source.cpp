#include <iostream>														
#include <sstream>  													
#include <string>														
#include <fstream>														
#include "glew.h"														
#include "glui.h"														
#include "freeglut.h"													
#include "Angel.h"																		
#include "OpenGL\lib\glm\glm.hpp"														
#include "irrKlang\irrKlang.h"
#pragma comment(lib,"irrKlang.lib")																						
																						//							                                                               .---.
																						//							                                                             |\_/|   |											                                                             |   |  /|
using namespace std;																	//							  .----------------------------------------------------------------' |
using namespace Angel;																	//							 /  .-.                                                              |
																						//							|  /   \                                                             |
typedef Angel::vec4  color4;															//							| |\_.  |     _____.___.      _________      ___.                    |
typedef Angel::vec4  point4;															//							|\|  | /|     \__  |   |____  \_   ___ \ __ _\_ |__   ____           |
																						//							| `---' |      /   |   \__  \ /    \  \/|  |  \ __ \_/ __ \          |
int window_id;																			//							|       |      \____   |/ __ \\     \___|  |  / \_\ \  ___/          |
float time;																				//							|       |      / ______(____  /\______  /____/|___  /\___  >         |
int rotatedAngle=0;
float cubeRotationSpeed = 5;															//							|       |      \/           \/        \/          \/     \/          |
vec3 playerCubePos = (0, 0, 0);															//							|       |                                                            |
// Projection transformation parameters													//							|       |                                                           /
																						//							|       |----------------------------------------------------------'
GLfloat  ortho_left = -1.0, ortho_right = 1.0;											//							\       |
GLfloat  ortho_bottom = -1.0, ortho_top = 1.0;											//							 \     /
GLfloat  ortho_zNear = 0.5, ortho_zFar = 3.0;											//							  `---'
																						//											
//point4 points[22754];
//color4 colors[22754];
irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice(); // initialize sound engine
const int NumVertices = 36*2; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int timeSinceStart; int deltaTime;
void initializeUniformVariables(GLuint program);
mat4 generateTranslationMatrix(GLfloat x, GLfloat y, GLfloat z);
point4 points[NumVertices*50];
color4 colors[NumVertices*50];
vec3   normals[NumVertices*50];
char playerCubeMoveDirection;
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 cubeVertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

point4 defaultCubeVertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(1.0, 0.0, 0.3984375, 1.0),  // black
	color4(1.0, 0.0, 0.3984375, 1.0),  // red
	color4(1.0, 0.0, 0.3984375, 1.0),  // yellow
	color4(1.0, 0.0, 0.3984375, 1.0),  // green
	color4(1.0, 0.0, 0.3984375, 1.0),  // blue
	color4(1.0, 0.0, 0.3984375, 1.0),  // magenta
	color4(1.0, 0.0, 0.3984375, 1.0),  // white
	color4(1.0, 0.0, 0.3984375, 1.0)   // cyan
};

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;
int numberOfPlatformsOnScene = 0;
color4 currentCubeColor = color4(1.0, 0.0, 0.3984375, 1.0); bool rTicker = false; bool bTicker = false; bool gTicker = false;
const int sceneSize = 50;
int platformType[sceneSize][sceneSize]; // 0: empty	1: basic
bool actionAlreadyDone = false;
void
quad(point4 * vertices, int a, int b, int c, int d)
{
	// Initialize temporary vectors along the quad's edge to
	//   compute its face normal 
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];

	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
cubicInitializer(point4 * vertices)
{
	quad(vertices, 1, 0, 3, 2);
	quad(vertices, 2, 3, 7, 6);
	quad(vertices, 3, 0, 4, 7);
	quad(vertices, 6, 5, 1, 2);
	quad(vertices, 4, 5, 6, 7);
	quad(vertices, 5, 4, 0, 1);
}

float radianConstant = 0.0174533;
int modelIndex = 0;

// position -5.263538, zNear:8.359282, zFar:15.757410,
vec3 position = vec3(-5.263538, 8.359282, 15.757410);
float horizontalAngle = 2.825704f;
// vertical angle : look downwards at the plate
float verticalAngle = -0.4725f;

vec3 direction(
	cos(verticalAngle) * sin(horizontalAngle),
	sin(verticalAngle),
	cos(verticalAngle) * cos(horizontalAngle)
);

// Right vector
vec3 right_vector = vec3(
	sin(horizontalAngle - 3.14f / 2.0f),
	0,
	cos(horizontalAngle - 3.14f / 2.0f)
);

// Up vector : perpendicular to both direction and right
vec3 up = cross(right_vector, direction);



float speed = 0.1f; // 3 units / second
float mouseSpeed = 0.005f;

int mouseX, mouseY;

// Viewing transformation parameters

GLfloat radius = 8.000000;
GLfloat theta = 2.617994;
GLfloat phi = 0.872665;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint  trs_matrix;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  normal_matrix;  
					 // Projection transformation parameters

GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 2.670417, zFar = 37.022503;

GLuint  projection; // projection matrix uniform shader variable location

					//----------------------------------------------------------------------------

					// quad generates two triangles for each face and assigns colors
					//    to the vertices
GLuint ambientProduct;
GLuint diffuseProduct;
GLuint specularProduct;
GLuint lightPosition;
GLuint shininess;

//----------------------------------------------------------------------------
void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<GLushort> &elements)
{
	ifstream in(filename, ios::in);
	if (!in)
	{
		cerr << "Cannot open " << filename << endl; exit(1);
	}

	string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			istringstream s(line.substr(2));
			glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a; s >> b; s >> c;
			a--; b--; c--;
			elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/* ignoring this line */
		}
		else
		{
			/* ignoring this line */
		}
	}

}
//----------------------------------------------------------------------------

void
setModel(std::vector<glm::vec4> vertices, std::vector<GLushort> faces, color4 color)
{
	
	for each (GLushort element in faces)
	{
		glm::vec4 currentPoint = vertices[element];
		points[modelIndex] = point4(currentPoint.x, currentPoint.y, currentPoint.z, 1.0);
		colors[modelIndex] = color;
		modelIndex++;
	}
	printf("%d \n", modelIndex);
}

void
SetupSurface(void) {

	points[modelIndex] = vec4(0.0, 0.0, 0.0 , 1.0); //setting up the vertices for the outer circle which will create the crescent
	colors[modelIndex] = vec4(1.0, 0.0, 0.0, 1.0); modelIndex++;
	for (modelIndex; modelIndex < 360; modelIndex++) {
		points[modelIndex] = vec4(cos(modelIndex*radianConstant)*2.7, 0.0, sin(modelIndex*radianConstant)*2.7,1.0);
		colors[modelIndex] = vec4(1.0, 0.0, 0.0, 1.0);
	}
	points[modelIndex] = vec4(2.69958878, 0.0, 0.0471215174, 1.0);
	colors[modelIndex] = vec4(1.0, 0.0, 0.0, 1.0); modelIndex++;
	printf("%d \n", modelIndex);
}
//----------------------------------------------------------------------------

float
gridUnitToCoord(int gridUnit)
{
	return gridUnit;
}

void
addPlatformPiece(int gameGridX, int gameGridY, int type)
{
	point4 platformPieceVertices[8] = {
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(-0.5, -0.80,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(-0.5,  -0.50,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(0.5,  -0.50,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(0.5, -0.80,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(-0.5, -0.80, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(-0.5,  -0.50, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(0.5,  -0.50, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),0.0, gridUnitToCoord(gameGridY))*point4(0.5, -0.80, -0.5, 1.0)
	};
	point4 * platformPieceVPointer = platformPieceVertices;
	cubicInitializer(platformPieceVPointer);
	platformType[gameGridX + sceneSize/2][gameGridY + sceneSize/2] = type;
	numberOfPlatformsOnScene++;
}

void
setStartLevel()
{
	addPlatformPiece(-2, -2, 3);	addPlatformPiece(-1, -2, 1);
								addPlatformPiece(-1, -1, 1); addPlatformPiece(0, -1, 1); addPlatformPiece(1, -1, 1);	addPlatformPiece(2, -1, 1);	addPlatformPiece(3, -1, 1);
								addPlatformPiece(-1, 0, 1); addPlatformPiece(0, 0, 1); addPlatformPiece(1, 0, 1);	addPlatformPiece(2, 0, 2);	addPlatformPiece(3, 0, 1);	
								addPlatformPiece(-1, 1, 1); addPlatformPiece(0, 1, 1); addPlatformPiece(1, 1, 1);	addPlatformPiece(2, 1, 1);	addPlatformPiece(3, 1, 1);
}

// OpenGL initialization
void
init()
{
	point4 * cubeVPointer = cubeVertices;
	cubicInitializer(cubeVPointer);
	if (!engine)printf("could not start engine"); // 
	setStartLevel();
	engine->play2D("presenting_vvvvvv.mp3", true);
	srand(static_cast <unsigned> (glutGet(GLUT_ELAPSED_TIME)));

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

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

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)+ sizeof(colors)));

	initializeUniformVariables(program);

	glEnable(GL_DEPTH_TEST); glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.77734375, 0.6484375, 1.0);

}
void
initializeUniformVariables(GLuint program) {
	ambientProduct = glGetUniformLocation(program, "AmbientProduct");
	diffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
	specularProduct = glGetUniformLocation(program, "SpecularProduct");
	lightPosition = glGetUniformLocation(program, "LightPosition");
	shininess = glGetUniformLocation(program, "Shininess");
	model_view = glGetUniformLocation(program, "model_view");
	normal_matrix = glGetUniformLocation(program, "normal_matrix");
	projection = glGetUniformLocation(program, "projection");
	trs_matrix = glGetUniformLocation(program, "trs_matrix");
}

//----------------------------------------------------------------------------
mat4
generateTranslationMatrix(GLfloat x, GLfloat y, GLfloat z) {
	return mat4(1.0, 0.0, 0.0, x,
		0.0, 1.0, 0.0, y,
		0.0, 0.0, 1.0, z,
		0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------
mat4
generateRotationMatrix(int angleX, int angleY, int angleZ) {

	vec3 angles = vec3(angleX * radianConstant, angleY * radianConstant, angleZ * radianConstant);
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

//----------------------------------------------------------------------------
void MouseController(int x, int y)
{

	mouseX = x; 
	mouseY = y;
	horizontalAngle += mouseSpeed * 0.01 *float(800 / 2 - mouseX);
	verticalAngle += mouseSpeed *0.01 *float(800 / 2 - mouseY);

	if (x != 800 / 2 || y != 800 / 2)
		glutWarpPointer(800/2, 800 / 2);
	
	glutPostRedisplay();
}
//----------------------------------------------------------------------------

void
updateLightProperties(color4 baseObjectColor)
{
	// Initialize shader lighting parameters
	point4 light_position(-5.0, 15.0, 10.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
	color4 material_diffuse = baseObjectColor;
	color4 material_specular = baseObjectColor;
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(ambientProduct, 1, ambient_product);
	glUniform4fv(diffuseProduct, 1, diffuse_product);
	glUniform4fv(specularProduct, 1, specular_product);

	glUniform4fv(lightPosition, 1, light_position);

	glUniform1f(shininess, material_shininess);

}

void
drawPlatforms() 
{
	updateLightProperties(color4(1.0, 1.0, 1.0, 1.0));
	
	for (int i = 0; i < numberOfPlatformsOnScene; i++)
	{
		if (i==0)
		{
			updateLightProperties(color4(1.0, 0.0, 0.0, 1.0));
		}
		if (i==10)
		{
			updateLightProperties(color4(0.0, 1.0, 0.0, 1.0));
		}
		
		glDrawArrays(GL_TRIANGLES, 36 + i*36, 36);
		updateLightProperties(color4(1.0, 1.0, 1.0, 1.0));
	}
}

void
MoveCube()
{
	mat4 playerCubeRotationMatrix;
	mat4 playerCubeTranslationMatrix;
	mat4 playerCubeReverseTranslationMatrix;
	if (playerCubeMoveDirection == 'R' || playerCubeMoveDirection == 'L' || playerCubeMoveDirection == 'U' || playerCubeMoveDirection == 'D')
	{
		int angleToRotate = (cubeRotationSpeed * deltaTime/10);
		
		if (angleToRotate + rotatedAngle > 90)
		{
			angleToRotate = 90 - rotatedAngle;
		}
		
		switch (playerCubeMoveDirection) {
		case 'U': playerCubeRotationMatrix = generateRotationMatrix(angleToRotate, 0, 0); playerCubeTranslationMatrix = generateTranslationMatrix(0, -0.5, -0.5 + (playerCubePos.z)); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0, 0.5, 0.5-(playerCubePos.z)); break;
		case 'D': playerCubeRotationMatrix = generateRotationMatrix(-angleToRotate, 0, 0);  playerCubeTranslationMatrix = generateTranslationMatrix(0, -0.5, 0.5 + (playerCubePos.z)); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0, 0.5, -0.5 - (playerCubePos.z)); break;
		case 'R': playerCubeRotationMatrix = generateRotationMatrix(0, 0, -angleToRotate);  playerCubeTranslationMatrix = generateTranslationMatrix(0.5 + (playerCubePos.x), -0.5, 0); playerCubeReverseTranslationMatrix = generateTranslationMatrix(-0.5 - (playerCubePos.x), 0.5, 0); break;
		case 'L': playerCubeRotationMatrix = generateRotationMatrix(0, 0, angleToRotate); playerCubeTranslationMatrix = generateTranslationMatrix(-0.5 + (playerCubePos.x), -0.5, 0); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0.5 - (playerCubePos.x), 0.5, 0); break;
		}

		for (int i = 0; i < 8; i++)
		{
			cubeVertices[i] = playerCubeTranslationMatrix * playerCubeRotationMatrix * playerCubeReverseTranslationMatrix * cubeVertices[i];
		}

		point4 * cubeVPointer = cubeVertices;
		Index = 0;
		cubicInitializer(cubeVPointer);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

		rotatedAngle += angleToRotate;
		if (rotatedAngle == 90)
		{
			switch (playerCubeMoveDirection) {
			case 'U':  playerCubePos.z--;  break;
			case 'L':  playerCubePos.x--;  break;
			case 'D':  playerCubePos.z++;  break;
			case 'R':  playerCubePos.x++;  break;
			}
			playerCubeMoveDirection = 'n';
			rotatedAngle = 0;
			engine->play2D("hit.wav");
		}
	}
}
void
DropCube()
{
	mat4 playerCubeTranslationMatrix = generateTranslationMatrix(0.0,-1.0* deltaTime/20,0.0);

		for (int i = 0; i < 8; i++)
		{
			cubeVertices[i] = playerCubeTranslationMatrix * cubeVertices[i];
		}

		point4 * cubeVPointer = cubeVertices;
		Index = 0;
		cubicInitializer(cubeVPointer);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


		
		playerCubeMoveDirection = 'n';

}
void
Respawn()
{
	std::copy(defaultCubeVertices, defaultCubeVertices + 8, cubeVertices);
	point4 * cubeVPointer = cubeVertices;
	Index = 0;
	cubicInitializer(cubeVPointer);
	playerCubePos = vec3(0, 0, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
	currentCubeColor = color4(1.0, 0.0, 0.3984375, 1.0);
	playerCubeMoveDirection = 'n';

}
float
OscillateColorElement(float &colorElement, bool &colorTicker) {
	float colorRandom = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*deltaTime / 850;
	if (colorElement+colorRandom > 1)
	{
		colorElement = 1;
		colorTicker = true;
	}
	if (colorElement + colorRandom < 0)
	{
		colorElement = 0;
		colorTicker = false;
	}
	if (colorTicker == false)
	{
		colorElement += colorRandom;
	}
	if (colorTicker == true)
	{
		colorElement -= colorRandom;
	}
	return colorElement;
}
void
RandomizeColor(color4 &color){
	color.x = OscillateColorElement(color.x, rTicker);
	color.y = OscillateColorElement(color.y, gTicker);
	color.z = OscillateColorElement(color.z, bTicker);
}
void
InitializeLevel1()
{
	RandomizeColor(currentCubeColor);
}
void
CalculateDeltaTime()
{
	int oldTimeSinceStart = timeSinceStart;
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (timeSinceStart - oldTimeSinceStart);
	oldTimeSinceStart = timeSinceStart;
}
//----------------------------------------------------------------------------
void
display(void)
{
	CalculateDeltaTime();
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 2)
	{
		InitializeLevel1();
	}
	//if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] != 2)
	//{

	//}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 3)
	{
		exit(EXIT_SUCCESS);
	}
	if (platformType[(int)playerCubePos.x + sceneSize/2][(int)playerCubePos.z + sceneSize/2] == 0)
	{
		DropCube();
	}
	else
	{
		MoveCube();
	}
	updateLightProperties(currentCubeColor);

	direction = vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	right_vector = vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	up = cross(right_vector, direction);

	// Camera matrix
	mat4 mView = LookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mView);

	/*mat4  p = Ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, zNear, zFar);*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	
	
	mat4 r = generateRotationMatrix(0,0,0);
	glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, r);
	mat4 t = generateTranslationMatrix(0, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	drawPlatforms();

	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}
//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q': engine->drop(); // delete engine
		exit(EXIT_SUCCESS);
		break;
	case 'w': case 'W': if (rotatedAngle == 0) { playerCubeMoveDirection = 'U';} break;
	case 'a': case 'A': if (rotatedAngle == 0) { playerCubeMoveDirection = 'L';} break;
	case 's': case 'S': if (rotatedAngle == 0) { playerCubeMoveDirection = 'D';} break;
	case 'd': case 'D': if (rotatedAngle == 0) { playerCubeMoveDirection = 'R';} break;
	case 'r': case 'R': Respawn(); break;
	case 'f': printf("verticalAngle:%f, zNear:%f, zFar:%f, radius:%f, theta : %f, phi:%f\n", horizontalAngle, position.y, position.z, radius, theta, phi); break;

	case ' ':  // reset values to their defaults
		zNear = 0.5;
		zFar = 3.0;

		radius = 1.0;
		theta = 0.0;
		phi = 0.0;
		break;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------
void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: position += direction * speed; break;
	case GLUT_KEY_DOWN: position -= direction * speed; break;
	case GLUT_KEY_RIGHT: position += right_vector * speed; break;
	case GLUT_KEY_LEFT: position -= right_vector * speed; break;
	}
	glutPostRedisplay();
}

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = GLfloat(width) / height;
}

//----------------------------------------------------------------------------

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
	glutInitWindowSize(1280, 720);
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE
#if _DEBUG		
		| GLUT_DEBUG
#endif
	);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	window_id = glutCreateWindow("Experiment 4 - Cem Aslan 21426639");
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

	/*setGluiUp();*/

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutSpecialFunc(SpecialInput);
	glutPassiveMotionFunc(MouseController);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutMainLoop();
	return 0;
}
