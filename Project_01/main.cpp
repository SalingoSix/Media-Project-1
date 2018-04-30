#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <stdio.h>
#include <Windows.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SceneHandler.h"
#include "cShaderManager.h"
#include "cMesh.h"
#include "cVAOMeshManager.h"
#include "cGameObject.h"
#include "cLightManager.h"
#include "fmodManager.h"

//Globals

FMOD_RESULT mresult;
FMOD::System *msystem = NULL;
FMOD::Channel *pChannel[10];
FMOD::Sound *pSound[10];
std::string songNames[10];
float songPan[10] = { 0 };
float songVolume[10] = { 1,1,1,1,1,1,1,1,1,1 };
float songPitch[10] = { 1,1,1,1,1,1,1,1,1,1 };;
float songSpeed[10] = { 1,1,1,1,1,1,1,1,1,1 };; //Not useful unless sound file type supports speed changing

float mfrequency = 0.0f;
float mvolume = 0.0f;
float mpan = 0.0f;

bool mis_esc = false;
bool mkeydown = false;
bool playingMusic = 0;
unsigned int posInSong = 0;
unsigned int songLength = 0;
int currentChannel = 1;
int refreshInfo = 50;

//Camera variables
float angle = 0.0f;
float lookx = 0.0f, looky = 1.0f, lookz = -1.0f;
float camPosx = 0.0f, camPosz = 5.0f;
glm::vec3 g_cameraXYZ = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 g_cameraTarget_XYZ = glm::vec3(0.0f, 1.0f, 0.0f);

int radioStatus = 0;
int stepStatus = 0;
bool sickStatus = false;
bool botherStatus = false;

cShaderManager* g_pShaderManager = new cShaderManager();
cVAOMeshManager* g_pVAOManager = new cVAOMeshManager();
std::vector <cGameObject*> g_vecGameObject;
cLightManager* g_pLightManager = new cLightManager();

void toggleRadio();
void takeAStep();
void brokenBowl();
void pourDrink();
void sickMan();
void botherMan();

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if(g_cameraXYZ.x > 0.0f && g_cameraXYZ.x < 0.2f &&
			g_cameraXYZ.z >= 3.6f && g_cameraXYZ.z <= 3.83f)
			toggleRadio();

		else if (g_cameraXYZ.x > -2.2f && g_cameraXYZ.x < 0.0f &&
			g_cameraXYZ.z >= 3.6f && g_cameraXYZ.z <= 3.83f)
			pourDrink();

		else if (g_cameraXYZ.x > -0.5f && g_cameraXYZ.x < 0.08f &&
			g_cameraXYZ.z >= 3.85f && g_cameraXYZ.z <= 4.3f)
			sickMan();
	}

	const float CAMERASPEED = 0.03f; //Controls movement, and triggers movement based sound effects
	switch (key)
	{
	//MOVEMENT USING TANK CONTROLS WASD
	case GLFW_KEY_A:		// Left
		angle -= CAMERASPEED * 3;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_D:		// Right
		angle += CAMERASPEED * 3;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_W:		// Forward (along z)
		camPosx += lookx * 0.1f;
		camPosz += lookz * 0.1f;
		takeAStep();

		if (camPosx > -0.5f && camPosx < -0.08f &&
			camPosz >= 3.85f && camPosz <= 4.3f &&
			!sickStatus)
			sickMan();

		if (camPosx > 0.345f && camPosx < 0.8f &&
			camPosz >= 4.23f && camPosz <= 4.6f)
		{
			if (!botherStatus)
				botherMan();
		}
		else
			botherStatus = false;
		break;
	case GLFW_KEY_S:		// Backwards (along z)
		camPosx -= lookx * 0.1f;
		camPosz -= lookz * 0.1f;
		takeAStep();

		if (camPosx > -0.5f && camPosx < -0.08f &&
			camPosz >= 3.85f && camPosz <= 4.3f &&
			!sickStatus)
			sickMan();

		if (camPosx > 0.345f && camPosx < 0.8f &&
			camPosz >= 4.23f && camPosz <= 4.6f)
		{
			if(!botherStatus)
				botherMan();
		}
		else
			botherStatus = false;	
		break;
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		currentChannel = 0;

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		currentChannel = 1;

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		currentChannel = 2;

	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		currentChannel = 3;

	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		currentChannel = 4;

	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		currentChannel = 5;

	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		currentChannel = 6;

	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		currentChannel = 7;

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
		currentChannel = 8;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		currentChannel = 9;

	//Volume Up
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) 
	{
		if (songVolume[currentChannel] < 5)
		{
			songVolume[currentChannel] += 0.05f;
			pChannel[currentChannel]->setVolume(songVolume[currentChannel]);
		}
		else
		{
			songVolume[currentChannel] = 5;
			pChannel[currentChannel]->setVolume(songVolume[currentChannel]);
		}
	}
	//Volume Down
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) 
	{
		if (songVolume[currentChannel] > 0)
		{
			songVolume[currentChannel] -= 0.05f;
			pChannel[currentChannel]->setVolume(songVolume[currentChannel]);
		}
		else
		{
			songVolume[currentChannel] = 0;
			pChannel[currentChannel]->setVolume(songVolume[currentChannel]);
		}
	}
	
	//Pan Left
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) 
	{
		if (songPan[currentChannel] > -1)
		{
			songPan[currentChannel] -= 0.05;
			pChannel[currentChannel]->setPan(songPan[currentChannel]);
		}
		else
		{
			songPan[currentChannel] = -1;
			pChannel[currentChannel]->setPan(songPan[currentChannel]);
		}
	}
	//Pan Right
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) 
	{
		if (songPan[currentChannel] < 1)
		{
			songPan[currentChannel] += 0.05;
			pChannel[currentChannel]->setPan(songPan[currentChannel]);
		}
		else
		{
			songPan[currentChannel] = 1;
			pChannel[currentChannel]->setPan(songPan[currentChannel]);
		}
	}
	
	//Speed Down (These don't work with MP3's, which are being used here)
	if (key == GLFW_KEY_O && action == GLFW_PRESS) 
	{
		if (songSpeed[currentChannel] >= 0.03f)
		{
			songSpeed[currentChannel] -= 0.02f;
			pSound[currentChannel]->setMusicSpeed(songSpeed[currentChannel]);
		}
		else
		{
			songSpeed[currentChannel] = 0.01f;
			pSound[currentChannel]->setMusicSpeed(songSpeed[currentChannel]);
		}
	}
	//Speed Up
	if (key == GLFW_KEY_P && action == GLFW_PRESS) 
	{
		if (songSpeed[currentChannel] < 10.0f)
		{
			songSpeed[currentChannel] += 0.02f;
			pSound[currentChannel]->setMusicSpeed(songSpeed[currentChannel]);
		}
		else
		{
			songSpeed[currentChannel] = 10.0f;
			pSound[currentChannel]->setMusicSpeed(songSpeed[currentChannel]);
		}
	}
	
	//Pitch Down
	if (key == GLFW_KEY_K && action == GLFW_PRESS) 
	{
		if (songPitch[currentChannel] > 0.03f)
		{
			songPitch[currentChannel] -= 0.02f;
			pChannel[currentChannel]->setPitch(songPitch[currentChannel]);
		}
		else
		{
			songPitch[currentChannel] = 0.01;
			pChannel[currentChannel]->setPitch(songPitch[currentChannel]);
		}
	}
	//Pitch Up
	if (key == GLFW_KEY_L && action == GLFW_PRESS) 
	{
		if (songPitch[currentChannel] < 10.0f)
		{
			songPitch[currentChannel] += 0.02f;
			pChannel[currentChannel]->setPitch(songPitch[currentChannel]);
		}
		else
		{
			songPitch[currentChannel] = 10.0f;
			pChannel[currentChannel]->setPitch(songPitch[currentChannel]);
		}
	}
}

int main()
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//Initialize FMOD
	// Create the main system object.
	mresult = FMOD::System_Create(&msystem);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	//Initializes the system object, and the msound device. This has to be called at the start of the user's program
	mresult = msystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}

	bool compressMusic = false;
	char userIn = 0;
	std::cout << "Type 'y' for uncompressed sound, else type 'n' for compressed audio: ";
	std::cin >> userIn;
	system("cls");
	if (userIn != 'y')
		compressMusic = true;

	//Load all sounds
	std::ifstream soundFiles("SoundDetails.txt");
	if (!soundFiles.is_open())
		return 0;

	std::string curLine;

	for (int i = 0; i < 10; i++)
	{
		soundFiles >> curLine;
		if(compressMusic)
			mresult = msystem->createSound(curLine.c_str(), FMOD_CREATECOMPRESSEDSAMPLE, 0, &pSound[i]);
		else
			mresult = msystem->createSound(curLine.c_str(), FMOD_CREATESAMPLE, 0, &pSound[i]);
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		soundFiles >> curLine;
		songNames[i] = curLine;
	}
	pSound[0]->setMode(FMOD_LOOP_NORMAL);
	pSound[1]->setMode(FMOD_LOOP_NORMAL);
	pSound[2]->setMode(FMOD_LOOP_NORMAL); //https://www.youtube.com/watch?v=2LHYAmtfxiY


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1080, 720,
		"Welcome to a great video game",
		NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath("assets//shaders//");

	if (!::g_pShaderManager->createProgramFromFile(
		"simpleShader", vertShader, fragShader))
	{
		std::cout << "Failed to create shader program. Shutting down." << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		return -1;
	}
	//std::cout << "The shaders comipled and linked OK" << std::endl;
	GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	std::string* plyDirects;
	std::string* plyNames;
	int numPlys;

	if (!readPlysToLoad(&plyDirects, &plyNames, &numPlys))
	{
		std::cout << "Couldn't find files to read. Giving up hard.";
		return -1;
	}

	for (int i = 0; i < numPlys; i++)
	{	//Load each file into a VAO object
		cMesh newMesh;
		newMesh.name = plyNames[i];
		if (!LoadPlyFileIntoMesh(plyDirects[i], newMesh))
		{
			std::cout << "Didn't load model" << std::endl;
		}
		if (!::g_pVAOManager->loadMeshIntoVAO(newMesh, shaderID))
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}

	int numEntities;
	if (!readEntityDetails(&g_vecGameObject, &numEntities))
	{
		std::cout << "Unable to find game objects for placement." << std::endl;
	}

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	GLint uniLoc_mModel = glGetUniformLocation(currentProgID, "mModel");
	GLint uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
	GLint uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");

	GLint uniLoc_materialDiffuse = glGetUniformLocation(currentProgID, "materialDiffuse");

	::g_pLightManager->createLights();	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);

	glEnable(GL_DEPTH);

	//Start playing our ambient noises
	mresult = msystem->playSound(pSound[0], 0, false, &pChannel[0]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	mresult = msystem->playSound(pSound[1], 0, true, &pChannel[1]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	mresult = msystem->playSound(pSound[2], 0, false, &pChannel[2]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}

	brokenBowl();

	bool isPlaying = false;
	unsigned int channel_position = 0;
	unsigned int sound_length = 0;
	float origSoundFreq = 0;
	float channelPitch;
	FMOD_SOUND_TYPE songType;
	FMOD_SOUND_FORMAT songFormat;
	int songChannels;
	int songBits;

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4x4 m, p;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		::g_pLightManager->CopyInfoToShader();

		unsigned int sizeOfVector = ::g_vecGameObject.size();	//*****//
		for (int index = 0; index != sizeOfVector; index++)
		{
			// Is there a game object? 
			if (::g_vecGameObject[index] == 0)	//if ( ::g_GameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObject[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
			{	// Didn't find mesh
				continue;
			}



			// There IS something to draw

			m = glm::mat4x4(1.0f);	//		mat4x4_identity(m);

			glm::mat4 matPreRotX = glm::mat4x4(1.0f);
			matPreRotX = glm::rotate(matPreRotX, ::g_vecGameObject[index]->orientation.x,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotX;
			
			glm::mat4 matPreRotY = glm::mat4x4(1.0f);
			matPreRotY = glm::rotate(matPreRotY, ::g_vecGameObject[index]->orientation.y,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotY;
			
			glm::mat4 matPreRotZ = glm::mat4x4(1.0f);
			matPreRotZ = glm::rotate(matPreRotZ, ::g_vecGameObject[index]->orientation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotZ;

			glm::mat4 trans = glm::mat4x4(1.0f);
			trans = glm::translate(trans,
				::g_vecGameObject[index]->position);
			m = m * trans;

			glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
			matPostRotZ = glm::rotate(matPostRotZ, ::g_vecGameObject[index]->orientation2.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPostRotZ;

			glm::mat4 matPostRotY = glm::mat4x4(1.0f);
			matPostRotY = glm::rotate(matPostRotY, ::g_vecGameObject[index]->orientation2.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			m = m * matPostRotY;


			glm::mat4 matPostRotX = glm::mat4x4(1.0f);
			matPostRotX = glm::rotate(matPostRotX, ::g_vecGameObject[index]->orientation2.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			m = m * matPostRotX;

			float finalScale = ::g_vecGameObject[index]->scale;
			glm::mat4 matScale = glm::mat4x4(1.0f);
			matScale = glm::scale(matScale,
				glm::vec3(finalScale,
						finalScale,
						finalScale));
			m = m * matScale;

			p = glm::perspective(0.6f,			// FOV
				ratio,		// Aspect ratio
				0.1f,			// Near (as big as possible)
				1000.0f);	// Far (as small as possible)

							// View or "camera" matrix
			glm::mat4 v = glm::mat4(1.0f);	// identity

											//glm::vec3 cameraXYZ = glm::vec3( 0.0f, 0.0f, 5.0f );	// 5 units "down" z
			g_cameraXYZ.x = camPosx;
			g_cameraXYZ.z = camPosz;

			g_cameraTarget_XYZ.x = camPosx + lookx;
			g_cameraTarget_XYZ.y = looky;
			g_cameraTarget_XYZ.z = camPosz + lookz;

			v = glm::lookAt(g_cameraXYZ,						// "eye" or "camera" position
				g_cameraTarget_XYZ,		// "At" or "target" 
				glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector


			glUniform4f(uniLoc_materialDiffuse,
				::g_vecGameObject[index]->diffuseColour.r,
				::g_vecGameObject[index]->diffuseColour.g,
				::g_vecGameObject[index]->diffuseColour.b,
				::g_vecGameObject[index]->diffuseColour.a);


			//        glUseProgram(program);
			::g_pShaderManager->useShaderProgram("simpleShader");

			glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(m));

			glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(v));

			glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(p));

			if(::g_vecGameObject[index]->wireFrame)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(VAODrawInfo.VAO_ID);

			glDrawElements(GL_TRIANGLES,
				VAODrawInfo.numberOfIndices,
				GL_UNSIGNED_INT,
				0);

			glBindVertexArray(0);

		}//for ( int index = 0...

		isPlaying = false;
		pSound[currentChannel]->getDefaults(&origSoundFreq, 0);
		pSound[currentChannel]->getFormat(&songType, &songFormat, &songChannels, &songBits);
		pSound[currentChannel]->getLength(&songLength, FMOD_TIMEUNIT_MS);
		//Get position
		pChannel[currentChannel]->getPosition(&channel_position, FMOD_TIMEUNIT_MS);

		//Get pitch
		pChannel[currentChannel]->getPitch(&channelPitch);

		//Get is paused
		if (currentChannel <= 2)	//Constantly streaming music
			pChannel[currentChannel]->getPaused(&isPlaying);
		else						//Sound effects
		{
			pChannel[currentChannel]->isPlaying(&isPlaying);
			isPlaying = !isPlaying;
		}

		//Get Volume
		pChannel[currentChannel]->getVolume(&mvolume);

		pChannel[currentChannel]->getPosition(&posInSong, FMOD_TIMEUNIT_MS);

		print_text("==============================================================");
		print_text("Media Fundamentals...");
		print_text("==============================================================");
		print_text("");
		print_text("Sound File Name: %s          ", songNames[currentChannel].c_str());
		print_text("");
		print_text("Current Position: %d:%d%d:%d%d out of %d:%d%d:%d%d", posInSong / 60000, (posInSong / 10000) % 6, (posInSong / 1000) % 10, (posInSong / 100) % 10, (posInSong / 10) % 10, songLength / 60000, (songLength / 10000) % 6, (songLength / 1000) % 10, (songLength / 100) % 10, (songLength / 10) % 10);
		print_text("Sound currently Playing: ");
		if (!isPlaying)
			print_text("YES");
		else
			print_text("NO ");
		print_text("Native Frequency: %f", origSoundFreq);
		print_text("Current Volume: %f", songVolume[currentChannel]);
		print_text("Current Pitch: %f", songPitch[currentChannel]);
		print_text("Pan Balance: %f", songPan[currentChannel]);
		print_text("Sound Type: %s", fmod_sound_type_as_string(songType));
		print_text("Sound Format: %s", fmod_sound_format_as_string(songFormat));
		print_text("Number of Channels Used: %d", songChannels);
		print_text("Bits Per Sample: %d", songBits);

		//Needed for print_text
		end_text();

		glfwSwapBuffers(window);
		glfwPollEvents();

		mresult = msystem->update();
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}

	}

	glfwDestroyWindow(window);
	glfwTerminate();
	for (int i = 0; i < 10; i++)
	{
		if (pSound[i]) 
		{
			mresult = pSound[i]->release();
			if (mresult != FMOD_OK)
			{
				fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
				exit(-1);
			}
		}
	}
	if (msystem) {
		mresult = msystem->close();
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		mresult = msystem->release();
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
	}
	return 0;
}

void toggleRadio()
{
	bool playing = false;
	mresult = pChannel[4]->isPlaying(&playing);
	if (!playing)
	{
		mresult = msystem->playSound(pSound[4], 0, false, &pChannel[4]);
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		pChannel[4]->setVolume(songVolume[4]);
		pChannel[4]->setPan(songPan[4]);
		pChannel[4]->setPitch(songPitch[4]);
		pSound[4]->setMusicSpeed(songSpeed[4]);

		if (radioStatus == 0)
		{
			mresult = pChannel[1]->setPaused(false);
			if (mresult != FMOD_OK)
			{
				fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
				exit(-1);
			}
			mresult = pChannel[2]->setPaused(true);
			if (mresult != FMOD_OK)
			{
				fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
				exit(-1);
			}
			radioStatus = 1;
		}
		else if (radioStatus == 1)
		{
			mresult = pChannel[1]->setPaused(true);
			if (mresult != FMOD_OK)
			{
				fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
				exit(-1);
			}
			radioStatus = 2;
		}
		else if (radioStatus == 2)
		{
			mresult = pChannel[2]->setPaused(false);
			if (mresult != FMOD_OK)
			{
				fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
				exit(-1);
			}
			radioStatus = 0;
		}
	}
	return;
}

void takeAStep()
{
	if (stepStatus == 0)
	{
		mresult = msystem->playSound(pSound[6], 0, false, &pChannel[6]);
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		pChannel[6]->setVolume(songVolume[6]);
		pChannel[6]->setPan(songPan[6]);
		pChannel[6]->setPitch(songPitch[6]);
		pSound[6]->setMusicSpeed(songSpeed[6]);
		stepStatus = 1;
	}
	else if (stepStatus == 1)
		stepStatus = 2;
	else if (stepStatus == 2)
	{
		mresult = msystem->playSound(pSound[7], 0, false, &pChannel[7]);
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		pChannel[7]->setVolume(songVolume[7]);
		pChannel[7]->setPan(songPan[7]);
		pChannel[7]->setPitch(songPitch[7]);
		pSound[7]->setMusicSpeed(songSpeed[7]);
		stepStatus = 3;
	}
	else if (stepStatus == 3)
		stepStatus = 0;
	return;
}

void brokenBowl()
{
	mresult = msystem->playSound(pSound[5], 0, false, &pChannel[5]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	pChannel[5]->setVolume(songVolume[5]);
	pChannel[5]->setPan(songPan[5]);
	pChannel[5]->setPitch(songPitch[5]);
	pSound[5]->setMusicSpeed(songSpeed[5]);
	return;
}

void pourDrink()
{
	bool playing = false;
	mresult = pChannel[9]->isPlaying(&playing);
	if (!playing)
	{
		mresult = msystem->playSound(pSound[9], 0, false, &pChannel[9]);
		if (mresult != FMOD_OK)
		{
			fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
			exit(-1);
		}
		pChannel[9]->setVolume(songVolume[9]);
		pChannel[9]->setPan(songPan[9]);
		pChannel[9]->setPitch(songPitch[9]);
		pSound[9]->setMusicSpeed(songSpeed[9]);
	}
	return;
}

void sickMan()
{
	mresult = msystem->playSound(pSound[3], 0, false, &pChannel[3]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	pChannel[3]->setVolume(songVolume[3]);
	pChannel[3]->setPan(songPan[3]);
	pChannel[3]->setPitch(songPitch[3]);
	pSound[3]->setMusicSpeed(songSpeed[3]);
	sickStatus = true;
	return;
}

void botherMan()
{
	mresult = msystem->playSound(pSound[8], 0, false, &pChannel[8]);
	if (mresult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", mresult, FMOD_ErrorString(mresult));
		exit(-1);
	}
	pChannel[8]->setVolume(songVolume[8]);
	pChannel[8]->setPan(songPan[8]);
	pChannel[8]->setPitch(songPitch[8]);
	pSound[8]->setMusicSpeed(songSpeed[8]);
	botherStatus = true;
	return;
}