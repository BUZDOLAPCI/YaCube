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
int rotatedAngle = 0;																		//							|		|															 |
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
const int NumVertices = 36 * 2; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int timeSinceStart; int deltaTime;
void initializeUniformVariables(GLuint program);
mat4 generateTranslationMatrix(GLfloat x, GLfloat y, GLfloat z);
mat4 generateRotationMatrix(int x, int y, int z);
vector<point4> points;
vector<color4> colors;
vector<vec3> normals;
//point4 points[NumVertices*50];
//color4 colors[NumVertices*50];			
//vec3   normals[NumVertices*50];
int playerCubeIndex;
char playerCubeMoveDirection;
bool freecamToggle = false;
bool playerMovementLockToggle = false;
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

// quadInitializer generates two triangles for each face and assigns colors
//    to the vertices


int numberOfPlatformsOnScene = 0;
color4 currentCubeColor = color4(1.0, 0.0, 0.3984375, 1.0); bool rTicker = false; bool bTicker = false; bool gTicker = false;
color4 bb8Color = color4(0.234, 4.56, 0.4345, 1.0);
const int sceneSize = 50;
int platformType[sceneSize][sceneSize]; // 0: empty	1: basic
int platformIndex[sceneSize][sceneSize];
int platformLeveled[sceneSize][sceneSize];
float platformLevitationSpeed[sceneSize][sceneSize];
bool actionAlreadyDone = false;
void
quadInitializer(point4 * vertices, int a, int b, int c, int d)
{
	// Initialize temporary vectors along the quadInitializer's edge to
	//   compute its face normal 
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];

	vec3 normal = normalize(cross(u, v));
	normals.push_back(normal); colors.push_back(vertex_colors[a]); points.push_back(vertices[a]);
	normals.push_back(normal); colors.push_back(vertex_colors[b]); points.push_back(vertices[b]);
	normals.push_back(normal); colors.push_back(vertex_colors[c]); points.push_back(vertices[c]);
	normals.push_back(normal); colors.push_back(vertex_colors[a]); points.push_back(vertices[a]);
	normals.push_back(normal); colors.push_back(vertex_colors[c]); points.push_back(vertices[c]);
	normals.push_back(normal); colors.push_back(vertex_colors[d]); points.push_back(vertices[d]);
}

void
quadUpdater(point4 * vertices, int a, int b, int c, int d, int &index)
{
	// Initialize temporary vectors along the quadInitializer's edge to
	//   compute its face normal 
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];

	vec3 normal = normalize(cross(u, v));

	normals[index] = normal; colors[index] = vertex_colors[a]; points[index] = vertices[a]; index++;
	normals[index] = normal; colors[index] = vertex_colors[b]; points[index] = vertices[b]; index++;
	normals[index] = normal; colors[index] = vertex_colors[c]; points[index] = vertices[c]; index++;
	normals[index] = normal; colors[index] = vertex_colors[a]; points[index] = vertices[a]; index++;
	normals[index] = normal; colors[index] = vertex_colors[c]; points[index] = vertices[c]; index++;
	normals[index] = normal; colors[index] = vertex_colors[d]; points[index] = vertices[d]; index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
cubicInitializer(point4 * vertices)
{
	quadInitializer(vertices, 1, 0, 3, 2);
	quadInitializer(vertices, 2, 3, 7, 6);
	quadInitializer(vertices, 3, 0, 4, 7);
	quadInitializer(vertices, 6, 5, 1, 2);
	quadInitializer(vertices, 4, 5, 6, 7);
	quadInitializer(vertices, 5, 4, 0, 1);
}

void
cubicUpdater(point4 * vertices, int index)
{
	quadUpdater(vertices, 1, 0, 3, 2, index);
	quadUpdater(vertices, 2, 3, 7, 6, index);
	quadUpdater(vertices, 3, 0, 4, 7, index);
	quadUpdater(vertices, 6, 5, 1, 2, index);
	quadUpdater(vertices, 4, 5, 6, 7, index);
	quadUpdater(vertices, 5, 4, 0, 1, index);
}

float radianConstant = 0.0174533;
int modelIndex = 0;

// position -5.263538, zNear:8.359282, zFar:15.757410,
vec3 initialPosition = vec3(-5.270120, 5.348522, 14.118294);
vec3 position = initialPosition;
float initialHorizontalAngle = 2.820005;
float horizontalAngle = initialHorizontalAngle;
// vertical angle : look downwards at the plate
float initialVerticalAngle = -0.292000;
float verticalAngle = initialVerticalAngle;
vec3 direction;
//positionX: -5.270120, positionY : 5.348522, positionZ : 14.118294, horizontalAngle : 2.820005
//	verticalAngle : -0.292000, zNear : 5.348522, zFar : 14.118294, radius : 8.000000, theta : 2.617994, phi : 0.872665

// Right vector
vec3 right_vector;

// Up vector : perpendicular to both direction and right
vec3 up = cross(right_vector, direction);

float speed = 0.3f; // 3->9? units / second
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
GLuint eye_position;
GLuint toon_enable;

GLint toonEnable = 0;
// Projection transformation parameters

GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 2.670417, zFar = 1000.022503;

GLuint  projection; // projection matrix uniform shader variable location

					//----------------------------------------------------------------------------

					// quadInitializer generates two triangles for each face and assigns colors
					//    to the vertices
GLuint ambientProduct;
GLuint diffuseProduct;
GLuint specularProduct;
GLuint lightPosition;
GLuint shininess;

//----------------------------------------------------------------------------

mat4
generateScaleMatrix(float scaleCoefficient) {
	return mat4(scaleCoefficient, 0.0, 0.0, 0.0,
		0.0, scaleCoefficient, 0.0, 0.0,
		0.0, 0.0, scaleCoefficient, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

int
gridUnitToCoord(int gridUnit)
{
	return gridUnit;
}

float bottomCoord = -20.0;
void
addPlatformPiece(int gameGridX, int gameGridY, int type)
{
	point4 platformPieceVertices[8] = {
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(-0.5, -0.80,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(-0.5,  -0.50,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(0.5,  -0.50,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(0.5, -0.80,  0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(-0.5, -0.80, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(-0.5,  -0.50, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(0.5,  -0.50, -0.5, 1.0),
		generateTranslationMatrix(gridUnitToCoord(gameGridX),bottomCoord, gridUnitToCoord(gameGridY))*point4(0.5, -0.80, -0.5, 1.0)
	};

	platformIndex[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = points.size();
	platformType[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = type;
	platformLeveled[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 0;

	float levitationSpeed = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	levitationSpeed = fmod(levitationSpeed, 0.25) + 0.25;
	platformLevitationSpeed[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = levitationSpeed;

	point4 * platformPieceVPointer = platformPieceVertices;
	cubicInitializer(platformPieceVPointer);
	numberOfPlatformsOnScene++;
}

void
RemovePlatformPiece(int gameGridX, int gameGridY)
{
	platformLeveled[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 2;
}

void
RemoveAllPlatforms()
{
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			if (platformType[i][j] != 0)
			{
				if ((int)playerCubePos.x == (i - (sceneSize / 2)) && (int)playerCubePos.z == (j - (sceneSize / 2)))
				{
					continue;
				}
				else
				{
					RemovePlatformPiece(i - (sceneSize / 2), j - (sceneSize / 2));
				}
			}
			else
			{
				continue;
			}
		}
	}
}

//p 0 1 2 e  tabs doesn't matter
void importLevel(string file) {
	string line;
	ifstream myfile(file);
	while (getline(myfile, line))
	{
		istringstream iss(line);
		char type;
		char type2;
		GLint a, b, c;
		GLint d, e, f;
		iss >> type;
		while (type == '\t') {
			iss >> type;
		}
		if (type == 'p') {
			iss >> a >> b >> c;
			addPlatformPiece(a, b, c);
			iss >> type2;
			while (type2 == 'p') {
				iss >> d >> e >> f;
				addPlatformPiece(d, e, f);
				iss >> type2;
			}
		}
	}
	myfile.close();
}

GLint importFromOBJ(const char* filename, mat4 scaleMatrix, mat4 rotationMatrix, mat4 TranslationMatrix) {
	FILE * file = fopen(filename, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return -1;
	}
	vector<point4> importedVertices;
	vector<vec3> importedNormals;
	GLint vertexCount = 0;
	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		GLfloat a, b, c;
		GLint d, e, f;
		if (strcmp(lineHeader, "v") == 0) {
			fscanf(file, "%f %f %f\n", &a, &b, &c);
			point4 point = point4(a, b, c, 1.0);
			//importedVertices.push_back(scaling*translation*point); 
			/*importedVertices.push_back(scaleMatrix*generateRotationMatrix(90, 0, 0)*point);*/
			importedVertices.push_back(TranslationMatrix*rotationMatrix*scaleMatrix*point);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			//We don't use textures.
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			fscanf(file, "%f %f %f\n", &a, &b, &c);
			point4 point = point4(a, b, c, 1.0);
			vec3 normal = vec3(a, b, c);
			//importedNormals.push_back(scaling*translation*normal);
			vec4 temp = rotationMatrix*vec4(normal, 1.0);
			importedNormals.push_back(vec3(temp.x, temp.y, temp.z));
		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				//int matches2 = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				//if (matches2 != 6) {
				printf("File can't be read by our simple parser :( Try exporting with other options\n");
				return -1;
				//}
			}
			points.push_back(importedVertices[vertexIndex[0] - 1]);
			points.push_back(importedVertices[vertexIndex[1] - 1]);
			points.push_back(importedVertices[vertexIndex[2] - 1]);
			normals.push_back(importedNormals[normalIndex[0] - 1]);
			normals.push_back(importedNormals[normalIndex[1] - 1]);
			normals.push_back(importedNormals[normalIndex[2] - 1]);
			colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));

			vertexCount += 3;
		}
	}
	fclose(file);
	return vertexCount;
}

int indexBeforeMetronome;
vec4 cubePosition;
void addMetronomeCube() {
	indexBeforeMetronome = points.size();
	cubePosition = position + direction;
	mat4 scalingMat = generateScaleMatrix(0.75);
	mat4 translationMat = generateTranslationMatrix(cubePosition.x / 2, cubePosition.y / 2 - 3, cubePosition.z / 2);
	mat4 rotationMat = generateRotationMatrix(100, -2, 28);
	point4 metronomeCubeVertices[8] = {
		translationMat*rotationMat*scalingMat*point4(-0.5, -0.5,  0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(-0.5,  0.5,  0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(0.5,  0.5,  0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(0.5, -0.5,  0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(-0.5, -0.5, -0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(-0.5,  0.5, -0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(0.5,  0.5, -0.5, 1.0),
		translationMat*rotationMat*scalingMat*point4(0.5, -0.5, -0.5, 1.0)
	};

	point4 * metronomeCubeVPointer = metronomeCubeVertices;
	cubicInitializer(metronomeCubeVPointer);
}
float scaleMultiplier = 2;
int increasing = 1;

void updateMetronomeCube() {
	//Fareye gore hareketini iyilestirmek icin biseyler yapmak lazim burda
	cubePosition = position + 2 * (position - cubePosition)*direction;
	// 0.75-2 scaling
	if (scaleMultiplier >= 2) {
		increasing = 0;
	}
	else if (scaleMultiplier <= 0.5) {
		increasing = 1;
	}

	//VVVVVVV icin
	if (increasing) {
		scaleMultiplier += 0.001989*deltaTime;
	}
	else {
		scaleMultiplier -= 0.001989*deltaTime;
	}

	//Paced Energy icin
	//if (increasing) {
	//	scaleMultiplier += 0.002675*deltaTime;
	//}
	//else {
	//	scaleMultiplier -= 0.002675*deltaTime;
	//}


	mat4 scalingMat = generateScaleMatrix(scaleMultiplier);
	mat4 translationMat = generateTranslationMatrix(cubePosition.x, cubePosition.y, cubePosition.z);
	mat4 translationMat2 = generateTranslationMatrix(0, -6, 0);
	mat4 rotationMat = generateRotationMatrix(100, -2, 28);
	point4 metronomeCubeVertices[8] = {
		translationMat2*translationMat*rotationMat*scalingMat*point4(-0.5, -0.5,  0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(-0.5,  0.5,  0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(0.5,  0.5,  0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(0.5, -0.5,  0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(-0.5, -0.5, -0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(-0.5,  0.5, -0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(0.5,  0.5, -0.5, 1.0),
		translationMat2*translationMat*rotationMat*scalingMat*point4(0.5, -0.5, -0.5, 1.0)
	};
	/*platformIndex[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = Index;
	platformType[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = type;*/

	point4 * metronomeCubeVPointer = metronomeCubeVertices;
	cubicUpdater(metronomeCubeVPointer, indexBeforeMetronome);
}

int cityIndex[6] = { 0,0,0,0,0,0 };
int cityVerticeCount[6] = { 0,0,0,0,0,0 };
float citiesTranslateXOffset = 15;
float citiesTranslateYOffset = -15;
float citiesTranslateZOffset = 15;
void
SetupBackground() {
	cityIndex[0] = points.size();
	cityVerticeCount[0] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 180, 0),	generateTranslationMatrix(-40.0 + citiesTranslateXOffset,-15.06 + citiesTranslateYOffset , -180.0 + citiesTranslateZOffset));
	cityIndex[1] = points.size();
	cityVerticeCount[1] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 270, 0),	generateTranslationMatrix(40.0 + citiesTranslateXOffset, -15.05 + citiesTranslateYOffset, -40.0 + citiesTranslateZOffset));
	cityIndex[2] = points.size();
	cityVerticeCount[2] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 90, 0),	generateTranslationMatrix(0.0 + citiesTranslateXOffset,	-15.04 + citiesTranslateYOffset, -80.0 + citiesTranslateZOffset));
	cityIndex[3] = points.size();
	cityVerticeCount[3] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 90, 0),	generateTranslationMatrix(-60.0 + citiesTranslateXOffset,-15.03 + citiesTranslateYOffset, -210.0 + citiesTranslateZOffset));
	cityIndex[4] = points.size();
	cityVerticeCount[4] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 270, 0),	generateTranslationMatrix(-20.0 + citiesTranslateXOffset,-15.02 + citiesTranslateYOffset, -120.0 + citiesTranslateZOffset));
	cityIndex[5] = points.size();
	cityVerticeCount[5] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 180, 0),	generateTranslationMatrix(20.0 + citiesTranslateXOffset, -15.01 + citiesTranslateYOffset, -190.0 + citiesTranslateZOffset));

}
/*void
setStartLevel()
{


addPlatformPiece(0, -6, 1);	addPlatformPiece(1, -6, 1);	addPlatformPiece(2, -6, 1);
addPlatformPiece(0, -5, 1);	addPlatformPiece(1, -5, 4);	addPlatformPiece(2, -5, 1);
addPlatformPiece(0, -4, 1);	addPlatformPiece(1, -4, 1);	addPlatformPiece(2, -4, 1);
addPlatformPiece(-5, -3, 1);addPlatformPiece(-4, -3, 1); addPlatformPiece(-3, -3, 1);																						addPlatformPiece(1, -3, 1);
addPlatformPiece(-5, -2, 1);addPlatformPiece(-4, -2, 3); addPlatformPiece(-3, -2, 1); addPlatformPiece(-2, -2, 1);	addPlatformPiece(-1, -2, 1);							addPlatformPiece(1, -2, 1);
addPlatformPiece(-5, -1, 1);addPlatformPiece(-4, -1, 1); addPlatformPiece(-3, -1, 1);								addPlatformPiece(-1, -1, 1); addPlatformPiece(0, -1, 1);addPlatformPiece(1, -1, 1);	addPlatformPiece(2, -1, 1);	addPlatformPiece(3, -1, 1);
addPlatformPiece(-1, 0, 1);	 addPlatformPiece(0, 0, 1);	addPlatformPiece(1, 0, 1);	addPlatformPiece(2, 0, 2);	addPlatformPiece(3, 0, 1);
addPlatformPiece(-1, 1, 1);	 addPlatformPiece(0, 1, 1);	addPlatformPiece(1, 1, 1);	addPlatformPiece(2, 1, 1);	addPlatformPiece(3, 1, 1);
}*/
GLuint program;
void
updateBuffers() {
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + normals.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4), normals.size() * sizeof(vec3), &normals[0]);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4) + colors.size() * sizeof(vec4)));
}

vec4 surfaceVertices[4] = { generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(-0.5,  0.0,  0.5, 1.0), 
							generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(0.5,  0.0,  0.5, 1.0), 
							generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(-0.5,  0.0, -0.5, 1.0),
							generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(0.5,  0.0, -0.5, 1.0) 
};

int bb8Index;
int bb8VCount;
int surfaceBeforeIndex;
// OpenGL initialization
void
init()
{
	srand(static_cast <unsigned> (glutGet(GLUT_ELAPSED_TIME)));
	point4 * cubeVPointer = cubeVertices;
	cubicInitializer(cubeVPointer);
	//Fill background surface
	surfaceBeforeIndex = points.size();
	quadInitializer(surfaceVertices, 3, 2, 0, 1);

	playerCubeIndex = 0;
	if (!engine)printf("could not start engine"); // 
												  //setStartLevel();
	importLevel("level1.txt");
	addMetronomeCube();
	bb8Index = points.size();
	bb8VCount = importFromOBJ("Stormtrooper.obj", generateScaleMatrix(0.75), generateRotationMatrix(0, -25, 0), generateTranslationMatrix(-4.0, -0.65, -6.5));
	SetupBackground();

	engine->play2D("presenting_vvvvvv.mp3", true);


	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + normals.size() * sizeof(vec3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4), normals.size() * sizeof(vec3), &normals[0]);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4) + colors.size() * sizeof(vec4)));

	initializeUniformVariables(program);

	glEnable(GL_DEPTH_TEST); glShadeModel(GL_FLAT);
	glClearColor(0.85546875, 0.96484375, 0.92578125, 1.0);

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
	eye_position = glGetUniformLocation(program, "eye_position");
	toon_enable = glGetUniformLocation(program, "toonEnable");
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
	if (freecamToggle == true)
	{
		horizontalAngle += mouseSpeed * deltaTime * 0.01 *float(1280 / 2 - mouseX);
		verticalAngle += mouseSpeed * deltaTime * 0.01 *float(720 / 2 - mouseY);
		if (verticalAngle > 89.0f*radianConstant)
			verticalAngle = 89.0f*radianConstant;
		if (verticalAngle < -89.0f*radianConstant)
			verticalAngle = -89.0f*radianConstant;
	}

	if (x != 1280 / 2 || y != 720 / 2)
		glutWarpPointer(1280 / 2, 720 / 2);

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

bool
CheckIfPlatformsAreLeveled()
{
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			if (platformType[i][j] != 0)
			{
				if (platformLeveled[i][j] == 0 || platformLeveled[i][j] == 2)
				{
					return false;
				}
			}
		}
	}
	return true;
}
void
CleanRemovedPlatformTypes()
{
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			if (platformType[i][j] != 0)
			{
				if (platformLeveled[i][j] == 1)
				{
					platformType[i][j] = 0;
				}
			}
		}
	}
}

void
LevitatePlatform(int i, int j)
{
	if (platformLeveled[i][j] == 1)
	{
		return;
	}
	float levitateUpperLimit = (-0.8);
	if (platformLeveled[i][j] == 2)
	{
		levitateUpperLimit = (10.8);
	}
	if (platformLeveled[i][j] == 0)
	{
		levitateUpperLimit = (-0.8);
	}
	float speed = platformLevitationSpeed[i][j];
	float levitationDiff = speed * deltaTime / 20;
	int platformBufferIndex = platformIndex[i][j];
	mat4 platformTranslationMatrix;
	if ((points[platformBufferIndex + 1].y + levitationDiff) >= levitateUpperLimit)
	{
		float offset = levitateUpperLimit - (points[platformBufferIndex + 1].y + levitationDiff);
		levitationDiff += offset;
		platformTranslationMatrix = generateTranslationMatrix(0.0, levitationDiff, 0.0);
		platformLeveled[i][j] = 1;
	}
	else
	{
		platformTranslationMatrix = generateTranslationMatrix(0.0, levitationDiff, 0.0);
	}
	point4 platformPieceVertices[8] = {
		points[platformBufferIndex + 1],
		points[platformBufferIndex + 3],
		points[platformBufferIndex + 5],
		points[platformBufferIndex + 7],
		points[platformBufferIndex + 14],
		points[platformBufferIndex + 19],
		points[platformBufferIndex + 11],
		points[platformBufferIndex + 10]
	};
	for (int i = 0; i < 8; i++)
	{
		platformPieceVertices[i] = platformTranslationMatrix * platformPieceVertices[i];
	}

	point4 * cubeVPointer = platformPieceVertices;
	cubicUpdater(cubeVPointer, platformIndex[i][j]);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4), normals.size() * sizeof(vec3), &normals[0]);
}
void
drawPlatforms()
{
	updateLightProperties(color4(1.0, 1.0, 1.0, 1.0));
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			switch (platformType[i][j]) {
			case 0:  break;
			case 1: LevitatePlatform(i, j); updateLightProperties(color4(1.0, 1.0, 1.0, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; // normal platform - white
			case 2: LevitatePlatform(i, j); updateLightProperties(color4(0.0, 1.0, 0.0, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; // start - green
			case 3: LevitatePlatform(i, j); updateLightProperties(color4(1.0, 0.0, 0.0, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; // exit - red
			case 4: LevitatePlatform(i, j); updateLightProperties(color4(0.0, 1.0, 1.0, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //color changer - cyan
			case 5: LevitatePlatform(i, j); updateLightProperties(color4(0.8, 0.8, 0.8, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //normal platform -grey
			case 6: LevitatePlatform(i, j); updateLightProperties(color4(0.984, 0.855, 0.140, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //credits button -yellow
			}
		}
	}
}

vec4 metronomeCubeColor = vec4(0.0, 0.0, 0.0, 1.0);

void
drawMetronomeCube()
{
	updateLightProperties(metronomeCubeColor);
	glDrawArrays(GL_TRIANGLES, indexBeforeMetronome, 36);
}

void
MoveCube()
{
	mat4 playerCubeRotationMatrix;
	mat4 playerCubeTranslationMatrix;
	mat4 playerCubeReverseTranslationMatrix;
	if (playerCubeMoveDirection == 'R' || playerCubeMoveDirection == 'L' || playerCubeMoveDirection == 'U' || playerCubeMoveDirection == 'D')
	{
		int angleToRotate = (cubeRotationSpeed * deltaTime / 10);

		if (angleToRotate + rotatedAngle > 90)
		{
			angleToRotate = 90 - rotatedAngle;
		}

		switch (playerCubeMoveDirection) {
		case 'U': playerCubeRotationMatrix = generateRotationMatrix(angleToRotate, 0, 0); playerCubeTranslationMatrix = generateTranslationMatrix(0, -0.5, -0.5 + (playerCubePos.z)); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0, 0.5, 0.5 - (playerCubePos.z)); break;
		case 'D': playerCubeRotationMatrix = generateRotationMatrix(-angleToRotate, 0, 0);  playerCubeTranslationMatrix = generateTranslationMatrix(0, -0.5, 0.5 + (playerCubePos.z)); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0, 0.5, -0.5 - (playerCubePos.z)); break;
		case 'R': playerCubeRotationMatrix = generateRotationMatrix(0, 0, -angleToRotate);  playerCubeTranslationMatrix = generateTranslationMatrix(0.5 + (playerCubePos.x), -0.5, 0); playerCubeReverseTranslationMatrix = generateTranslationMatrix(-0.5 - (playerCubePos.x), 0.5, 0); break;
		case 'L': playerCubeRotationMatrix = generateRotationMatrix(0, 0, angleToRotate); playerCubeTranslationMatrix = generateTranslationMatrix(-0.5 + (playerCubePos.x), -0.5, 0); playerCubeReverseTranslationMatrix = generateTranslationMatrix(0.5 - (playerCubePos.x), 0.5, 0); break;
		}

		for (int i = 0; i < 8; i++)
		{
			cubeVertices[i] = playerCubeTranslationMatrix * playerCubeRotationMatrix * playerCubeReverseTranslationMatrix * cubeVertices[i];
		}

		point4 * cubeVPointer = cubeVertices;

		cubicUpdater(cubeVPointer, playerCubeIndex);


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
	mat4 playerCubeTranslationMatrix = generateTranslationMatrix(0.0, -1.0* deltaTime / 20, 0.0);

	for (int i = 0; i < 8; i++)
	{
		cubeVertices[i] = playerCubeTranslationMatrix * cubeVertices[i];
	}

	point4 * cubeVPointer = cubeVertices;
	cubicUpdater(cubeVPointer, playerCubeIndex);



	playerCubeMoveDirection = 'n';

}
void
Respawn()
{
	std::copy(defaultCubeVertices, defaultCubeVertices + 8, cubeVertices);
	point4 * cubeVPointer = cubeVertices;
	cubicUpdater(cubeVPointer, playerCubeIndex);
	playerCubePos = vec3(0, 0, 0);
	currentCubeColor = color4(1.0, 0.0, 0.3984375, 1.0);
	playerCubeMoveDirection = 'n';
	//Camera reset
	position = initialPosition;
	verticalAngle = initialVerticalAngle;
	horizontalAngle = initialHorizontalAngle;
	//Metronome cube reset
	metronomeCubeColor = vec4(0.0, 0.0, 0.0, 1.0);
}
float
OscillateColorElement(float &colorElement, bool &colorTicker) {
	float colorRandom = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*deltaTime / 850;
	if (colorElement + colorRandom > 1)
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
RandomizeColor(color4 &color) {
	color.x = OscillateColorElement(color.x, rTicker);
	color.y = OscillateColorElement(color.y, gTicker);
	color.z = OscillateColorElement(color.z, bTicker);
}
bool creditsInitialized = false;
void
InitializeCredits()
{
	if (creditsInitialized == false)
	{


		addPlatformPiece(4, 0, 1); addPlatformPiece(5, 0, 1); addPlatformPiece(6, 0, 1); addPlatformPiece(7, 0, 1); addPlatformPiece(8, 0, 1); addPlatformPiece(9, 0, 1); addPlatformPiece(10, 0, 1);	addPlatformPiece(11, 0, 1);
		addPlatformPiece(11, 1, 1);
		addPlatformPiece(11, 2, 1);
		addPlatformPiece(11, 3, 1);
		addPlatformPiece(11, 4, 1);
		addPlatformPiece(11, 5, 1);
		addPlatformPiece(11, 6, 1);
		addPlatformPiece(11, 7, 1);
		addPlatformPiece(9, 8, 5);	addPlatformPiece(10, 8, 1);		addPlatformPiece(11, 8, 5);		addPlatformPiece(12, 8, 1);		addPlatformPiece(13, 8, 5);
		addPlatformPiece(9, 9, 1);	addPlatformPiece(10, 9, 5);		addPlatformPiece(11, 9, 1);		addPlatformPiece(12, 9, 5);		addPlatformPiece(13, 9, 1);
		addPlatformPiece(9, 10, 5); addPlatformPiece(10, 10, 1);	addPlatformPiece(11, 10, 5);	addPlatformPiece(12, 10, 1);	addPlatformPiece(13, 10, 5);
		addPlatformPiece(9, 11, 1); addPlatformPiece(10, 11, 5);	addPlatformPiece(11, 11, 1);	addPlatformPiece(12, 11, 5);	addPlatformPiece(13, 11, 1);
		addPlatformPiece(9, 12, 5); addPlatformPiece(10, 12, 1);	addPlatformPiece(11, 12, 5);	addPlatformPiece(12, 12, 1);	addPlatformPiece(13, 12, 5);
		addPlatformPiece(9, 13, 1); addPlatformPiece(10, 13, 5);	addPlatformPiece(11, 13, 1);	addPlatformPiece(12, 13, 5);	addPlatformPiece(13, 13, 1);
		addPlatformPiece(9, 14, 5); addPlatformPiece(10, 14, 1);	addPlatformPiece(11, 14, 5);	addPlatformPiece(12, 14, 1);	addPlatformPiece(13, 14, 5);
		addPlatformPiece(9, 15, 1); addPlatformPiece(10, 15, 5);	addPlatformPiece(11, 15, 1);	addPlatformPiece(12, 15, 5);	addPlatformPiece(13, 15, 1);
		addPlatformPiece(9, 16, 5); addPlatformPiece(10, 16, 1);	addPlatformPiece(11, 16, 5);	addPlatformPiece(12, 16, 1);	addPlatformPiece(13, 16, 5);
		addPlatformPiece(9, 17, 1); addPlatformPiece(10, 17, 5);	addPlatformPiece(11, 17, 1);	addPlatformPiece(12, 17, 5);	addPlatformPiece(13, 17, 1);
		addPlatformPiece(9, 18, 5); addPlatformPiece(10, 18, 1);	addPlatformPiece(11, 18, 5);	addPlatformPiece(12, 18, 1);	addPlatformPiece(13, 18, 5);
		addPlatformPiece(9, 19, 1); addPlatformPiece(10, 19, 5);	addPlatformPiece(11, 19, 1);	addPlatformPiece(12, 19, 5);	addPlatformPiece(13, 19, 1);





		updateBuffers();
		creditsInitialized = true;
	}

}

int level1InitializeState = 0;
void
InitializeLevel1()
{
	if (level1InitializeState == 0)
	{
		RemoveAllPlatforms();
		level1InitializeState = 1;
	}
	if (level1InitializeState == 1)
	{
		if (CheckIfPlatformsAreLeveled())
		{
			CleanRemovedPlatformTypes();
			platformType[2 + sceneSize / 2][0 + sceneSize / 2] = 1;

			addPlatformPiece(-2, -3, 1);	addPlatformPiece(-1, -3, 1); addPlatformPiece(0, -3, 1); addPlatformPiece(1, -3, 1); addPlatformPiece(2, -3, 1);	addPlatformPiece(3, -3, 1); addPlatformPiece(4, -3, 1); addPlatformPiece(5, -3, 1); addPlatformPiece(6, -3, 1);
			addPlatformPiece(-2, -2, 1);	addPlatformPiece(-1, -2, 1); addPlatformPiece(0, -2, 1); addPlatformPiece(1, -2, 1); addPlatformPiece(2, -2, 1);	addPlatformPiece(3, -2, 1); addPlatformPiece(4, -2, 1); addPlatformPiece(5, -2, 1); addPlatformPiece(6, -2, 1);
			addPlatformPiece(-1, -1, 1); addPlatformPiece(0, -1, 1); addPlatformPiece(1, -1, 1); addPlatformPiece(2, -1, 1);	addPlatformPiece(3, -1, 1); addPlatformPiece(4, -1, 1); addPlatformPiece(5, -1, 1);
			addPlatformPiece(-1, 0, 1);	addPlatformPiece(0, 0, 1);	addPlatformPiece(1, 0, 1);/*addPlatformPiece(2, 0, 1);*/addPlatformPiece(3, 0, 1);	addPlatformPiece(4, 0, 1);  addPlatformPiece(5, 0, 1);
			addPlatformPiece(-1, 1, 1);	addPlatformPiece(0, 1, 1);	addPlatformPiece(1, 1, 1); addPlatformPiece(2, 1, 1);	addPlatformPiece(3, 1, 1);	addPlatformPiece(4, 1, 1);  addPlatformPiece(5, 1, 1);

			level1InitializeState = 2;
			updateBuffers();
		}
		return;
	}
}
void
CalculateDeltaTime()
{
	int oldTimeSinceStart = timeSinceStart;
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (timeSinceStart - oldTimeSinceStart);
	oldTimeSinceStart = timeSinceStart;
}

bool turnComplete = false; float anglesTurned = 0; bool turnTicker = false;
void
TurnCameraRight()
{
	if (turnTicker == false)
	{
		if (turnComplete == false)
		{
			playerMovementLockToggle = true;
			horizontalAngle -= 0.001*deltaTime;
			anglesTurned += 0.001*deltaTime;
			if (anglesTurned >= 1.33)
			{
				playerMovementLockToggle = false;
				turnComplete = true;
				anglesTurned = 0;
				turnTicker = !turnTicker;
			}
		}
	}
	else
	{
		if (turnComplete == false)
		{
			playerMovementLockToggle = true;
			horizontalAngle += 0.001*deltaTime;
			anglesTurned += 0.001*deltaTime;
			if (anglesTurned >= 1.33)
			{
				playerMovementLockToggle = false;
				turnComplete = true;
				anglesTurned = 0;
				turnTicker = !turnTicker;

			}
		}
	}
}


void
DecideCurrentPlatformAction()
{
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 2)
	{
		InitializeLevel1();
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 6)
	{
		InitializeCredits();
	}
	if (playerCubePos.x == 11 && playerCubePos.z == 5)
	{
		TurnCameraRight();
	}
	if (!(playerCubePos.x == 11 && playerCubePos.z == 5))
	{
		turnComplete = false;
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 4)
	{
		RandomizeColor(currentCubeColor);
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 3)
	{
		exit(EXIT_SUCCESS);
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 0)
	{
		DropCube();
	}
	else
	{
		MoveCube();
	}
}
//----------------------------------------------------------------------------
void
display(void)
{

	CalculateDeltaTime();

	DecideCurrentPlatformAction();

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
	glUniform3fv(eye_position, 1, position);
	glUniform1i(toon_enable, toonEnable);
	/*mat4  p = Ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, zNear, zFar);*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);


	mat4 r = generateRotationMatrix(0, 0, 0);
	glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, r);
	mat4 t = generateTranslationMatrix(0, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	//Draw background surface
	glDrawArrays(GL_TRIANGLES, surfaceBeforeIndex, 6);
	drawPlatforms();
	/*updateMetronomeCube();
	drawMetronomeCube();*/
	RandomizeColor(bb8Color);
	updateLightProperties(bb8Color);
	glDrawArrays(GL_TRIANGLES, bb8Index, bb8VCount);
	

	updateLightProperties(vec4(0.046875, 0.14453125, 0.22265625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[0], cityVerticeCount[0]);
	updateLightProperties(vec4(0.24609375, 0.35546875, 0.44140625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[1], cityVerticeCount[1]);
	updateLightProperties(vec4(0.39453125, 0.51953125, 0.609375, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[2], cityVerticeCount[2]);
	updateLightProperties(vec4(0.59765625, 0.7265625, 0.80078125, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[3], cityVerticeCount[3]);
	updateLightProperties(vec4(0.73828125, 0.84375, 0.91015625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[4], cityVerticeCount[4]);
	updateLightProperties(vec4(0.73828125, 0.84375, 0.91015625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[5], cityVerticeCount[5]);

	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4), normals.size() * sizeof(vec3), &normals[0]);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}
//----------------------------------------------------------------------------


int metronomeCubeHealth() {
	//Return 0 means 0 health, return 1 means wrong button press, return 2 means correct button press
	if (scaleMultiplier < 1.60) {
		//Wrong button press
		if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y > 0.99 && metronomeCubeColor.y <= 1.0) {
			//Full health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor.y = 1.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x, metronomeCubeColor.y - 0.2, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y > 0.01 && metronomeCubeColor.y < 0.99) {
			//Above half health but not full health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x, metronomeCubeColor.y - 0.2, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Half health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor.y = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x + 0.2, metronomeCubeColor.y, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x > 0.01 && metronomeCubeColor.x < 0.99 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Below half health but not zero health
			metronomeCubeColor.y = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x + 0.2, metronomeCubeColor.y, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x > 0.99 && metronomeCubeColor.x <= 1.00 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Zero health
			Respawn();
			return 0;
		}
		return 2;
	}
	else if (scaleMultiplier >= 1.60) {
		//Correct button press
		if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y > 0.99 && metronomeCubeColor.y <= 1.0) {
			//Full health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor.y = 1.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x, metronomeCubeColor.y, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y > 0.01 && metronomeCubeColor.y < 0.99) {
			//Above half health but not full health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x, metronomeCubeColor.y + 0.2, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x >= 0.0 && metronomeCubeColor.x < 0.01 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Half health
			metronomeCubeColor.x = 0.0;
			metronomeCubeColor.y = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x, metronomeCubeColor.y + 0.2, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x > 0.01 && metronomeCubeColor.x < 0.99 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Below half health but not zero health
			metronomeCubeColor.y = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x - 0.2, metronomeCubeColor.y, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		else if (metronomeCubeColor.x > 0.99 && metronomeCubeColor.x <= 1.00 && metronomeCubeColor.y >= 0.0 && metronomeCubeColor.y < 0.01) {
			//Zero health
			metronomeCubeColor.x = 1.0;
			metronomeCubeColor.y = 0.0;
			metronomeCubeColor = vec4(metronomeCubeColor.x - 0.2, metronomeCubeColor.y, metronomeCubeColor.z, metronomeCubeColor.w);
		}
		return 2;
	}
}

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q': engine->drop(); // delete engine
		exit(EXIT_SUCCESS);
		break;
	case 'w': case 'W': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'U'; if (turnTicker == true)playerCubeMoveDirection = 'R'; } break;
	case 'a': case 'A': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'L'; if (turnTicker == true)playerCubeMoveDirection = 'U'; } break;
	case 's': case 'S': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'D'; if (turnTicker == true)playerCubeMoveDirection = 'L'; } break;
	case 'd': case 'D': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'R'; if (turnTicker == true)playerCubeMoveDirection = 'D'; } break;
	case 'f': case 'F': freecamToggle = !freecamToggle; break;
	case 'r': case 'R': Respawn(); break;
	case 'c': case 'C': position = initialPosition; verticalAngle = initialVerticalAngle; horizontalAngle = initialHorizontalAngle; break;
	case 't': case 'T': if (toonEnable == 0) { toonEnable = 1; }
			  else if (toonEnable == 1) { toonEnable = 2; }
			  else if (toonEnable == 2) { toonEnable = 0; } break;
	case 'p': printf("positionX: %f, positionY: %f,positionZ: %f,horizontalAngle:%f\n verticalAngle:%f, zNear:%f, zFar:%f, radius:%f, theta : %f, phi:%f\n", position.x, position.y, position.z, horizontalAngle, verticalAngle, position.y, position.z, radius, theta, phi); break;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------
void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: if (freecamToggle == true)position += direction * speed; break;
	case GLUT_KEY_DOWN: if (freecamToggle == true)position -= direction * speed; break;
	case GLUT_KEY_RIGHT: if (freecamToggle == true)position += right_vector * speed; break;
	case GLUT_KEY_LEFT: if (freecamToggle == true)position -= right_vector * speed; break;
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