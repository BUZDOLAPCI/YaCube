#include <iostream>														
#include <sstream>  													
#include <string>														
#include <fstream>														
#include "glew.h"														
#include "glui.h"														
#include "freeglut.h"													
#include "Angel.h"							
#include "OpenGL\lib\glm\glm.hpp"			
#include "AntTweakBar.h"
#include "irrKlang\irrKlang.h"
#include "SOIL.h" // Library for Image Loading - Texture
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
int rotatedAngle = 0;																	//							|		|															 |
float cubeRotationSpeed = 7; //default 5	
float highlightPathSpeed = 0.5;
float highlightLevitationSpeed = 0.0009 * highlightPathSpeed;
float pathHighlightDelay = 250 / highlightPathSpeed;
//							|       |      \/           \/        \/          \/     \/          |
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
irrklang::ISoundEffectControl* fx = 0;
irrklang::ISound* vvvvvv;
irrklang::ISound* menuMusic;
const int NumVertices = 36 * 2; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int timeSinceStart; int deltaTime;
void initializeUniformVariables(GLuint program);
mat4 generateTranslationMatrix(GLfloat x, GLfloat y, GLfloat z);
mat4 generateRotationMatrix(int x, int y, int z);
int metronomeCubeHealth();
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
bool mouseLocker = true;
bool trooperExist = 0;

float light_multiplier = 1.0f;
float light_multiplier2 = 1.0f;
float temp_light_multiplier;
float light_info[4] = { 1.31, -11.52, -14.69,0.0 };
float light_ambient_info[3] = { 0.2, 0.2, 0.2 };
float light_specular_info[3] = { 1.0, 1.0, 1.0 };
float light_diffuse_info[3] = { 1.0, 1.0, 1.0 };

float trooper_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float logo_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float score_Rotation[] = { -0.47f, 0.0f, 0.04f, 1.0f };

int new_platform_x = 1;
int new_platform_y = 1;

//texture trial
GLuint textured;
vector<GLfloat> texCoords = { //WIP
	256.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 256.0f,
	256.0f, 0.0f,
	0.0f, 0.0f,
	256.0f, 256.0f
};

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
color4 platformHighlightColor[sceneSize][sceneSize];
int platformHiglightTicker[sceneSize][sceneSize];
bool actionAlreadyDone = false;
bool gotInput = false;
float fScore;
int iScore;

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
GLfloat  zNear = 0.170417, zFar = 1000.022503;

GLuint  projection; // projection matrix uniform shader variable location

					//----------------------------------------------------------------------------

					// quadInitializer generates two triangles for each face and assigns colors
					//    to the vertices
GLuint ambientProduct;
GLuint diffuseProduct;
GLuint specularProduct;
GLuint lightPosition;
GLuint ambientProduct_2;
GLuint diffuseProduct_2;
GLuint specularProduct_2;
GLuint lightPosition_2;
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
	platformHiglightTicker[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 0;

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
	platformHiglightTicker[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 0;
}
void
RemovePlatformPieceCompletely(int gameGridX, int gameGridY)
{
	platformLeveled[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 2;
	platformType[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 0;
	platformHiglightTicker[gameGridX + sceneSize / 2][gameGridY + sceneSize / 2] = 0;
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
					RemovePlatformPieceCompletely(i - (sceneSize / 2), j - (sceneSize / 2));
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

GLint importFromOBJ(const char* filename, mat4 scaleMatrix, mat4 rotationMatrix, mat4 TranslationMatrix, bool hasTexture) {
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
		float a, b, c;
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
			//vec4 temp = rotationMatrix*vec4(normal, 1.0);
			//importedNormals.push_back(vec3(temp.x, temp.y, temp.z));
			importedNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			int vertexIndex[3], uvIndex[3], normalIndex[3];
			if (hasTexture) {
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					return -1;
				}
			}
			else {
				int matches2 = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches2 != 6) {
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					return -1;
				}
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
float scaleMultiplier = 2.2;
int increasing = 1;
int lastPlayPosition = 500; //time of the starting beat of the song in ms
point4 centerOfPlayerCube;
int lastLegitInputTime = lastPlayPosition;
bool beat = false;
int bpm = 100; //bpm of the song
bool highlight3StepsAhead = false;
void HighlightingPathAccordingtoMove();
bool playerFollowingPath = false;
int bpmSegmentsState = 0;
bool musicIntro = false;
bool musicStarted = false;
bool SongFinished = false;
void stopMusic();
void SongFinishedSequence();
void updateMetronomeCube() {
	//Fareye gore hareketini iyilestirmek icin biseyler yapmak lazim burda
	cubePosition = position + 2 * (position - cubePosition)*direction;

	if (musicIntro) {
		if (vvvvvv->getPlayPosition() >= 17300) {  // 0xx00 speed up at 20.00 second
			bpm = 20;
			if (bpmSegmentsState == 0)
			{
				highlight3StepsAhead = false; playerFollowingPath = false;
				bpmSegmentsState = 1;
				HighlightingPathAccordingtoMove();
			}

		}

		/*if (vvvvvv->getPlayPosition() >= 19000) {

		}*/
		if (vvvvvv->getPlayPosition() >= 19700) {  // 0xx00 speed up at 20.00 second
			bpm = 200;
			cubeRotationSpeed = 10;
			if (bpmSegmentsState == 1)
			{
				highlight3StepsAhead = true;
				bpmSegmentsState = 2;
			}
		}

		if (vvvvvv->getPlayPosition() >= 38600) {  // 0xx00 speed up at 20.00 second
			bpm = 100;
			cubeRotationSpeed = 7;
			if (bpmSegmentsState == 2)
			{
				highlight3StepsAhead = false;
				bpmSegmentsState = 3;
				HighlightingPathAccordingtoMove();
			}

		}

		if (vvvvvv->getPlayPosition() >= 86900) {  // 0xx00 speed up at 20.00 second
			bpm = 200;
			cubeRotationSpeed = 10;
			if (bpmSegmentsState == 3)
			{
				highlight3StepsAhead = true;
				bpmSegmentsState = 4;
			}
		}

		if (vvvvvv->getPlayPosition() >= 105900) {  // 0xx00 speed up at 20.00 second
			bpm = 100;
			cubeRotationSpeed = 7;
			if (bpmSegmentsState == 5)
			{
				highlight3StepsAhead = false;
				bpmSegmentsState = 6;
				HighlightingPathAccordingtoMove();
			}
		}

		if (vvvvvv->getPlayPosition() >= 105900 + 600 * 64) {  // 0xx00 speed up at 20.00 second
			bpm = 200;
			cubeRotationSpeed = 10;
			if (bpmSegmentsState == 6)
			{
				highlight3StepsAhead = true;
				bpmSegmentsState = 7;
			}
		}

		int msPerBeat = 60000 / bpm;

		//To loop the song
		if (vvvvvv->getPlayLength() <= vvvvvv->getPlayPosition() + msPerBeat * 3) {
			stopMusic();
			SongFinishedSequence();
			
		}

		if (vvvvvv->getPlayPosition() - lastLegitInputTime > msPerBeat * 3 / 2) {
			//Missed beat
			bool temp = beat;
			beat = false;
			metronomeCubeHealth();
			beat = temp;
			lastLegitInputTime = vvvvvv->getPlayPosition();
		}

		if (vvvvvv->getPlayPosition() - lastPlayPosition >= msPerBeat * 3 / 4 && vvvvvv->getPlayPosition() - lastPlayPosition < msPerBeat * 5 / 4) {
			beat = true;
			if (gotInput) {
				lastLegitInputTime = vvvvvv->getPlayPosition();
			}
			gotInput = false;
		}
		else if (vvvvvv->getPlayPosition() - lastPlayPosition > msPerBeat * 5 / 4) {
			beat = false;
			lastPlayPosition = vvvvvv->getPlayPosition() - ((vvvvvv->getPlayPosition() - lastPlayPosition) - msPerBeat);
		}

		if (beat) {
			scaleMultiplier = 2.0;
		}
		else {
			scaleMultiplier -= 0.005*deltaTime;
		}
	}

	mat4 scalingMat = generateScaleMatrix(scaleMultiplier);
	mat4 translationMat = generateTranslationMatrix(cubePosition.x, cubePosition.y, cubePosition.z);
	mat4 translationMat2 = generateTranslationMatrix(0, -6, 3);
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

	point4 * metronomeCubeVPointer = metronomeCubeVertices;
	cubicUpdater(metronomeCubeVPointer, indexBeforeMetronome);
}

int surfaceBeforeIndex;
vec4 surfaceVertices[4] = { generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(-0.5,  0.0,  0.5, 1.0),
generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(0.5,  0.0,  0.5, 1.0),
generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(-0.5,  0.0, -0.5, 1.0),
generateTranslationMatrix(0,-30,0)*generateRotationMatrix(0,20,0)*generateScaleMatrix(1000.0)*point4(0.5,  0.0, -0.5, 1.0)
};
int cityIndex[6] = { 0,0,0,0,0,0 };
int cityVerticeCount[6] = { 0,0,0,0,0,0 };
float citiesTranslateXOffset = 15;
float citiesTranslateYOffset = -15;
float citiesTranslateZOffset = 15;
int numbersIndex[10], numbersCount[10];
void
SetupBackground() {
	//Fill background surface
	surfaceBeforeIndex = points.size();
	quadInitializer(surfaceVertices, 3, 2, 0, 1);

	cityIndex[0] = points.size();
	cityVerticeCount[0] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 180, 0), generateTranslationMatrix(-40.0 + citiesTranslateXOffset, -15.06 + citiesTranslateYOffset, -180.0 + citiesTranslateZOffset), true);
	cityIndex[1] = points.size();
	cityVerticeCount[1] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 270, 0), generateTranslationMatrix(40.0 + citiesTranslateXOffset, -15.05 + citiesTranslateYOffset, -40.0 + citiesTranslateZOffset), true);
	cityIndex[2] = points.size();
	cityVerticeCount[2] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 90, 0), generateTranslationMatrix(0.0 + citiesTranslateXOffset, -15.04 + citiesTranslateYOffset, -80.0 + citiesTranslateZOffset), true);
	cityIndex[3] = points.size();
	cityVerticeCount[3] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 90, 0), generateTranslationMatrix(-60.0 + citiesTranslateXOffset, -15.03 + citiesTranslateYOffset, -210.0 + citiesTranslateZOffset), true);
	cityIndex[4] = points.size();
	cityVerticeCount[4] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 270, 0), generateTranslationMatrix(-20.0 + citiesTranslateXOffset, -15.02 + citiesTranslateYOffset, -120.0 + citiesTranslateZOffset), true);
	cityIndex[5] = points.size();
	cityVerticeCount[5] += importFromOBJ("TheCity.obj", generateScaleMatrix(0.05), generateRotationMatrix(0, 180, 0), generateTranslationMatrix(20.0 + citiesTranslateXOffset, -15.01 + citiesTranslateYOffset, -190.0 + citiesTranslateZOffset), true);

	numbersIndex[0] = points.size();
	numbersCount[0] = importFromOBJ("0.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[1] = points.size();
	numbersCount[1] = importFromOBJ("1.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[2] = points.size();
	numbersCount[2] = importFromOBJ("2.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[3] = points.size();
	numbersCount[3] = importFromOBJ("3.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[4] = points.size();
	numbersCount[4] = importFromOBJ("4.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[5] = points.size();
	numbersCount[5] = importFromOBJ("5.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[6] = points.size();
	numbersCount[6] = importFromOBJ("6.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[7] = points.size();
	numbersCount[7] = importFromOBJ("7.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[8] = points.size();
	numbersCount[8] = importFromOBJ("8.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
	numbersIndex[9] = points.size();
	numbersCount[9] = importFromOBJ("9.obj", generateScaleMatrix(1), generateRotationMatrix(0, 0, 0), generateTranslationMatrix(0, 0, 0), false);
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
	//texture trial
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + normals.size() * sizeof(vec3), texCoords.size() * sizeof(GLfloat), &texCoords[0]);

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

	//texture trial
	GLint texAttrib = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + texCoords.size() * sizeof(GLfloat)));
}



int bb8Index;
int bb8VCount;
int logoIndex;
int logoCount;

bool drawLogo = true;

void startMusic() {
	if (musicIntro == false)
	{
		fScore = 0; // lastchange
		if (vvvvvv == NULL) {
			vvvvvv = engine->play2D("00xx00.mp3", false, false, true, irrklang::ESM_AUTO_DETECT, false);
			vvvvvv->setPlayPosition(500);
			musicIntro = true;
		}
		else {
			vvvvvv->setIsPaused(false);
			musicIntro = true;
		}
	}
}

void stopMusic() {
	if (musicIntro == true)
	{
		if (menuMusic->getIsPaused()) {
			vvvvvv->setIsPaused(true);
			vvvvvv->setPlayPosition(500);
			menuMusic->setPlayPosition(0);
			menuMusic->setIsPaused(false);
			musicIntro = false;
			musicStarted = false;
		}
	}
}

bool menuPlaying = false;
void startMenuMusic() {
	if (menuPlaying == false)
	{
		menuMusic = engine->play2D("tall_ships.mp3", true, false, true, irrklang::ESM_AUTO_DETECT, false);
		menuMusic->setVolume(0.5);
		menuPlaying = true;
	}
}

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
	startMenuMusic();
	addPlatformPiece(0, 0, 1);
	addMetronomeCube();
	bb8Index = points.size();
	bb8VCount = importFromOBJ("Stormtrooper.obj", generateScaleMatrix(1), generateRotationMatrix(0, -25, 0), generateTranslationMatrix(10, 0, 0), true);
	logoIndex = points.size();
	logoCount = importFromOBJ("yacube_logo.obj", generateScaleMatrix(60), generateRotationMatrix(-90, 0, 0), generateTranslationMatrix(110.0, 0.0, -350.0), false);
	SetupBackground();

	//texture trial code
	GLuint texture;
	glGenTextures(1, &texture);

	int width = 0, height = 0;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	image = SOIL_load_image("256.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(program, "texKitten"), 0);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//texture trial end

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
	//texture trial
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + normals.size() * sizeof(vec3), texCoords.size() * sizeof(GLfloat), &texCoords[0]);

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

	//texture trial
	GLint texAttrib = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points.size() * sizeof(vec4) + colors.size() * sizeof(vec4) + texCoords.size() * sizeof(GLfloat)));

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
	ambientProduct_2 = glGetUniformLocation(program, "AmbientProduct2");
	diffuseProduct_2 = glGetUniformLocation(program, "DiffuseProduct2");
	specularProduct_2 = glGetUniformLocation(program, "SpecularProduct2");
	lightPosition_2 = glGetUniformLocation(program, "LightPosition2");
	shininess = glGetUniformLocation(program, "Shininess");
	model_view = glGetUniformLocation(program, "model_view");
	normal_matrix = glGetUniformLocation(program, "normal_matrix");
	projection = glGetUniformLocation(program, "projection");
	trs_matrix = glGetUniformLocation(program, "trs_matrix");
	eye_position = glGetUniformLocation(program, "eye_position");
	toon_enable = glGetUniformLocation(program, "toonEnable");
	//texture trial
	textured = glGetUniformLocation(program, "textured");
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
	if (!TwEventMouseMotionGLUT(mouseX, mouseY))  // send event to AntTweakBar
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
		if (mouseLocker) {
			if (x != 1280 / 2 || y != 720 / 2)
				glutWarpPointer(1280 / 2, 720 / 2);
		}

	}

	glutPostRedisplay();

}
//----------------------------------------------------------------------------

void
updateLightProperties(color4 baseObjectColor)
{

	// Initialize shader lighting parameters
	point4 light_position(light_info[0], light_info[1], light_info[2], light_info[3]);
	color4 light_ambient(light_ambient_info[0], light_ambient_info[1], light_ambient_info[2], 1.0);
	color4 light_diffuse(light_diffuse_info[0], light_diffuse_info[1], light_diffuse_info[2], 1.0);
	color4 light_specular(light_specular_info[0], light_specular_info[1], light_specular_info[2], 1.0);

	light_ambient = light_ambient * light_multiplier; light_ambient[3] = 1.0;
	light_diffuse = light_diffuse * light_multiplier; light_diffuse[3] = 1.0;
	light_specular = light_specular * light_multiplier; light_specular[3] = 1.0;

	color4 material_ambient(0.5, 0.5, 0.5, 1.0);
	color4 material_diffuse = baseObjectColor;
	color4 material_specular = baseObjectColor;
	float  material_shininess = 50.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	point4 centerOfPlayerCube =
		(points[5] +
			points[14]) / 2;
	point4 light_position_2 = centerOfPlayerCube;

	color4 ambient_product_2 = color4(0, 0.0, 0, 0) * material_ambient;
	color4 diffuse_product_2 = currentCubeColor * material_diffuse;
	color4 specular_product_2 = color4(0, 0.0, 0, 0) * material_specular;

	glUniform4fv(ambientProduct, 1, ambient_product);
	glUniform4fv(diffuseProduct, 1, diffuse_product);
	glUniform4fv(specularProduct, 1, specular_product);

	glUniform4fv(ambientProduct_2, 1, ambient_product_2);
	glUniform4fv(diffuseProduct_2, 1, diffuse_product_2);
	glUniform4fv(specularProduct_2, 1, specular_product_2);

	glUniform4fv(lightPosition, 1, light_position);
	glUniform4fv(lightPosition_2, 1, light_position_2);


	glUniform1f(shininess, material_shininess);

}
//void
//updateLightProperties(color4 baseObjectColor)
//{
//	// Initialize shader lighting parameters
//	point4 light_position(-5.0, 15.0, 10.0, 0.0);
//	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
//	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
//	color4 light_specular(1.0, 1.0, 1.0, 1.0);
//
//	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
//	color4 material_diffuse = baseObjectColor;
//	color4 material_specular = baseObjectColor;
//	float  material_shininess = 100.0;
//
//	color4 ambient_product = light_ambient * material_ambient;
//	color4 diffuse_product = light_diffuse * material_diffuse;
//	color4 specular_product = light_specular * material_specular;
//
//	glUniform4fv(ambientProduct, 1, ambient_product);
//	glUniform4fv(diffuseProduct, 1, diffuse_product);
//	glUniform4fv(specularProduct, 1, specular_product);
//
//	glUniform4fv(lightPosition, 1, light_position);
//
//	glUniform1f(shininess, material_shininess);
//
//}

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
				if (platformLeveled[i][j] == 1 || platformLeveled[i][j] == 3)
				{
					platformType[i][j] = 0;
				}
			}
		}
	}
}

void
CleanRemovedPlatformTypesSecure()
{
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			if (platformType[i][j] != 0)
			{
				if (platformLeveled[i][j] == 3)
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
	if (platformLeveled[i][j] == 1 || platformLeveled[i][j] == 3)
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
		if (levitateUpperLimit == (10.8))
		{
			platformLeveled[i][j] = 3;
		}
		else
		{
			platformLeveled[i][j] = 1;
		}
		
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
			case 7: LevitatePlatform(i, j);	updateLightProperties(color4(1.0, 0.0, 0.0, 1.0)); /*updateLightProperties(platformHighlightColor[i][j])*/; glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //Highlighted Path - colored
			case 8: LevitatePlatform(i, j); updateLightProperties(color4(0.298, 0.176, 0.749)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //Highlight Path Button -grey
			case 9: LevitatePlatform(i, j); updateLightProperties(color4(0.0, 1.0, 0.0, 1.0)); glDrawArrays(GL_TRIANGLES, platformIndex[i][j], 36); break; //return  menu button green

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


int comboCounter = 0;
bool tutorialComplete = false;
int startMusicSequenceState = 0;
void HighlightingPathAccordingtoMove();
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

			if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 7)
			{
				playerFollowingPath = true;
			}
			if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] != 7)
			{
				playerFollowingPath = false;
			}
			if (tutorialComplete)
			{
				if (playerFollowingPath && !highlight3StepsAhead)
				{
					HighlightingPathAccordingtoMove();
				}
				if (highlight3StepsAhead)
				{
					HighlightingPathAccordingtoMove();
				}
			}
			if (!tutorialComplete)
			{
				if (playerFollowingPath)
				{
					switch (comboCounter) {
					case 0: engine->play2D("Combo1.wav"); break;
					case 1: engine->play2D("Combo2.wav"); break;
					case 2: engine->play2D("Combo3.wav"); break;
					case 3: engine->play2D("Combo4.wav"); break;
					case 4: engine->play2D("Combo5.wav"); break;
					case 5: engine->play2D("Combo6.wav"); tutorialComplete = true; startMusicSequenceState = 1; comboCounter = 0; break;
					}
					comboCounter++;
				}
				else
				{
					comboCounter = 0;
				}
			}


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

			addPlatformPiece(-3, -3, 1); addPlatformPiece(-2, -3, 1); addPlatformPiece(-1, -3, 1); addPlatformPiece(0, -3, 1); addPlatformPiece(1, -3, 1); addPlatformPiece(2, -3, 1);	addPlatformPiece(3, -3, 1);
			addPlatformPiece(-3, -2, 1); addPlatformPiece(-2, -2, 1); addPlatformPiece(-1, -2, 1); addPlatformPiece(0, -2, 1); addPlatformPiece(1, -2, 1); addPlatformPiece(2, -2, 1);	addPlatformPiece(3, -2, 1);
			addPlatformPiece(-3, -1, 1); addPlatformPiece(-2, -1, 1); addPlatformPiece(-1, -1, 1); addPlatformPiece(0, -1, 1); addPlatformPiece(1, -1, 1); addPlatformPiece(2, -1, 1);	addPlatformPiece(3, -1, 1);
			addPlatformPiece(-3, 0, 1);	addPlatformPiece(-2, 0, 8);	addPlatformPiece(-1, 0, 1);	addPlatformPiece(0, 0, 9);	addPlatformPiece(1, 0, 1);/*addPlatformPiece(2, 0, 1);*/addPlatformPiece(3, 0, 1);
			addPlatformPiece(-3, 1, 1);	addPlatformPiece(-2, 1, 1);	addPlatformPiece(-1, 1, 1);	addPlatformPiece(0, 1, 1);	addPlatformPiece(1, 1, 1); addPlatformPiece(2, 1, 1);	addPlatformPiece(3, 1, 1);

			level1InitializeState = 2;
			updateBuffers();
		}
		return;
	}
}
void
ExpandLevel1() {
	addPlatformPiece(-3, -5, 1);	addPlatformPiece(-2, -5, 1);	addPlatformPiece(-1, -5, 1);	addPlatformPiece(0, -5, 1); addPlatformPiece(1, -5, 1);	addPlatformPiece(2, -5, 1);	addPlatformPiece(3, -5, 1);  addPlatformPiece(4, -5, 1);		addPlatformPiece(5, -5, 1);		addPlatformPiece(6, -5, 1);	addPlatformPiece(7, -5, 1);
	addPlatformPiece(-3, -4, 1);	addPlatformPiece(-2, -4, 1);	addPlatformPiece(-1, -4, 1);	addPlatformPiece(0, -4, 1); addPlatformPiece(1, -4, 1);	addPlatformPiece(2, -4, 1);	addPlatformPiece(3, -4, 1);  addPlatformPiece(4, -4, 1);		addPlatformPiece(5, -4, 1);		addPlatformPiece(6, -4, 1); addPlatformPiece(7, -4, 1);
	/*addPlatformPiece(-3, -3, 1);	addPlatformPiece(-2, -3, 1);addPlatformPiece(-1, -3, 1); addPlatformPiece(0, -3, 1); addPlatformPiece(1, -3, 1); addPlatformPiece(2, -3, 1);	addPlatformPiece(3, -3, 1); */addPlatformPiece(4, -3, 1); addPlatformPiece(5, -3, 1);			addPlatformPiece(6, -3, 1); addPlatformPiece(7, -3, 1);
	/*addPlatformPiece(-3, -2, 1);	addPlatformPiece(-2, -2, 1);addPlatformPiece(-1, -2, 1); addPlatformPiece(0, -2, 1); addPlatformPiece(1, -2, 1); addPlatformPiece(2, -2, 1);	addPlatformPiece(3, -2, 1); */addPlatformPiece(4, -2, 1); addPlatformPiece(5, -2, 1);			addPlatformPiece(6, -2, 1); addPlatformPiece(7, -2, 1);
	/*addPlatformPiece(-3, -1, 1);	addPlatformPiece(-2, -1, 1);addPlatformPiece(-1, -1, 1); addPlatformPiece(0, -1, 1); addPlatformPiece(1, -1, 1); addPlatformPiece(2, -1, 1);	addPlatformPiece(3, -1, 1); */addPlatformPiece(4, -1, 1); addPlatformPiece(5, -1, 1);			addPlatformPiece(6, -1, 1); addPlatformPiece(7, -1, 1);
	/*addPlatformPiece(-3, 0, 1);		addPlatformPiece(-2, 0, 1);	addPlatformPiece(-1, 0, 1);	addPlatformPiece(0, 0, 8);	addPlatformPiece(1, 0, 1);addPlatformPiece(2, 0, 1);addPlatformPiece(3, 0, 1);		*/addPlatformPiece(4, 0, 1); addPlatformPiece(5, 0, 1);			addPlatformPiece(6, 0, 1);  addPlatformPiece(7, 0, 1);
	/*addPlatformPiece(-3, 1, 1);		addPlatformPiece(-2, 1, 1);	addPlatformPiece(-1, 1, 1);	addPlatformPiece(0, 1, 1);	addPlatformPiece(1, 1, 1); addPlatformPiece(2, 1, 1);	addPlatformPiece(3, 1, 1);	*/addPlatformPiece(4, 1, 1);  addPlatformPiece(5, 1, 1);			addPlatformPiece(6, 1, 1);  addPlatformPiece(7, 1, 1);
	addPlatformPiece(-3, 2, 1);		addPlatformPiece(-2, 2, 1);	addPlatformPiece(-1, 2, 1);	addPlatformPiece(0, 2, 1); addPlatformPiece(1, 2, 1);	addPlatformPiece(2, 2, 1);	addPlatformPiece(3, 2, 1);  addPlatformPiece(4, 2, 1);		addPlatformPiece(5, 2, 1);			addPlatformPiece(6, 2, 1);	addPlatformPiece(7, 2, 1);
	addPlatformPiece(-3, 3, 1);		addPlatformPiece(-2, 3, 1);	addPlatformPiece(-1, 3, 1);	addPlatformPiece(0, 3, 1); addPlatformPiece(1, 3, 1);	addPlatformPiece(2, 3, 1);	addPlatformPiece(3, 3, 1);  addPlatformPiece(4, 3, 1);		addPlatformPiece(5, 3, 1);			addPlatformPiece(6, 3, 1);	addPlatformPiece(7, 3, 1);

}

void
mouseClickHandler(int button, int state, int x, int y) {

	if (!TwEventMouseButtonGLUT(button, state, x, y))  // send event to AntTweakBar
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			mouseLocker = false;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
			break;
		case GLUT_MIDDLE_BUTTON:
			mouseLocker = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}


	glutPostRedisplay();

}

void
CalculateDeltaTime()
{
	centerOfPlayerCube = (points[5] + points[14]) / 2;
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
HighlightPanel(int i, int j) {
	i = i + sceneSize / 2; j = j + sceneSize / 2;
	if (platformType[i][j] == 1)
	{
		platformType[i][j] = 7;
		platformHiglightTicker[i][j] = 1;
	}
}

int panelToHighlight = 0;
vector<int> tutorialPath = { 8,8,6,6,5,6 };
int highlightState = 0;
int addX = 0, addY = 0;
float pathHighlightTicker = 0;

void
HighlightTutorialPath() {
	if (highlightState == 0)
	{
		addX = (int)playerCubePos.x;
		addY = (int)playerCubePos.y;
		highlightState = 1;
	}
	/*platformType[(int)playerCubePos.x + addX + sceneSize / 2][(int)playerCubePos.y + addY + sceneSize / 2] = 1;*/
}

int timeTicker = 0;
int dangerSoundTick = 1500; bool dangerSoundPlayed = false;
int songStartTick = 3000;
int scoreUpTick = 6500;
int lightsTick1 = 10200;
int lightsTick2 = 10500;
int lightsTick3 = 10550;
int startHighlightTick = 12150; bool level1Expanded = false;

float offsetForSwapping = 0; int addXm = 0, addYm = 0;
void
HandleMusicSequence() {
	if (startMusicSequenceState == 0)
	{
		return;
	}

	if (startMusicSequenceState == 1)
	{
		platformType[25][25] = 1;
		playerMovementLockToggle = false; // lock player during cutscene
		timeTicker += deltaTime * 1;
		if (timeTicker >= dangerSoundTick && timeTicker < songStartTick)
		{
			if (!dangerSoundPlayed)
			{
				engine->play2D("Danger.wav");
				menuMusic->setIsPaused(true);
				dangerSoundPlayed = true;
			}
			if (light_multiplier > 0.10)
			{
				light_multiplier -= deltaTime * 0.004;
				if (light_multiplier < 0.10)
				{
					light_multiplier = 0.10;
				}
			}

			/*pathHighlightTicker = 0;*/
		}
		if (timeTicker >= songStartTick && timeTicker < scoreUpTick)
		{
			platformType[23][25] = 1;
			/*glClearColor(1.0,0.0,0.0,1.0);*/
			startMusic();
			offsetForSwapping -= deltaTime * 0.02;
			/*pathHighlightTicker = 0;*/
		}
		if (timeTicker >= scoreUpTick && timeTicker < lightsTick1)
		{
			drawLogo = false;
			if (offsetForSwapping < 0)
			{
				offsetForSwapping += deltaTime * 0.02;
				if (offsetForSwapping > 0.0)
				{
					offsetForSwapping = 0.0;
				}
			}
			if (!level1Expanded)
			{
				ExpandLevel1();
				updateBuffers();
				level1Expanded = true;
			}

		}
		if (timeTicker >= lightsTick1 && timeTicker < lightsTick2)
		{
			light_multiplier = 1.0;
		}
		if (timeTicker >= lightsTick2 && timeTicker < lightsTick3)
		{
			light_multiplier = 0.1;
		}
		if (timeTicker >= lightsTick3 && timeTicker < startHighlightTick)
		{
			light_multiplier = 1.0;
		}
		if (timeTicker >= startHighlightTick)
		{
			addXm = (int)playerCubePos.x; addYm = (int)playerCubePos.y - 1;
			HighlightingPathAccordingtoMove();
			playerMovementLockToggle = false;
			startMusicSequenceState = 0;
		}
	}
}
vector<int> musicPath = { 4,4,4,5,6,6,8,4,5,6,5,4,8,6,5,6,8,4,5,6,8,4,5,4,8,6,5,4,8,6,5,4,8 };


int previousDirection = 3;
int
RandomDirection(int addXt, int addYt) {
	if (highlight3StepsAhead)
	{
		int unwantedDirection = 0;
		switch (previousDirection) {
		case 0: unwantedDirection = 2; break;
		case 1: unwantedDirection = 3;  break;
		case 2: unwantedDirection = 0;  break;
		case 3: unwantedDirection = 1;  break;
		}
		int randomCase = 0;
		randomCase = timeSinceStart % 4;
		do
		{
			randomCase = (randomCase + 1) % 4;
			switch (randomCase) {
			case 0: addXt += -1; addYt += 0; break;
			case 1: addXt += 0;	addYt += -1;  break;
			case 2: addXt += 1;	addYt += 0;  break;
			case 3: addXt += 0;	addYt += 1;  break;
			}
		} while (platformType[addXt + sceneSize / 2][addYt + sceneSize / 2] == 0/* || randomCase == unwantedDirection*/);
		previousDirection = randomCase;
		return randomCase;
	}
	else
	{
		int randomCase = 0;
		randomCase = timeSinceStart % 4;
		do
		{
			randomCase = (randomCase + 1) % 4;
			switch (randomCase) {
			case 0: addXt += -1; addYt += 0; break;
			case 1: addXt += 0;	addYt += -1;  break;
			case 2: addXt += 1;	addYt += 0;  break;
			case 3: addXt += 0;	addYt += 1;  break;
			}
		} while (platformType[addXt + sceneSize / 2][addYt + sceneSize / 2] == 0);
		previousDirection = randomCase;
		return randomCase;
	}

}
bool RandomPath = true;
void
HighlightingPathAccordingtoMove() {
	if (!RandomPath)
	{
		switch (musicPath[panelToHighlight]) {
		case 4: addXm += -1; addYm += 0; break;
		case 8: addXm += 0;	addYm += -1;  break;
		case 6: addXm += 1;	addYm += 0;  break;
		case 5: addXm += 0;	addYm += 1;  break;
		}
		HighlightPanel(addXm, addYm);

		panelToHighlight++;
		if (panelToHighlight == musicPath.size())
		{
			panelToHighlight = 0;
			addXm = 0, addYm = 0;
		}
	}

	else
	{
		int direction = RandomDirection(addXm, addYm);
		switch (direction) {
		case 0: addXm += -1; addYm += 0; break;
		case 1: addXm += 0;	addYm += -1;  break;
		case 2: addXm += 1;	addYm += 0;  break;
		case 3: addXm += 0;	addYm += 1;  break;
		}

		HighlightPanel(addXm, addYm);
	}
}

void
HighlightingPath() {

	if (highlightState == 1)
	{
		pathHighlightTicker += deltaTime * 1;
		if (pathHighlightTicker >= pathHighlightDelay)
		{
			switch (tutorialPath[panelToHighlight]) {
			case 4: addX += -1; addY += 0; break;
			case 8: addX += 0;	addY += -1;  break;
			case 6: addX += 1;	addY += 0;  break;
			case 5: addX += 0;	addY += 1;  break;
			}
			HighlightPanel(addX, addY);

			panelToHighlight++;
			if (panelToHighlight == tutorialPath.size())
			{
				panelToHighlight = 0;
				addX = 0, addY = 0;
				highlightState = 0;
			}
			pathHighlightTicker = 0;
		}
	}
}

void
HandleHighlightedPanels()
{
	if (highlight3StepsAhead)
	{
		highlightPathSpeed = 0.1;
	}
	if (!highlight3StepsAhead)
	{
		highlightPathSpeed = 0.5;
	}
	for (int i = 0; i < sceneSize; i++)
	{
		for (int j = 0; j < sceneSize; j++)
		{
			if (platformType[i][j] == 7)
			{
				float levitationDiff = highlightLevitationSpeed * deltaTime;
				int platformBufferIndex = platformIndex[i][j];
				mat4 platformTranslationMatrix;
				float levitateUpperLimit = (-0.6);

				if ((points[platformBufferIndex + 1].y + levitationDiff) >= levitateUpperLimit && platformHiglightTicker[i][j] == 1)
				{
					float offset = levitateUpperLimit - (points[platformBufferIndex + 1].y + levitationDiff);
					levitationDiff += offset;
					if ((points[platformBufferIndex + 1].y + levitationDiff) >= (-0.7) && highlight3StepsAhead)
					{
						/*HighlightingPathAccordingtoMove();*/

					}
					platformTranslationMatrix = generateTranslationMatrix(0.0, levitationDiff, 0.0);
					platformHiglightTicker[i][j] = 2;
				}
				if ((points[platformBufferIndex + 1].y - levitationDiff) <= -0.8 && platformHiglightTicker[i][j] == 2)
				{
					float offset = -0.8 - (points[platformBufferIndex + 1].y - levitationDiff);
					levitationDiff -= offset;
					platformTranslationMatrix = generateTranslationMatrix(0.0, -levitationDiff, 0.0);
					platformHiglightTicker[i][j] = 0;
					platformType[i][j] = 1;
				}
				if (platformHiglightTicker[i][j] == 1)
				{
					platformTranslationMatrix = generateTranslationMatrix(0.0, levitationDiff, 0.0);
				}
				if (platformHiglightTicker[i][j] == 2)
				{
					platformTranslationMatrix = generateTranslationMatrix(0.0, -levitationDiff, 0.0);
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
			}
			else
			{
				continue;
			}
		}
	}
}
float citiesYTranslateSpeed = 0.003;
float citiesYTranslateLimit = -30;
float citiesYTranslate = citiesYTranslateLimit;
void
DrawCities()
{
	mat4 s = generateScaleMatrix(1);
	mat4 r = generateRotationMatrix(0, 0, 0);
	mat4 t = generateTranslationMatrix(0, citiesYTranslate, 0); glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, t*r*s);
	glUniform1i(textured, 1);
	glDrawArrays(GL_TRIANGLES, surfaceBeforeIndex, 6);
	glUniform1i(textured, 0);
	updateLightProperties(vec4(0.046875, 0.14453125, 0.22265625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[0], cityVerticeCount[0]);
	updateLightProperties(vec4(0.24609375, 0.35546875, 0.44140625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[1], cityVerticeCount[1]);
	updateLightProperties(vec4(0.39453125, 0.51953125, 0.609375, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[2], cityVerticeCount[2]);
	updateLightProperties(vec4(0.59765625, 0.7265625, 0.80078125, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[3], cityVerticeCount[3]);
	updateLightProperties(vec4(0.73828125, 0.84375, 0.91015625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[4], cityVerticeCount[4]);
	updateLightProperties(vec4(0.73828125, 0.84375, 0.91015625, 1.0)); glDrawArrays(GL_TRIANGLES, cityIndex[5], cityVerticeCount[5]);
}
void
DrawScore()
{
	updateLightProperties(vec4(1, 0.309, 0.035, 1.0));
	int multiplier = 180;
	int score = iScore;
	int scaleXOffset = 35;
	mat4 s = generateScaleMatrix(90);
	mat4 r = generateRotationMatrix(-90, 0, 0);
	mat4 t = generateTranslationMatrix(50, 0, -350);
	for (int place = 0; place < 4; place++) {
		int digit = score % 10;
		switch (place) {
		case 0:	t = generateTranslationMatrix(scaleXOffset + 150, 8 + offsetForSwapping, -350);	 break;
		case 1:	t = generateTranslationMatrix(scaleXOffset + 100, 8 + offsetForSwapping, -350);	 break;
		case 2:	t = generateTranslationMatrix(scaleXOffset + 50, 8 + offsetForSwapping, -350);	 break;
		case 3:	t = generateTranslationMatrix(scaleXOffset, 8 + offsetForSwapping, -350);	 break;
		}
		glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, t * generateRotationMatrix(score_Rotation[0] * multiplier, score_Rotation[1] * multiplier, score_Rotation[2] * multiplier)  * s);
		switch (digit) {
		case 0:	glDrawArrays(GL_TRIANGLES, numbersIndex[0], numbersCount[0]);	 break;
		case 1:	glDrawArrays(GL_TRIANGLES, numbersIndex[1], numbersCount[1]);	 break;
		case 2:	glDrawArrays(GL_TRIANGLES, numbersIndex[2], numbersCount[2]);	 break;
		case 3:	glDrawArrays(GL_TRIANGLES, numbersIndex[3], numbersCount[3]);	 break;
		case 4:	glDrawArrays(GL_TRIANGLES, numbersIndex[4], numbersCount[4]);	 break;
		case 5:	glDrawArrays(GL_TRIANGLES, numbersIndex[5], numbersCount[5]);	 break;
		case 6:	glDrawArrays(GL_TRIANGLES, numbersIndex[6], numbersCount[6]);	 break;
		case 7:	glDrawArrays(GL_TRIANGLES, numbersIndex[7], numbersCount[7]);	 break;
		case 8:	glDrawArrays(GL_TRIANGLES, numbersIndex[8], numbersCount[8]);	 break;
		case 9:	glDrawArrays(GL_TRIANGLES, numbersIndex[9], numbersCount[9]);	 break;
		}
		score /= 10;
	} while (score > 0);
}
int scoreShownState = 3; //  0 - bottom          1 - bottom to top            2 - top         3 - top to bottom
void
HandleShowScoreHeight() {
	if (scoreShownState == 0)
	{
		return;
	}
	if (scoreShownState == 2)
	{
		return;
	}
	if (scoreShownState == 3)
	{
		citiesYTranslate += citiesYTranslateSpeed * deltaTime;
	}
	if (citiesYTranslate >= 0.0 && scoreShownState == 3)
	{
		scoreShownState = 0;
		citiesYTranslate = 0;
	}
	if (scoreShownState == 1)
	{
		citiesYTranslate -= citiesYTranslateSpeed * deltaTime;
	}
	if (citiesYTranslate <= citiesYTranslateLimit && scoreShownState == 1)
	{
		scoreShownState = 2;
		citiesYTranslate = citiesYTranslateLimit;
	}

}

void calculateScore() {
	int timeSpent = vvvvvv->getPlayPosition() - lastLegitInputTime;
	//Add 51 points each second in perfect condition
	//fScore += (((metronomeCubeColor.y - metronomeCubeColor.x) * timeSpent)*0.0005 + 0.1 + 0.01*comboCounter)*deltaTime*0.1;
	fScore += (((metronomeCubeColor.y - metronomeCubeColor.x) * timeSpent)*0.0005 + 0.10)*deltaTime*0.1;
	if (fScore < 0.0) {
		fScore = 0.0;
	}
	iScore = fScore;
	/*printf("Score float: %f\t Score int: %d\n ", fScore, iScore);*/
}bool menuInitialized = false;
void
InitMenu() {
	if (!menuInitialized)
	{
		if (scoreShownState == 0)
		{
			importLevel("menuLevel.ymp");
			menuInitialized = true;
			updateBuffers();
		}
	}
	else
	{
		return;
	}
}

void
DeathSequence() {
	stopMusic();
	highlightState = 0;
	RemoveAllPlatforms(); RemovePlatformPieceCompletely(playerCubePos.x, playerCubePos.y);
	updateBuffers();
}

void
SongFinishedSequence() {
	scoreShownState = 1;
	addPlatformPiece(0, 0, 9);
	updateBuffers();
}

void
DecideCurrentPlatformAction()
{
	if (centerOfPlayerCube.y < -100)
	{
		cubeRotationSpeed = 7;
		int timeTicker = 0;
		int panelToHighlight = 0;
		vector<int> tutorialPath = { 8,8,6,6,5,6 };
		int highlightState = 0;
		highlight3StepsAhead = false;
		int addX = 0, addY = 0;
		stopMusic();
		startMusicSequenceState = 0;
		float pathHighlightTicker = 0;
		tutorialComplete = false;
		Respawn();
		menuInitialized = false;
		level1InitializeState = 0;
		RemoveAllPlatforms(); addPlatformPiece(0, 0, 1);
		InitMenu();
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 2)
	{
		InitializeLevel1();
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 9)
	{
		scoreShownState = 3;
		menuInitialized = false;
		level1InitializeState = 0;
		RemoveAllPlatforms(); addPlatformPiece(0, 0, 1);
		InitMenu();
	}
	if (platformType[(int)playerCubePos.x + sceneSize / 2][(int)playerCubePos.z + sceneSize / 2] == 8)
	{
		HighlightTutorialPath(); /* HighlightPanel(2, -1);  startMusic();*/
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
	InitMenu();
	HandleShowScoreHeight();
	HandleMusicSequence();
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

	HighlightingPath();
	HandleHighlightedPanels();
	drawPlatforms();
	updateMetronomeCube();
	if (musicIntro) {
		if (vvvvvv->getPlayPosition() >= 500 + 600 * 16) {
			musicStarted = true;
		}
		drawMetronomeCube();
	}
	if (musicStarted) {
		calculateScore();
	}
	float multiplier = 180;
	if (trooperExist) {
		glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, generateTranslationMatrix(10, 0, 0) * generateRotationMatrix(trooper_Rotation[0] * multiplier, trooper_Rotation[1] * multiplier, trooper_Rotation[2] * multiplier) * generateTranslationMatrix(-10, 0, 0));
		RandomizeColor(bb8Color);
		updateLightProperties(bb8Color);
		glDrawArrays(GL_TRIANGLES, bb8Index, bb8VCount);
		r = generateRotationMatrix(0, 0, 0);
		glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, r);
	}

	temp_light_multiplier = light_multiplier;

	/*DrawHighlightPath();*/
	DrawCities();
	light_multiplier = 2.0f;
	updateLightProperties(vec4(1, 0, 0.501, 1.0));
	if (drawLogo == true)
	{
		glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, generateTranslationMatrix(0, offsetForSwapping, 0)*generateTranslationMatrix(110.0, 0.0, -350.0) * generateRotationMatrix(logo_Rotation[0] * multiplier, logo_Rotation[1] * multiplier, logo_Rotation[2] * multiplier) * generateTranslationMatrix(-110.0, 0.0, 350.0));

		glDrawArrays(GL_TRIANGLES, logoIndex, logoCount);
	}
	else
	{
		//glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, generateTranslationMatrix(50, 0, -350) * generateRotationMatrix(logo_Rotation[0] * multiplier, logo_Rotation[1] * multiplier, logo_Rotation[2] * multiplier) * generateTranslationMatrix(-50, 0, +350));
		DrawScore();
	}
	light_multiplier = temp_light_multiplier;
	r = generateRotationMatrix(0, 0, 0);
	glUniformMatrix4fv(trs_matrix, 1, GL_TRUE, r);


	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4), normals.size() * sizeof(vec3), &normals[0]);

	// Draw tweak bars
	TwDraw();
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
	if (musicStarted) {
		gotInput = true;
		//if (scaleMultiplier < 1.60) {
		if (!beat) {
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
				DeathSequence();
				/*return 0;*/

			}
			return 2;
		}
		//else if (scaleMultiplier >= 1.60) {
		else if (beat) {
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
	else {
		return 2;
	}
}

void
keyboard(unsigned char key, int x, int y)
{
	if (!TwEventKeyboardGLUT(key, x, y))  // send event to AntTweakBar
	{
		switch (key) {
		case 033: // Escape Key
			engine->drop(); // delete engine
			exit(EXIT_SUCCESS);
			break;
		case 'w': case 'W': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'U'; if (turnTicker == true)playerCubeMoveDirection = 'R'; } break;
		case 'a': case 'A': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'L'; if (turnTicker == true)playerCubeMoveDirection = 'U'; } break;
		case 's': case 'S': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'D'; if (turnTicker == true)playerCubeMoveDirection = 'L'; } break;
		case 'd': case 'D': if (metronomeCubeHealth() != 2) { break; } if (rotatedAngle == 0 && playerMovementLockToggle == false) { if (turnTicker == false)playerCubeMoveDirection = 'R'; if (turnTicker == true)playerCubeMoveDirection = 'D'; } break;
		case 'f': case 'F': freecamToggle = !freecamToggle; break;
		case 'r': case 'R': Respawn(); break;
		case 'p': case 'P': stopMusic(); SongFinishedSequence(); break;
		case 'c': case 'C': position = initialPosition; verticalAngle = initialVerticalAngle; horizontalAngle = initialHorizontalAngle; break;
		case 't': case 'T': if (toonEnable == 0) { toonEnable = 1; }
				  else if (toonEnable == 1) { toonEnable = 2; }
				  else if (toonEnable == 2) { toonEnable = 0; } break;
		//case 'p': printf("positionX: %f, positionY: %f,positionZ: %f,horizontalAngle:%f\n verticalAngle:%f, zNear:%f, zFar:%f, radius:%f, theta : %f, phi:%f\n", position.x, position.y, position.z, horizontalAngle, verticalAngle, position.y, position.z, radius, theta, phi); break;
		}
	}
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
void SpecialInput(int key, int x, int y)
{
	if (!TwEventSpecialGLUT(key, x, y))  // send event to AntTweakBar
	{
		switch (key)
		{
		case GLUT_KEY_UP: if (freecamToggle == true)position += direction * speed; break;
		case GLUT_KEY_DOWN: if (freecamToggle == true)position -= direction * speed; break;
		case GLUT_KEY_RIGHT: if (freecamToggle == true)position += right_vector * speed; break;
		case GLUT_KEY_LEFT: if (freecamToggle == true)position -= right_vector * speed; break;
		}
	}
	glutPostRedisplay();

}

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = GLfloat(width) / height;
	TwWindowSize(width, height);

}
void TW_CALL trooperTrigger(void * /*clientData*/)
{

	if (trooperExist) {
		trooperExist = false;
	}
	else {
		trooperExist = true;
	}
}

void TW_CALL newPlatform(void * /*clientData*/)
{

	addPlatformPiece(new_platform_x, new_platform_y, 1);
	updateBuffers();
}

void TW_CALL removePlatform(void * /*clientData*/)
{
	RemovePlatformPieceCompletely(new_platform_x, new_platform_y);
	updateBuffers();
}
void TW_CALL Reset(void * /*clientData*/)
{
	light_info[0] = 1.31;
	light_info[1] = -11.52;
	light_info[2] = -14.69;
	trooper_Rotation[0] = trooper_Rotation[1] = trooper_Rotation[2] = 0.0f;
	trooper_Rotation[3] = 1.0f;
	logo_Rotation[0] = logo_Rotation[1] = logo_Rotation[2] = 0.0f;
	logo_Rotation[3] = 1.0f;
	score_Rotation[0] = -0.47f; score_Rotation[1] = 0.0f; score_Rotation[2] = 0.04f;
	score_Rotation[3] = 1.0f;
	light_multiplier = 1.0f;
	trooperExist = false;
}

// Function called at exit
void Terminate(void)
{

	TwTerminate();
}

//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAlphaAttrib(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = light_info[3]; // copy g_AutoRotate to value
}

void TW_CALL SetAlphaAttrib(const void *value, void *clientData)
{
	(void)clientData; // unused

	light_info[3] = *(const int *)value; // copy value to g_AutoRotate

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

	TwBar *bar; // Pointer to the tweak bar
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	//glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE
#if _DEBUG		
		| GLUT_DEBUG
#endif
	);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	window_id = glutCreateWindow("YaCuBe");
	glutCreateMenu(NULL);

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

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Set GLUT event callbacks
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutSpecialFunc(SpecialInput);
	glutPassiveMotionFunc(MouseController);
	glutMouseFunc(mouseClickHandler);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutFullScreen();
	atexit(Terminate);




	// Create a tweak bar
	TwWindowSize(200, 595);
	bar = TwNewBar("YaCubeUi");
	TwDefine(" YaCubeUi iconified=true ");

	TwDefine(" GLOBAL help='Controls: W,A,S,D: player movement				  			R: RespawnLeft											Mouse Button: Show cursor												Middle Mouse Button: Hide cursor													 F: Toggle FreeCam (Mouse should be hidden)													Freecam Controls: Move Camera with arrow keys, look around with mouse.																	 C: Reset Camera Position																T: Switch Toon Shading modes' ");// Message added to the help bar.
	//TwDefine(" GLOBAL help='	      R: Respawn ' ");
	//TwDefine(" GLOBAL help='	      Left Mouse Button: Show cursor ' ");
	//TwDefine(" GLOBAL help='	      Middle Mouse Button: Hide cursor ' ");
	//TwDefine(" GLOBAL help='	      F: Toggle FreeCam (Mouse should be hidden) ' ");
	//TwDefine(" GLOBAL help='	      Freecam Controls: Move Camera with arrow keys, look around with mouse. ' ");
	//TwDefine(" GLOBAL help='	      C: Reset Camera Position ' ");
	//TwDefine(" GLOBAL help='	      T: Switch Toon Shading Modes ' ");
	TwDefine(" YaCubeUi size='200 595' color='255 0 0' "); // change default tweak bar size and 
														   // Add 'g_LightDirection' to 'bar': this is a variable of type TW_TYPE_DIR3F which defines the light direction
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &light_info,
		" label='Light direction' opened=true help='Change the light direction.' ");

	// Add 'g_MatAmbient' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
	// and is inserted into a group named 'Material'.
	TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &light_ambient_info, " group='Material' ");

	// Add 'g_MatDiffuse' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
	// and is inserted into group 'Material'.
	TwAddVarRW(bar, "Diffuse", TW_TYPE_COLOR3F, &light_diffuse_info, " group='Material'");


	// Add 'g_MatDiffuse' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
	// and is inserted into group 'Material'.
	TwAddVarRW(bar, "Specular", TW_TYPE_COLOR3F, &light_specular_info, " group='Material' ");
	TwAddSeparator(bar, "sep1", NULL);

	// Add 'g_Rotation' to 'bar': this is a variable of type TW_TYPE_QUAT4F which defines the object's orientation
	TwAddVarRW(bar, "trooperRotation", TW_TYPE_QUAT4F, &trooper_Rotation,
		" label='Trooper rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRW(bar, "logoRotation", TW_TYPE_QUAT4F, &logo_Rotation,
		" label='Logo rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRW(bar, "scoreRotation", TW_TYPE_QUAT4F, &score_Rotation,
		" label='Score rotation' opened=true help='Change the object orientation.' ");
	// Add 'g_LightMultiplier' to 'bar': this is a variable of type TW_TYPE_FLOAT. Its key shortcuts are [+] and [-].
	TwAddVarRW(bar, "Multiplier", TW_TYPE_FLOAT, &light_multiplier,
		" label='Light booster' min=0.1 max=4 step=0.02 keyIncr='+' keyDecr='-' help='Increase/decrease the light power.' ");

	TwAddButton(bar, "Reset", Reset, NULL, " label='Reset' help='Re-initialize rotation variables and multipliers.' ");
	TwAddButton(bar, "Trooper", trooperTrigger, NULL, " label='Trooper' help='Toogle trooper.' ");
	TwAddSeparator(bar, "sep1", NULL);

	TwAddVarRW(bar, "X", TW_TYPE_INT32, &new_platform_x, "label='Platform X'  group='Platform' ");
	TwAddVarRW(bar, "Y", TW_TYPE_INT32, &new_platform_y, "label='Platform Y' group='Platform' ");
	TwAddButton(bar, "NewPlatform", newPlatform, NULL, " group='Platform' label='Add Platform' help='Adds new platform to given parameters.' ");
	TwAddButton(bar, "RemovePlatform", removePlatform, NULL, " group='Platform' label='Remove Platform' help='Deletes the platform with the given parameters.' ");


	glutMainLoop();
	return 0;
}
