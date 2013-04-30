#include "Fubi.h"

// OSC includes
#include "oscpkt/oscpkt.hh"
#include "oscpkt/udp.hh"
// mapping include
#include "MappingMashtaCycle.h"

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

bool displayImageFromKinect = true;
bool displayOSCMessages = true;
bool sendOSCPosition = true;
bool checkCombinations = true;
bool sendOSCCombinations = true;

short g_showInfo = 0;


bool g_currentPostures[Fubi::Postures::NUM_POSTURES];
std::vector<bool> g_currentUserDefinedRecognizers;
short g_recognitionOutputMode = 1;
const short g_numOutputmodes = 5;
bool g_exitNextFrame = false;

bool trackingStates[16];

/////////// OSC defines
#define OSCPKT_OSTREAM_OUTPUT
// OSC global variables
const int OSC_PORT = 3332;
const std::string host = "192.168.2.255";
oscpkt::UdpSocket sock;
std::string comboName;
bool halfPositionSending = true;
std::vector<std::pair<std::string, std::vector<Fubi::SkeletonJoint::Joint> > > combinationsJoints;
// position filtering
#define FILTER_SIZE 7
float torsoPositionsX[FILTER_SIZE];
float torsoPositionsY[FILTER_SIZE];
float torsoPositionsZ[FILTER_SIZE];
int positionIndex = 0;
MappingMashtaCycle *mapping;

// Function called each frame for all tracked users
void checkPostures(unsigned int userID)
{	
//
	//std::vector<Fubi::SkeletonJoint::Joint> joints;
	FubiUser* user = Fubi::getUser(userID);
	oscpkt::PacketWriter pw;

	for (unsigned int i= 0; i < getNumUserDefinedCombinationRecognizers(); ++i)
	{
		if (getCombinationRecognitionProgressOn(getUserDefinedCombinationRecognizerName(i), userID) == Fubi::RecognitionResult::RECOGNIZED)
		{
			comboName = getUserDefinedCombinationRecognizerName(i);
			//lastComboName = comboName;
			//joints = getComboJoints(comboName);
			
			// send over OSC
/*			if(sendOSCCombinations)
			{
				oscpkt::Message combiMsg;
				std::string combiMessage;
				combiMessage += "/FUBI/Combination/";
				combiMessage += std::string(comboName);
				combiMsg.init(combiMessage);
			
				int nb = 0;
				for(unsigned int i=0; i<joints.size(); i++)
				{
					const Vec3f& jointPosition = user->m_currentTrackingData.jointPositions[joints[i]].m_position;
					Vec3f jointOrientation = user->m_currentTrackingData.jointOrientations[joints[i]].m_orientation.getRot();
					jointOrientation.normalize();

					combiMsg.pushStr(getJointName(joints[i]));
				
					combiMsg.pushFloat(jointPosition.x);
					combiMsg.pushFloat(jointPosition.y);
					combiMsg.pushFloat(jointPosition.z);
					nb += 3;
				
					// msg.pushFloat(jointOrientation.x);
					// msg.pushFloat(jointOrientation.y);
					// msg.pushFloat(jointOrientation.z);
					// nb += 3;
				}
				pw.startBundle().addMessage(combiMsg).endBundle();
				sock.sendPacket(pw.packetData(), pw.packetSize());
				combiMsg.clear();
				if(displayOSCMessages)
					std::cout << "sendind OSC message: \"/FUBI/Combination/" << comboName << "\" with " << nb << " parameters" << std::endl;
			}*/

			MessageToSend msg = mapping->getOSCMessage(user, comboName);
			if(msg.text != "")
			{
				oscpkt::Message combiMsg;
				combiMsg.init(msg.text);
			
				int nb = 0;
				for(unsigned int i=0; i<msg.values.size(); i++)
				{
					combiMsg.pushFloat(msg.values[i]);
					nb++;
				}
				pw.startBundle().addMessage(combiMsg).endBundle();
				sock.sendPacket(pw.packetData(), pw.packetSize());
				combiMsg.clear();
				if(displayOSCMessages)
				{
					std::cout << "sendind OSC message: \"" << msg.text;
					for(unsigned int i=0; i<msg.values.size(); i++)
						std::cout << " " << msg.values[i];
					std::cout << std::endl;
				}
			}
		}
			
	}
//
}

//
void checkTrackingState(std::deque<unsigned int> usersIDs)
{
	FubiUser* user;
	unsigned int userID;
	oscpkt::PacketWriter pw;

	for(unsigned int i=0; i<usersIDs.size(); i++)
	{
		userID = usersIDs[i];
		user = Fubi::getUser(userID);
		if(user->m_isTracked && user->m_inScene && !trackingStates[userID])
		{
			//Tracking starts
			trackingStates[userID] = true;
			//oscpkt::Message trackingMsg;
			//std::string trackingMessage;
			//trackingMessage += "/FUBI/Tracking/User";
			//trackingMsg.init(trackingMessage);
			//trackingMsg.pushInt32(userID);
			//trackingMsg.pushBool(trackingStates[userID]);

			//pw.startBundle().addMessage(trackingMsg).endBundle();
			//sock.sendPacket(pw.packetData(), pw.packetSize());
			//trackingMsg.clear();
			//if(displayOSCMessages)
			//	std::cout << "sendind OSC message: \"/FUBI/Tracking/User " << userID << " " << trackingStates[userID] << "\"" << std::endl;
		}
		if((!user->m_inScene || !user->m_isTracked) && trackingStates[userID])
		{
			// Tracking ends
			trackingStates[userID] = false;
			oscpkt::Message trackingMsg;
			//std::string trackingMessage;
			//trackingMessage += "/FUBI/Tracking/User";
			//trackingMsg.init(trackingMessage);
			//trackingMsg.pushInt32(userID);
			//trackingMsg.pushBool(trackingStates[userID]);

			trackingMsg.init("/mediacycle/browser/0/released");
			pw.startBundle().addMessage(trackingMsg).endBundle();
			sock.sendPacket(pw.packetData(), pw.packetSize());
			trackingMsg.clear();
			if(displayOSCMessages)
				std::cout << "sendind OSC message: \"/mediacycle/browser/0/released\"" << std::endl;		
		}
	}
}

//float simpleAverageFilter(float tab[])
//{
//	float mean = 0;
//	for (int i=0; i<FILTER_SIZE; i++)
//		mean += tab[i];
//	return mean/FILTER_SIZE;
//}
//
//float medianFilter(const float tab[])
//{
//	float medianTab[FILTER_SIZE];
//	for(int i=0; i<FILTER_SIZE; i++)
//		medianTab[i] = tab[i];
//
//	bool sorted = false;
//	float temp = 0;
//	while(!sorted)
//	{
//		sorted = true;
//		for(int j=0; j<FILTER_SIZE-1; j++)
//		{
//			if(medianTab[j] > medianTab[j+1])
//			{
//				temp = medianTab[j+1];
//				medianTab[j+1] = medianTab[j];
//				medianTab[j] = temp;
//				sorted = false;
//			}
//		}
//	}
//	if(FILTER_SIZE %2 == 0)
//		return (medianTab[FILTER_SIZE/2-1]+ medianTab[FILTER_SIZE/2])/2;
//	else
//		return medianTab[FILTER_SIZE/2];
//
//}
//
//void sendXYZTorsoPosition(unsigned int userID)
//{
//	FubiUser* user = Fubi::getUser(userID);
//	const Vec3f& torsoPosition = user->m_currentTrackingData.jointPositions[SkeletonJoint::TORSO].m_position;
//	torsoPositionsX[positionIndex] = torsoPosition.x;
//	torsoPositionsY[positionIndex] = torsoPosition.y;
//	torsoPositionsZ[positionIndex] = torsoPosition.z;
//
//	positionIndex = (positionIndex + 1) % FILTER_SIZE;
//
//	//send half of the values to not overload OSC
//	if(halfPositionSending)
//	{
//		float x, y, z;
//		//test without filter
//		//x = torsoPosition.x;
//		//y = torsoPosition.y
//		//z = torsoPosition.z;
//		
//		// with filter (simple running average)
//		//x = simpleAverageFilter(torsoPositionsX);
//		//y = simpleAverageFilter(torsoPositionsY);
//		//z = simpleAverageFilter(torsoPositionsZ);
//		
//		// with median filter
//		x = medianFilter(torsoPositionsX);
//		y = medianFilter(torsoPositionsY);
//		z = medianFilter(torsoPositionsZ);
//
//		oscpkt::PacketWriter pw;
//		oscpkt::Message trackingMsg;
//		std::string trackingMessage;
//		trackingMessage += "/FUBI/Position";
//		trackingMsg.init(trackingMessage);
//		trackingMsg.pushInt32(userID);
//		trackingMsg.pushFloat(x);
//		trackingMsg.pushFloat(y);
//		trackingMsg.pushFloat(z);
//
//		pw.startBundle().addMessage(trackingMsg).endBundle();
//		sock.sendPacket(pw.packetData(), pw.packetSize());
//		trackingMsg.clear();
//		if(displayOSCMessages)
//			std::cout << "sendind OSC message: \"/FUBI/Position " << userID << " " << x << " " << z << "\"" << std::endl;
//	}
//	halfPositionSending = !halfPositionSending;
//}
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
	if(displayImageFromKinect)
	{
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
	}
//
	// Check users tracking state
	std::deque<unsigned int> usersIDs = getClosestUserIDs(15);
	if(usersIDs.size()>0)
		checkTrackingState(usersIDs);
//
	// Check closest user's gestures
	unsigned int closestID = getClosestUserID();
	if (closestID > 0)
	{
//		if(trackingStates[closestID] && sendOSCPosition)
//			sendXYZTorsoPosition(closestID);
		if(trackingStates[closestID] && checkCombinations)
			checkPostures(closestID);
	}

	// Swap the OpenGL display buffers
	glutSwapBuffers();
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
	case 'm':
		displayOSCMessages = !displayOSCMessages;
		std::cout << "displayOSCMessage: " << displayOSCMessages << std::endl;
		break;
	case 'i':
		displayImageFromKinect = !displayImageFromKinect;
		std::cout << "displayImageFromKinect: " << displayImageFromKinect << std::endl;
		break;
	case 'p':
		sendOSCPosition = !sendOSCPosition;
		std::cout << "sendOSCPosition: " << sendOSCPosition << std::endl;
		break;
	case 'k':
		checkCombinations = !checkCombinations;
		std::cout << "checkCombinations: " << checkCombinations << std::endl;
		break;
	case 'l':
		sendOSCCombinations = !sendOSCCombinations;
		std::cout << "sendOSCCombinations: " << sendOSCCombinations << std::endl;
		break;
	case 'r':
		{
			g_showRGBImage = !g_showRGBImage;
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

// Initialize tracking states for 16 users
	for(int i=0; i<16; i++)
		trackingStates[i] = false;
	
	mapping = new MappingMashtaCycle();
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
    std::string recognizersFile("TutorialRecognizers.xml");
#if defined(__APPLE__) && !defined(USE_DEBUG)
    std::string appPath(argv[0]);
    std::string suffix(".app");
    size_t appNamePos = appPath.find(suffix.c_str());
    std::string appName = appPath.substr(0,appNamePos + suffix.size());
    recognizersFile = appName + std::string("/Contents/MacOS/") + recognizersFile;
#endif

    bool recognizersLoaded = loadRecognizersFromXML(recognizersFile.c_str());
    if(recognizersLoaded)
        std::cout << "Loaded ";
    else
        std::cout << "Couldn't load ";
    std::cout << "the recognizers from xml file " << recognizersFile << std::endl;

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
