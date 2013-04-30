#include "MappingMashtaCycle.h"
#include <iostream>
#include "Fubi.h"

using namespace Fubi;

MappingMashtaCycle::MappingMashtaCycle(void):sceneWidth(2.0), sceneDepth(1.5), sceneDepthOffset(2.0)
{
	initMapping();
}

MappingMashtaCycle::MappingMashtaCycle(float sw, float sd, float sdo):sceneWidth(sw), sceneDepth(sd), sceneDepthOffset(sdo)
{
	initMapping();
}


MappingMashtaCycle::~MappingMashtaCycle(void)
{
}

void MappingMashtaCycle::initMapping()
{
	mapping["RightHandPushAboveShoulder"] = SoundControl::LOOP;
	mapping["LeftHandWavingAboveHead"] = SoundControl::STOP;
	mapping["RightHandNearHead"] = SoundControl::REVERB_FREEZE;
	mapping["RightHandNearLeftArm"] = SoundControl::VOLUME;
	mapping["BothHandsInFront"] = SoundControl::SPEED;
	mapping["LeftHandOrientation3"] = SoundControl::REVERB_MIX;
	mapping["LeftHandScanning"] = SoundControl::PAN;
	mapping["BothHandsDown"] = SoundControl::POSITION;
}

MessageToSend MappingMashtaCycle::getOSCMessage(FubiUser* user, std::string comboName)
{
	MessageToSend mts;
	mts.text = "";

	std::map<std::string, SoundControl>::iterator  it = mapping.find(comboName);
	

    if(it == mapping.end())
    {
        std::cout << "Combination " << comboName << " not found " << std::endl;
		return mts;
    }
    
	switch(it->second)
	{
		case SoundControl::LOOP:
			mts = loopMessage();
			break;
		case SoundControl::STOP:
			mts = stopMessage();
			break;
		case SoundControl::REVERB_FREEZE:
			mts = reverbFreezeMessage();
			break;
		case SoundControl::VOLUME:
			mts = volumeMessage(user);
			break;
		case SoundControl::SPEED:
			mts = speedMessage(user);
			break;
		case SoundControl::REVERB_MIX:
			mts = reverbMixMessage(user);
			break;
		case SoundControl::PAN:
			mts = panMessage(user);
			break;
		case SoundControl::POSITION:
			mts = positionMessage(user);
			break;
		default:
			break;
	}
	return mts;
}

int MappingMashtaCycle::boundValue(float *value, float up, float low)
{
	if(*value > up)
	{
		*value = up;
		return 1;
	}
	if(*value < low)
	{
		*value = low;
		return -1;
	}
	else
		return 0;
}

MessageToSend MappingMashtaCycle::loopMessage()
{
	MessageToSend mts;
	mts.text = "/mediacycle/pointer/0/loop";
	return mts;
}

MessageToSend MappingMashtaCycle::stopMessage()
{
	MessageToSend mts;
	mts.text = "/mediacycle/pointer/0/mute";
	return mts;
}

MessageToSend MappingMashtaCycle::reverbFreezeMessage()
{
	/* TODO */
	MessageToSend mts;
	mts.text = "";
	if(!reverbFreeze)
	{
		reverbFreeze = true;
		mts.text = "/mediacycle/pointer/0/reverb_freeze";
		mts.values.push_back(1);
	}
	return mts;
}

MessageToSend MappingMashtaCycle::volumeMessage(FubiUser* user)
{
	MessageToSend mts;
	float volume;

	mts.text = "/mediacycle/pointer/0/playback_volume";
	float leftHandY = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_HAND].m_position.y;
	float rightHandY = user->m_currentTrackingData.jointPositions[SkeletonJoint::RIGHT_HAND].m_position.y;
	float leftElbowY = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_ELBOW].m_position.y;

	volume = (rightHandY-leftElbowY) / (leftHandY-leftElbowY);
	boundValue(&volume, 1, 0);
	mts.values.push_back(volume);

	return mts;
}

MessageToSend MappingMashtaCycle::speedMessage(FubiUser* user)
{
	MessageToSend mts;
	float speed;

	mts.text = "/mediacycle/pointer/0/playback_speed";
	float leftHandY = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_HAND].m_position.y;
	float rightHandY = user->m_currentTrackingData.jointPositions[SkeletonJoint::RIGHT_HAND].m_position.y;

	speed = (rightHandY-leftHandY)/100 + 1;
	boundValue(&speed, 5, -3);
	mts.values.push_back(speed);

	return mts;
}

MessageToSend MappingMashtaCycle::reverbMixMessage(FubiUser* user)
{
	MessageToSend mts;
	float reverbMix;

	mts.text = "/mediacycle/pointer/0/reverb_mix";
	float leftHandY = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_HAND].m_position.y;
	float leftShoulderY = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_SHOULDER].m_position.y;

	reverbMix = 1 - (leftShoulderY-leftHandY)/100;
	boundValue(&reverbMix, 1, 0);
	mts.values.push_back(reverbMix);

	return mts;
}

MessageToSend MappingMashtaCycle::panMessage(FubiUser* user)
{
	MessageToSend mts;
	float pan;

	mts.text = "/mediacycle/pointer/0/playback_pan";
	float leftHandX = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_HAND].m_position.x;
	float leftShoulderX = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_SHOULDER].m_position.x;

	pan = (leftHandX-leftShoulderX+100)/300;
	boundValue(&pan, 1, -1);
	mts.values.push_back(pan);

	return mts;
}

MessageToSend MappingMashtaCycle::positionMessage(FubiUser* user)
{
	MessageToSend mts;
	if(reverbFreeze)
	{
		reverbFreeze = false;
		mts.text = "/mediacycle/pointer/0/reverb_freeze";
		mts.values.push_back(0);
	}
	else
	{	
		float xPos, yPos;

		mts.text = "/mediacycle/browser/0/hover/xy";
		float x = user->m_currentTrackingData.jointPositions[SkeletonJoint::TORSO].m_position.x;
		float z = user->m_currentTrackingData.jointPositions[SkeletonJoint::TORSO].m_position.z;

		xPos = x/(500*sceneWidth);
		yPos = 1-(z-1000*sceneDepthOffset)/(500*sceneDepth);

		boundValue(&xPos, 1, -1);
		boundValue(&yPos, 1, -1);
		mts.values.push_back(xPos);
		mts.values.push_back(yPos);
	}
	return mts;
}