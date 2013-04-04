#include "Fubi.h"

// OSC includes
#include "oscpkt/oscpkt.hh"
#include "oscpkt/udp.hh"
//

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

bool g_currentPostures[Fubi::Postures::NUM_POSTURES];
std::vector<bool> g_currentUserDefinedRecognizers;
short g_recognitionOutputMode = 1;
const short g_numOutputmodes = 5;
bool g_takePictures = false;
bool g_exitNextFrame = false;

/////////// OSC defines
#define OSCPKT_OSTREAM_OUTPUT
// OSC global variables
const int OSC_PORT = 9001;
const std::string host = "localhost";
oscpkt::UdpSocket sock;
const char* comboName;
std::vector<std::pair<std::string, std::vector<Fubi::SkeletonJoint::Joint>>> combinationsJoints;
//////////////


// Function called each frame for all tracked users
void checkPostures(unsigned int userID)
{	
//
	std::vector<Fubi::SkeletonJoint::Joint> joints;
	for (unsigned int i= 0; i < getNumUserDefinedCombinationRecognizers(); ++i)
	{
		if (getCombinationRecognitionProgressOn(getUserDefinedCombinationRecognizerName(i), userID) == Fubi::RecognitionResult::RECOGNIZED)
		{
			comboName = getUserDefinedCombinationRecognizerName(i);
			joints = getComboJoints(comboName);
			
			// send over OSC
			oscpkt::Message msg;
			std::string message;
			message += "/FUBI/Combination/";
			message += std::string(comboName);
			msg.init(message);
			
			FubiUser* user = Fubi::getUser(userID);
			int nb = 0;
			for(unsigned int i=0; i<joints.size(); i++)
			{
				const Vec3f& jointPosition = user->m_currentTrackingData.jointPositions[joints[i]].m_position;
				Vec3f jointOrientation = user->m_currentTrackingData.jointOrientations[joints[i]].m_orientation.getRot();
				jointOrientation.normalize();

				msg.pushStr(getJointName(joints[i]));
				
				msg.pushFloat(jointPosition.x);
				msg.pushFloat(jointPosition.y);
				msg.pushFloat(jointPosition.z);
				nb += 3;
				
				// msg.pushFloat(jointOrientation.x);
				// msg.pushFloat(jointOrientation.y);
				// msg.pushFloat(jointOrientation.z);
				// nb += 3;
			}
			oscpkt::PacketWriter pw;
			pw.startBundle().addMessage(msg).endBundle();
			sock.sendPacket(pw.packetData(), pw.packetSize());
			msg.clear();
			std::cout << "sendind OSC message: \"/FUBI/Combination/" << comboName << "\" with " << nb << " parameters" << std::endl;
		}
			
	}
}
//
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
			static int pause = 0;
			if (pause == 0)
			{
				printf("User %d finger count: left=%d, right=%d\n", closestID, getFingerCount(closestID, true, g_useOldFingerCounts), getFingerCount(closestID, false, g_useOldFingerCounts));
			}
			pause = (pause + 1) % 20;
			//printf("User %d finger count right=%d\n", closestID, getFingerCount(closestID, false, g_useOldFingerCounts));
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
//			
			if (loadRecognizersFromXML("TutorialRecognizers.xml"))
			{		
				printf("Succesfully reloaded recognizers xml!\n");
				combinationsJoints = getCombinations();
			}
//
		}
	}
}

int main(int argc, char ** argv)
{
// Initialize UDP socket for OSC
	sock.connectTo(host, OSC_PORT);
	if (!sock.isOk()) {
		std::cerr << "Error connection to port " << OSC_PORT << ": " << sock.errorMessage() << "\n";
	} else {
		std::cout << "Client started, will send packets to port " << OSC_PORT << std::endl;
	}
//	
	// Alternative init without xml
	init(SensorOptions());

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
//
	loadRecognizersFromXML("TutorialRecognizers.xml");
	combinationsJoints = getCombinations();
//
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
