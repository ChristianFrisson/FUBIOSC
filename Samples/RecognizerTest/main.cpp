#include "Fubi.h"

#include <iostream>
#include <string>
#include <sstream>
#include <queue>

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <FubiUtils.h>

#if defined ( WIN32 ) || defined( _WINDOWS )
#include <Windows.h>
#endif

using namespace Fubi;

// Some additional OpenGL defines
#define GL_GENERATE_MIPMAP_SGIS           0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS      0x8192
#define GL_BGRA                           0x80E1


// Some global variables for the application
unsigned char* g_depthData = 0x0;
unsigned char* g_rgbData = 0x0;
unsigned char* g_irData = 0x0;

int dWidth= 0, dHeight= 0, rgbWidth= 0, rgbHeight= 0, irWidth = 0, irHeight = 0;

bool g_showRGBImage = false;
bool g_showIRImage = false;

short g_showInfo = 0;

bool g_showFingerCounts = false;
bool g_useOldFingerCounts = false;

bool g_currentPostures[Postures::NUM_POSTURES];
std::vector<bool> g_currentUserDefinedRecognizers;
short g_recognitionOutputMode = 0;
const short g_numOutputmodes = 5;
bool g_takePictures = false;
bool g_exitNextFrame = false;

// Function called each frame for all tracked users
void checkPostures(unsigned int userID)
{
	switch(g_recognitionOutputMode)
	{
	case 4: // Special: pointing gestures (categorie 1)
		{
			if (recognizeGestureOn(Postures::POINTING_RIGHT, userID) == RecognitionResult::RECOGNIZED)
			{
				// User is pointing somewhere
				FubiUser* user = getUser(userID);
				// Get pointing direction
				const Vec3f& rightHand = user->m_currentTrackingData.jointPositions[SkeletonJoint::RIGHT_HAND].m_position;
				const Vec3f& rightShoulder = user->m_currentTrackingData.jointPositions[SkeletonJoint::RIGHT_SHOULDER].m_position;
				Vec3f dir = rightHand - rightShoulder;
				printf("User is pointing from (%.0f,%.0f,%.0f) to dir (%.0f,%.0f,%.0f)\n", rightShoulder.x, rightShoulder.y, rightShoulder.z, dir.x, dir.y, dir.z);
			}

			break;
		}
	case 3: // Call recognizers for all postures (categorie 1)
		{
			for (unsigned int i= 0; i < Postures::NUM_POSTURES; ++i)
			{
				Postures::Posture p = (Postures::Posture) i;
				if (recognizeGestureOn(p, userID) == RecognitionResult::RECOGNIZED)
				{
					if (!g_currentPostures[i])
						printf("User %d - Posture Start: %s! -->\n", userID, getPostureName(p));
					g_currentPostures[i] = true;
				}
				else if (g_currentPostures[i])
				{
					printf("--> User %d - Posture Finished: %s!\n", userID, getPostureName(p));
					g_currentPostures[i] = false;
				}
			}
			break;
		}
	case 2: // Combinations (categorie 3)
		{
			for (unsigned int i= 0; i < Combinations::NUM_COMBINATIONS; ++i)
			{
				Combinations::Combination p = (Combinations::Combination) i;
				std::vector<FubiUser::TrackingData> recInfo;
				if (getCombinationRecognitionProgressOn(p, userID, &recInfo) == RecognitionResult::RECOGNIZED)
				{
					Vec3f handMov;
					float vel = 0;
					if (recInfo.size() >= 3)
					{
						FubiUser::TrackingData& lastState = recInfo.back();
						FubiUser::TrackingData& preState = recInfo[0];

						handMov = lastState.jointPositions[SkeletonJoint::RIGHT_HAND].m_position - preState.jointPositions[SkeletonJoint::RIGHT_HAND].m_position;
						handMov *= 0.001f; // Convert to meter
						vel = handMov.length() / (lastState.timeStamp - preState.timeStamp);
					}
					printf("User %d -- Combination Succeeded: %s! Vec:%.2f|%.2f|%.2f Vel:%.2f\n", userID, 
						getCombinationName(p), handMov.x, handMov.y, handMov.z, vel);
				}
			}
			break;
		}
	case 1: // User defined combinations
		{
			for (unsigned int i= 0; i < getNumUserDefinedCombinationRecognizers(); ++i)
			{
				if (getCombinationRecognitionProgressOn(getUserDefinedCombinationRecognizerName(i), userID) == RecognitionResult::RECOGNIZED)
				{
					printf("User %d -- User Defined Combination Succeeded: %s!\n", userID, 
						getUserDefinedCombinationRecognizerName(i));
				}
			}
			break;
		}
	case 0: // User defined joint relations and linear gestures (categorie 1 + 2)
		{
			unsigned int numRels = getNumUserDefinedRecognizers();
			if (g_currentUserDefinedRecognizers.size() < numRels)
			{
				g_currentUserDefinedRecognizers.resize(numRels, false);
			}
			for (unsigned int i=0; i < getNumUserDefinedRecognizers(); ++i)
			{
				if (recognizeGestureOn(i, userID)== RecognitionResult::RECOGNIZED)
				{				
					if (!g_currentUserDefinedRecognizers[i])
						printf("User %d -- User defined Rec START: %d %s -->\n", userID, i, getUserDefinedRecognizerName(i));
					g_currentUserDefinedRecognizers[i] = true;
				}
				else if (g_currentUserDefinedRecognizers[i])
				{
					printf("--> User %d -User defined Rec FINISHED: %d %s!\n", userID, i, getUserDefinedRecognizerName(i));
					g_currentUserDefinedRecognizers[i] = false;
				}

			}
			break;
		}
	}
}

void glutIdle (void)
{
	// Display the frame
	glutPostRedisplay();
}

// The glut update functions called every frame
void glutDisplay (void)
{
	if (g_exitNextFrame)
	{
		release();
		exit (0);
	}

	// Update the sensor	
	updateSensor();

	ImageType::Type type = ImageType::Depth;
	ImageNumChannels::Channel numChannels = ImageNumChannels::C4;
	unsigned char* buffer = g_depthData;
	if (g_showRGBImage)
	{
		buffer = g_rgbData;
		type = ImageType::Color;
		numChannels = ImageNumChannels::C3;
	}
	else if (g_showIRImage)
	{
		buffer = g_irData;
		type = ImageType::IR;
	}
	unsigned int options = RenderOptions::None;
	DepthImageModification::Modification mod = DepthImageModification::UseHistogram;
	if (g_showInfo == 0)
		options = RenderOptions::Shapes | RenderOptions::UserCaptions | RenderOptions::Skeletons | RenderOptions::FingerShapes | RenderOptions::Background | RenderOptions::DetailedFaceShapes;
	else if (g_showInfo == 1)
		options = RenderOptions::Shapes | RenderOptions::LocalOrientCaptions | RenderOptions::Skeletons;
	else if (g_showInfo == 2)
		mod = DepthImageModification::ConvertToRGB;
	else if (g_showInfo == 3)
		mod = DepthImageModification::StretchValueRange;

	getImage(buffer, type, numChannels, ImageDepth::D8, options, mod);

	// Clear the OpenGL buffers
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, (double)dWidth, (double)dHeight, 0, -1.0, 1.0);


	// Create the OpenGL texture map
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (g_showRGBImage)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgbWidth, rgbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, g_rgbData);
	else if (g_showIRImage)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, irWidth, irHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, g_irData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dWidth, dHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, g_depthData);

	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glBegin(GL_QUADS);

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f(1.0f, 0);
	glVertex2f((float)dWidth, 0);
	// bottom right
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f((float)dWidth, (float)dHeight);
	// bottom left
	glTexCoord2f(0, 1.0f);
	glVertex2f(0, (float)dHeight);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Check closest user's gestures
	unsigned int closestID = getClosestUserID();
	if (closestID > 0)
	{
		checkPostures(closestID);
		if (g_showFingerCounts)
		{
			/*static int pause = 0;
			if (pause == 0)
			{
				printf("User %d finger count: left=%d, right=%d\n", closestID, getFingerCount(closestID, true, g_useOldFingerCounts), getFingerCount(closestID, false, g_useOldFingerCounts));
			}
			pause = (pause + 1) % 20;*/
			printf("User %d finger count right=%d\n", closestID, getFingerCount(closestID, false, g_useOldFingerCounts));
		}
	}

	// Swap the OpenGL display buffers
	glutSwapBuffers();

	if (g_takePictures)
	{
		static int pause = 0;
		if (pause == 0)
		{
			static int num = 0;
			std::stringstream str;
			str << "rgbImage" << num << ".jpg";
			saveImage(str.str().c_str(), 95, ImageType::Color, ImageNumChannels::C3, ImageDepth::D8, RenderOptions::None, DepthImageModification::Raw, 1, SkeletonJoint::HEAD);
			str.str("");
			str << "depthImage" << num << ".png";
			saveImage(str.str().c_str(), 95, ImageType::Depth, ImageNumChannels::C1, ImageDepth::D16, RenderOptions::None, DepthImageModification::StretchValueRange);
			str.str("");
			str << "trackingImage" << num << ".png";
			saveImage(str.str().c_str(), 95, ImageType::Depth, ImageNumChannels::C3, ImageDepth::D8, RenderOptions::Shapes | RenderOptions::Background | RenderOptions::Skeletons | RenderOptions::FingerShapes);
			num++;
		}
		pause = (pause + 1) % 30;
	}
}

// Glut keyboards callback
void glutKeyboard (unsigned char key, int x, int y)
{
	//printf("key: %d\n", key);
	switch (key)
	{
	case 27: //ESC
		g_exitNextFrame = true;
		break;
	case ' ':
		{
			g_recognitionOutputMode = (g_recognitionOutputMode+1) % g_numOutputmodes;
			char* modeName = "Unknown";
			switch (g_recognitionOutputMode)
			{
				case 0: 
					modeName = "User defined joint relations, orientation, and linear gestures (categorie 1 + 2)";
					break;
				case 1: 
					modeName = "User defined posture combinations (categorie 3";
					break;
				case 2: 
					modeName = "Posture combinations (categorie 3)";
					break;
				case 3: 
					modeName = "Static Postures::Posture (categorie 1)";
					break;
				case 4: 
					modeName = "Special: Pointing gestures (categorie 1)";
					break;
			}
			std::cout << "Check recognizer mode: " << g_recognitionOutputMode << "- " << modeName << std::endl;
		}
		break;
	case 'p':
		{
			g_takePictures = !g_takePictures;
		}
		break;
	case 'r':
		{
			g_showRGBImage = !g_showRGBImage;
		}
		break;
	case 'i':
		{
			g_showIRImage = !g_showIRImage;

		}
		break;
	case 'f':
		{
			g_showFingerCounts = !g_showFingerCounts;
		}
		break;
	case 'c':
		{
			g_useOldFingerCounts = !g_useOldFingerCounts;
		}
		break;
	case 't':
		{
			g_showInfo = (g_showInfo+1) % 4;
		}
		break;
	case 's':
		{
			SensorType::Type type = Fubi::getCurrentSensorType();
			bool succes = false;
			while (!succes)
			{
				if (type == SensorType::NONE)
					type = SensorType::OPENNI2;
				else if (type == SensorType::OPENNI2)
					type = SensorType::OPENNI1;
				else if (type == SensorType::OPENNI1)
					type = SensorType::KINECTSDK;
				else if (type == SensorType::KINECTSDK)
					type = SensorType::NONE;
			
				succes = Fubi::switchSensor(SensorOptions(StreamOptions(), StreamOptions(), StreamOptions(-1, -1, -1), type));
				if (type == SensorType::NONE)
					break;	// None should always be succesful so we ensure termination of this loop
			}
		}
		break;
	case 9: //TAB
		{
			// Reload recognizers from xml
			clearUserDefinedRecognizers();
			if (loadRecognizersFromXML("SampleRecognizers.xml"))
			{
				printf("Succesfully reloaded recognizers xml!\n");
			}
		}
	}
}

int main(int argc, char ** argv)
{
	// This init is only valid when using OpenNI version 1.x
	/*if (!init("SamplesConfig.xml"))
		return false;*/

	// Alternative init without xml
	init(SensorOptions());

	//Alternative config if you use the Microsoft Kinect SDK bridge to OpenNI
	//init("MSSDKConfig.xml");

	getDepthResolution(dWidth, dHeight);
	getRgbResolution(rgbWidth, rgbHeight);
	getIRResolution(irWidth, irHeight);

	g_depthData = new unsigned char[dWidth*dHeight*4];
	if ( rgbWidth > 0 && rgbHeight > 0)
		g_rgbData = new unsigned char[rgbWidth*rgbHeight*3];
	if (irWidth > 0 && irHeight > 0)
		g_irData = new unsigned char[irWidth*irHeight*4];

	memset(g_currentPostures, 0, sizeof(g_currentPostures));

	// All known combination recognizers will be started automatically for new users
	setAutoStartCombinationRecognition(true);

	// Add a sample joint relation rec (left hand in front)
	/*unsigned int index = addJointRelationRecognizer(SkeletonJoint::LEFT_HAND, SkeletonJoint::LEFT_SHOULDER, 
	Vec3f(-Math::MaxFloat, -Math::MaxFloat, -Math::MaxFloat), Vec3f(Math::MaxFloat, Math::MaxFloat, -300.0f), 
	500.0f, Math::MaxFloat, false, false, -1, "Left hand in front");
	g_currentUserDefinedRecognizers.push_back(false);*/

	// And a sample linear gesture rec (left hand velocity 1 m/s in upper left direction)
	/*index = addLinearMovementRecognizer(SkeletonJoint::LEFT_HAND, SkeletonJoint::LEFT_SHOULDER, Vec3f(-1.f, 1.f , 0), 1000, 
	Math::MaxFloat, false, false, -1, "Hand upper left move");
	g_currentUserDefinedRecognizers.push_back(false);*/
	// Note: normally you would save the index for later calls to isUserDefinedGestureDetected(...)


	loadRecognizersFromXML("SampleRecognizers.xml");

	#if defined ( WIN32 ) || defined( _WINDOWS )
		SetWindowPos( GetConsoleWindow(), HWND_TOP, dWidth+10, 0, 0, 0,
                         SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER );
	#endif

	// OpenGL init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(dWidth, dHeight);
	glutCreateWindow ("FUBI - Recognizer OpenGL test");
	//glutFullScreen();
	//glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	// Per frame code is in glutDisplay
	glutMainLoop();

	release();
	delete[] g_depthData;
	delete[] g_rgbData;
	delete[] g_irData;
	return 0;
}
