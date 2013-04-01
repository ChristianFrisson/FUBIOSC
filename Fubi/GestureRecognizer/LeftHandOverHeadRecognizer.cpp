// ****************************************************************************************
//
// Posture Recognizers
// ---------------------------------------------------------
// Copyright (C) 2010-2013 Felix Kistler 
// 
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/org/documents/epl-v10.html
// 
// ****************************************************************************************
#include "LeftHandOverHeadRecognizer.h"

using namespace Fubi;

Fubi::RecognitionResult::Result LeftHandOverHeadRecognizer::recognizeOn(FubiUser* user)
{
	const SkeletonJointPosition& leftHand = user->m_currentTrackingData.jointPositions[SkeletonJoint::LEFT_HAND];
	const SkeletonJointPosition& head = user->m_currentTrackingData.jointPositions[SkeletonJoint::HEAD];
	if (leftHand.m_confidence >= m_minConfidence && head.m_confidence >= m_minConfidence)
	{
		if (leftHand.m_position.y > head.m_position.y)
			return Fubi::RecognitionResult::RECOGNIZED;
	}
	else
		return Fubi::RecognitionResult::TRACKING_ERROR;

	return Fubi::RecognitionResult::NOT_RECOGNIZED;
}