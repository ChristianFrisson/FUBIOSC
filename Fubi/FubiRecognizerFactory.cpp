// ****************************************************************************************
//
// Fubi Recognizer Factory
// ---------------------------------------------------------
// Copyright (C) 2010-2013 Felix Kistler 
// 
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/org/documents/epl-v10.html
// 
// ****************************************************************************************

#include "FubiRecognizerFactory.h"

// The enum for the recognizer ids
#include "Fubi.h"

// The generic posture recognizer
#include "GestureRecognizer/JointRelationRecognizer.h"

// The generic posture orienation recognizer
#include "GestureRecognizer/JointOrientationRecognizer.h"

// And linear gesture recognizers
#include "GestureRecognizer/LinearMovementRecognizer.h"

// And finger count recognizers
#include "GestureRecognizer/FingerCountRecognizer.h"

// Posture combination recognizers
#include "GestureRecognizer/CombinationRecognizer.h"

// All specific posture recognizers
#include "GestureRecognizer/RightHandUpRecognizer.h"
#include "GestureRecognizer/LeftHandUpRecognizer.h"
#include "GestureRecognizer/ArmsCrossedRecognizer.h"
#include "GestureRecognizer/ArmsNearPocketsRecognizer.h"
#include "GestureRecognizer/ArmsDownTogetherRecognizer.h"
#include "GestureRecognizer/RightHandOutRecognizer.h"
#include "GestureRecognizer/LeftHandOutRecognizer.h"
#include "GestureRecognizer/HandsFrontTogetherRecognizer.h"
#include "GestureRecognizer/LeftKneeUpRecognizer.h"
#include "GestureRecognizer/RightKneeUpRecognizer.h"
#include "GestureRecognizer/LeftHandOverHeadRecognizer.h"
#include "GestureRecognizer/RightHandOverHeadRecognizer.h"
#include "GestureRecognizer/RightHandLeftOfShoulderRecognizer.h"
#include "GestureRecognizer/RightHandRightOfShoulderRecognizer.h"
#include "GestureRecognizer/RightHandPointingRecognizer.h"
#include "GestureRecognizer/RightHandCloseToArmRecognizer.h"
#include "GestureRecognizer/LeftHandCloseToArmRecognizer.h"

using namespace std;

namespace Fubi
{
	IGestureRecognizer* createRecognizer(Postures::Posture postureID)
	{
		switch (postureID)
		{
		case Postures::RIGHT_HAND_OVER_SHOULDER:
			return new RightHandUpRecognizer();
		case Postures::LEFT_HAND_OVER_SHOULDER:
			return new LeftHandUpRecognizer();
		case Postures::ARMS_CROSSED:
			return new ArmsCrossedRecognizer();
		case Postures::ARMS_NEAR_POCKETS:
			return new ArmsNearPocketsRecognizer();
		case Postures::ARMS_DOWN_TOGETHER:
			return new ArmsDownTogetherRecognizer();
		case Postures::LEFT_HAND_OUT:
			return new LeftHandOutRecognizer();
		case Postures::RIGHT_HAND_OUT:
			return new RightHandOutRecognizer();
		case Postures::HANDS_FRONT_TOGETHER:
			return new HandsFrontTogetherRecognizer();
		case Postures::LEFT_KNEE_UP:
			return new LeftKneeUpRecognizer();
		case Postures::RIGHT_KNEE_UP:
			return new RightKneeUpRecognizer();
		case Postures::LEFT_HAND_OVER_HEAD:
			return new LeftHandOverHeadRecognizer();
		case Postures::RIGHT_HAND_OVER_HEAD:
			return new RightHandOverHeadRecognizer();
		case Postures::RIGHT_HAND_LEFT_OF_SHOULDER:
			return new RightHandLeftOfShoulderRecognizer();
		case Postures::RIGHT_HAND_RIGHT_OF_SHOULDER:
			return new RightHandRightOfShoulderRecognizer();
		case Postures::POINTING_RIGHT:
			return new RightHandPointingRecognizer();
		case Postures::RIGHT_HAND_CLOSE_TO_ARM:
			return new RightHandCloseToArmRecognizer();
		case Postures::LEFT_HAND_CLOSE_TO_ARM:
			return new LeftHandCloseToArmRecognizer();			
		}
		return 0x0;
	}

	IGestureRecognizer* createRecognizer(Fubi::SkeletonJoint::Joint joint, Fubi::SkeletonJoint::Joint relJoint,
		const Fubi::Vec3f& minValues /*= DefaultMinVec*/, 
		const Fubi::Vec3f& maxValues /*= DefaultMaxVec*/, 
		float minDistance /*= 0*/, 
		float maxDistance /*= Fubi::Math::MaxFloat*/,
		bool useLocalPositions /*= false*/,
		float minConfidence /*= -1.0f*/,
		Fubi::BodyMeasurement::Measurement measuringUnit /*= Fubi::BodyMeasurement::NUM_MEASUREMENTS*/)
	{
		return new JointRelationRecognizer(joint, relJoint, minValues, maxValues, 
			minDistance, maxDistance, useLocalPositions, minConfidence, measuringUnit);
	}

	IGestureRecognizer* createRecognizer(Fubi::SkeletonJoint::Joint joint,
		const Fubi::Vec3f& minValues /*= Fubi::Vec3f(-180.0f, -180.0f, -180.0f)*/, 
		const Fubi::Vec3f& maxValues /*= Fubi::Vec3f(180.0f, 180.0f, 180.0f)*/,
		bool useLocalOrientations /*= true*/,
		float minConfidence /*= -1.0f*/)
	{
		return new JointOrientationRecognizer(joint, minValues, maxValues, useLocalOrientations, minConfidence);
	}

	IGestureRecognizer* createRecognizer(Fubi::SkeletonJoint::Joint joint, Fubi::SkeletonJoint::Joint relJoint,
		const Fubi::Vec3f& direction, 
		float minVel, float maxVel /*= Fubi::Math::MaxFloat*/,
		bool useLocalPositions /*= false*/,
		float minConfidence /*= -1.0f*/,
		float maxAngleDiff /*= 45.0f*/, 
		bool useOnlyCorrectDirectionComponent /*= true*/)
	{
		return new LinearMovementRecognizer(joint, relJoint, direction, minVel, maxVel, useLocalPositions, minConfidence, maxAngleDiff, useOnlyCorrectDirectionComponent);
	}

	IGestureRecognizer* createRecognizer(Fubi::SkeletonJoint::Joint joint,
		const Fubi::Vec3f& direction, 
		float minVel, float maxVel /*= Fubi::Math::MaxFloat*/,
		bool useLocalPositions /*= false*/,
		float minConfidence /*= -1.0f*/,
		float maxAngleDiff /*= 45.0f*/, 
		bool useOnlyCorrectDirectionComponent /*= true*/)
	{
		return new LinearMovementRecognizer(joint, direction, minVel, maxVel, useLocalPositions, minConfidence, maxAngleDiff, useOnlyCorrectDirectionComponent);
	}

	IGestureRecognizer* createRecognizer(Fubi::SkeletonJoint::Joint handJoint /*= RIGHT_HAND*/,
		unsigned int minFingers /*= 0*/, unsigned int maxFingers /*= 5*/,
		float minConfidence /*= -1.0f*/, bool useMedianCalculation /*= false*/)
	{
		return new FingerCountRecognizer(handJoint, minFingers, maxFingers, minConfidence, useMedianCalculation);
	}


	CombinationRecognizer* createRecognizer(FubiUser* user, Combinations::Combination postureID)
	{
		CombinationRecognizer* rec = new CombinationRecognizer(user, postureID);
		switch (postureID)
		{
		case Combinations::WAVE_RIGHT_HAND_OVER_SHOULDER:
			{
				vector<IGestureRecognizer*> state1, state2, state3, state4;
				
				state1.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				state2.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				state3.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				state4.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));

				state1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(-1, 0, 0), 250.f));
				state3.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(-1, 0, 0), 250.f));

				state2.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(1, 0, 0), 250.f));
				state4.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(1, 0, 0), 250.f));

				// Waving with right hand over shoulder must be 2 x left and 2 x right, each at least 0.1 sec hold, at max 1.2 sec
				// and at maximum 0.3 sec between the states for transition
				rec->addState(state1, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(state2, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(state3, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(state4, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				return rec;
			}
			break;
		case Combinations::WAVE_RIGHT_HAND:
			{
				// Move left/right := always 250 mm/sec in left/right (the actual waving)
				// And not more than 1200 mm/sec in y or z direction (only to filter out strong movements on those axes)
				vector<IGestureRecognizer*> leftpostures;
				leftpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(-1, 0, 0), 250.f));
				leftpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 1, 0), -1200.f, 1200.f));
				leftpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, 1), -1200.f, 1200.f));

				vector<IGestureRecognizer*> rightpostures;
				rightpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(1, 0, 0), 250.f));
				rightpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 1, 0), -1200.f, 1200.f));
				rightpostures.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, 1), -1200.f, 1200.f));

				vector<IGestureRecognizer*> leftpostures1;
				leftpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(-1, 0, 0), 250.f));
				leftpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 1, 0), -1200.f, 1200.f));
				leftpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, 1), -1200.f, 1200.f));

				vector<IGestureRecognizer*> rightpostures1;
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(1, 0, 0), 250.f));
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 1, 0), -1200.f, 1200.f));
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, 1), -1200.f, 1200.f));


				// Waving with right hand over shoulder must be 2 x left and 2 x right, each movement at least 0.1 sec long at max 1.2 sec
				// and at maximum of 0.3 sec between the states for transition
				rec->addState(rightpostures, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(leftpostures, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(rightpostures1, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				rec->addState(leftpostures1, RecognitionState::s_emptyRecVec, 0.1, 1.2, 0.3);
				return rec;
			}
			break;
		case Combinations::CLIMBING_HANDS:
			{
				vector<IGestureRecognizer*> leftpostures1;
				leftpostures1.push_back(createRecognizer(Postures::LEFT_HAND_OVER_SHOULDER));
				leftpostures1.push_back(createRecognizer(SkeletonJoint::LEFT_HAND, SkeletonJoint::LEFT_SHOULDER, Vec3f(0, -1, 0), 250.f));

				vector<IGestureRecognizer*> leftpostures2;
				leftpostures2.push_back(createRecognizer(Postures::LEFT_HAND_OVER_SHOULDER));
				leftpostures2.push_back(createRecognizer(SkeletonJoint::LEFT_HAND, SkeletonJoint::LEFT_SHOULDER, Vec3f(0, -1, 0), 250.f));

				vector<IGestureRecognizer*> rightpostures1;
				rightpostures1.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, -1, 0), 250.f));

				vector<IGestureRecognizer*> rightpostures2;
				rightpostures2.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				rightpostures2.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, -1, 0), 250.f));

				// Climbing hand movement must be 2 x left hand and 2 x right hand moving down over the shoulder, each at least 0.05 sec long
				// at max 1.2 sec and at maximum 0.3 sec between the states for transition
				rec->addState(rightpostures1, RecognitionState::s_emptyRecVec, 0.05, 1.2, 0.3);
				rec->addState(leftpostures1, RecognitionState::s_emptyRecVec, 0.05, 1.2, 0.3);
				rec->addState(rightpostures2, RecognitionState::s_emptyRecVec, 0.05, 1.2, 0.3);
				rec->addState(leftpostures2, RecognitionState::s_emptyRecVec, 0.05, 1.2, 0.3);
				return rec;
			}
			break;

		case Combinations::THROWING_RIGHT:
			{
				vector<IGestureRecognizer*> rightpostures1;
				// Above the shoulder
				rightpostures1.push_back(createRecognizer(Postures::RIGHT_HAND_OVER_SHOULDER));
				// Movement to front
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, -1), 200.f));
				// Behind the shoulder
				rightpostures1.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(-Math::MaxFloat, -Math::MaxFloat, -100.0f))); 


				vector<IGestureRecognizer*> rightpostures2;
				// Movement to front stops
				rightpostures2.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Vec3f(0, 0, -1), -Math::MaxFloat, 100.f));
				// In front of the shoulder
				rightpostures2.push_back(createRecognizer(SkeletonJoint::RIGHT_HAND, SkeletonJoint::RIGHT_SHOULDER, Fubi::DefaultMinVec, Vec3f(Math::MaxFloat, Math::MaxFloat, -120.0f)));

				// Throwing with right hand is: hand behind and above the shoulder moving to front and slowing down in front of the shoulder
				rec->addState(rightpostures1, RecognitionState::s_emptyRecVec, 0, 0.5, 0.7);
				rec->addState(rightpostures2, RecognitionState::s_emptyRecVec, 0);

				return rec;
			}
			break;

		case Combinations::BALANCING:
			{
				vector<IGestureRecognizer*> state1;
				// Left hand out
				state1.push_back(createRecognizer(Postures::LEFT_HAND_OUT));
				// Right hand out
				state1.push_back(createRecognizer(Postures::RIGHT_HAND_OUT));

				rec->addState(state1, RecognitionState::s_emptyRecVec, 1.0f);

				return rec;
			}
			break;
		}
		// No recognizer found
		delete rec;
		return 0x0;
	}
}
