// ****************************************************************************************
//
// Fubi - Predefined posture and gesture ids and names
// -------------------------------------------------------------
// Copyright (C) 2010-2011 Felix Kistler 
// 
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/org/documents/epl-v10.html
//
// 
// ****************************************************************************************

using System;

namespace FubiNET
{
    public static class FubiPredefinedGestures
    {

        // All predefined postures that can be recognized
        public enum Postures
        {
            RIGHT_HAND_OVER_SHOULDER = 0,
            LEFT_HAND_OVER_SHOULDER,
            ARMS_CROSSED,
            ARMS_NEAR_POCKETS,
            ARMS_DOWN_TOGETHER,
            RIGHT_HAND_OUT,
            LEFT_HAND_OUT,
            HANDS_FRONT_TOGETHER,
            LEFT_KNEE_UP,
            RIGHT_KNEE_UP,
            RIGHT_HAND_OVER_HEAD,
            LEFT_HAND_OVER_HEAD,
            RIGHT_HAND_LEFT_OF_SHOULDER,
            RIGHT_HAND_RIGHT_OF_SHOULDER,
            POINTING_RIGHT,
            RIGHT_HAND_CLOSE_TO_ARM,
            LEFT_HAND_CLOSE_TO_ARM,
            NUM_POSTURES
        };

        // All predefined combinations that can be recognized
        public enum Combinations
        {
            WAVE_RIGHT_HAND_OVER_SHOULDER = 0,
            WAVE_RIGHT_HAND,
            CLIMBING_HANDS,
            THROWING_RIGHT,
            BALANCING,
            NUM_COMBINATIONS,
        };

        // String names for each posture
        public static string getPostureName(Postures postureID)
        {
            switch (postureID)
            {
                case Postures.RIGHT_HAND_OVER_SHOULDER:
                    return "Right hand over shoulder";
                case Postures.LEFT_HAND_OVER_SHOULDER:
                    return "Left hand over shoulder";
                case Postures.ARMS_CROSSED:
                    return "Arms crossed";
                case Postures.ARMS_NEAR_POCKETS:
                    return "Arms near pockets";
                case Postures.ARMS_DOWN_TOGETHER:
                    return "Arms down together";
                case Postures.RIGHT_HAND_OUT:
                    return "Right hand out";
                case Postures.LEFT_HAND_OUT:
                    return "Left hand out";
                case Postures.HANDS_FRONT_TOGETHER:
                    return "Hands together in front";
                case Postures.LEFT_KNEE_UP:
                    return "Left knee up";
                case Postures.RIGHT_KNEE_UP:
                    return "Right knee up";
                case Postures.RIGHT_HAND_OVER_HEAD:
                    return "Right hand over head";
                case Postures.LEFT_HAND_OVER_HEAD:
                    return "Left hand over head";
                case Postures.RIGHT_HAND_LEFT_OF_SHOULDER:
                    return "Right hand left of shoulder";
                case Postures.RIGHT_HAND_RIGHT_OF_SHOULDER:
                    return "Right hand right of shoulder";
                case Postures.POINTING_RIGHT:
                    return "Right hand pointing gesture";
                case Postures.RIGHT_HAND_CLOSE_TO_ARM:
                    return "Right hand close to arm";
                case Postures.LEFT_HAND_CLOSE_TO_ARM:
                    return "Left hand close to arm";
                case Postures.NUM_POSTURES:
                    return "Invalid posture id";
            }

            return "Unknown posture id";
        }

        // String names for each combination
        public static string getCombinationName(Combinations combinationID)
        {
            switch (combinationID)
            {
                case Combinations.WAVE_RIGHT_HAND_OVER_SHOULDER:
				    return "Waving right hand over shoulder";
                case Combinations.WAVE_RIGHT_HAND:
				    return "Waving right hand";
                case Combinations.CLIMBING_HANDS:
				    return "Climbing hands movement";
                case Combinations.THROWING_RIGHT:
				    return "Throwing with right hand";
                case Combinations.BALANCING:
				    return "Balancing";
                case Combinations.NUM_COMBINATIONS:
				    return "Invalid combination id";
		    }

		return "Unknown combination id";
        }
    };
}