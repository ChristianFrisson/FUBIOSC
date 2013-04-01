// ****************************************************************************************
//
// Fubi CS Wrapper
// ---------------------------------------------------------
// Copyright (C) 2010-2011 Felix Kistler 
// 
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/org/documents/epl-v10.html
// 
// ****************************************************************************************

using System;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;
using System.Collections.Generic;

/**
 * \namespace FubiNET
 *
 * \brief The FubiNET namespace wraps the C++ functions of Fubi for more details have a look at the C++ docu
 *
 */
namespace FubiNET
{

    public class Fubi
    {
        public static bool init(string openniXmlconfig, FubiUtils.SkeletonProfile profile = FubiUtils.SkeletonProfile.ALL, bool mirrorStream = true, float smoothing = 0)
        {
            bool ret = true;
            if (!isInitialized())
            {
                IntPtr openNiXmlPtr = Marshal.StringToHGlobalAnsi(openniXmlconfig);
                ret = FubiInternal.init(openNiXmlPtr, profile, mirrorStream, smoothing);
                Marshal.FreeHGlobal(openNiXmlPtr);
            }
            return ret;
        }

        public static bool init(FubiUtils.SensorOptions options)
        {
            bool ret = true;
            if (!isInitialized())
            {
                ret = FubiInternal.init(options.m_depthOptions.m_width, options.m_depthOptions.m_height, options.m_depthOptions.m_fps,
                    options.m_rgbOptions.m_width, options.m_rgbOptions.m_height, options.m_rgbOptions.m_fps,
                    options.m_irOptions.m_width, options.m_irOptions.m_height, options.m_irOptions.m_fps,
                    options.m_type,
                    options.m_profile, options.m_mirrorStreams, options.m_smoothing);
            }
            return ret;
        }

        public static bool switchSensor(FubiUtils.SensorOptions options)
        {
            return FubiInternal.switchSensor(options.m_type, options.m_depthOptions.m_width, options.m_depthOptions.m_height, options.m_depthOptions.m_fps,
                    options.m_rgbOptions.m_width, options.m_rgbOptions.m_height, options.m_rgbOptions.m_fps,
                    options.m_irOptions.m_width, options.m_irOptions.m_height, options.m_irOptions.m_fps,
                    options.m_profile, options.m_mirrorStreams, options.m_smoothing);
        }

        public static int getAvailableSensors()
        {
            return FubiInternal.getAvailableSensorTypes();
        }

        public static FubiUtils.SensorType getCurrentSensorType()
        {
            return FubiInternal.getCurrentSensorType();
        }

        public static void release()
        {
            FubiInternal.release();
        }

        public static void updateSensor()
        {
            FubiInternal.updateSensor();
        }

        public static void getImage(byte[] outputImage, FubiUtils.ImageType type, FubiUtils.ImageNumChannels numChannels, FubiUtils.ImageDepth depth,
            uint renderOptions = (uint)FubiUtils.RenderOptions.Default,
            FubiUtils.DepthImageModification depthModifications = FubiUtils.DepthImageModification.UseHistogram,
            uint userId = 0, FubiUtils.SkeletonJoint jointOfInterest = FubiUtils.SkeletonJoint.NUM_JOINTS)
        {
            if (outputImage != null)
            {
                int size = outputImage.Length + 1;

                // allocate memory for resource data
                IntPtr ptr = Marshal.AllocHGlobal(size);

                // load resource
                FubiInternal.getImage(ptr, type, numChannels, depth, renderOptions, depthModifications, userId, jointOfInterest);

                // copy byte data into allocated memory
                Marshal.Copy(ptr, outputImage, 0, outputImage.Length);


                Marshal.WriteByte(ptr, size, 0x00);
                // free previously allocated memory
                Marshal.FreeHGlobal(ptr);

                return;
            }
        }


        public static FubiUtils.RecognitionResult recognizeGestureOn(FubiPredefinedGestures.Postures postureID, UInt32 userID)
        {
            return FubiInternal.recognizeGestureOn(postureID, userID);
        }

        public static FubiUtils.RecognitionResult recognizeGestureOn(UInt32 recognizerIndex, UInt32 userID)
        {
            return FubiInternal.recognizeGestureOn(recognizerIndex, userID);
        }

        public static UInt32 getUserID(UInt32 index)
        {
            return FubiInternal.getUserID(index);
        }

        public static void setAutoStartCombinationRecognition(bool enable, FubiPredefinedGestures.Combinations combinationID = FubiPredefinedGestures.Combinations.NUM_COMBINATIONS)
        {
            FubiInternal.setAutoStartCombinationRecognition(enable, combinationID);
        }

        public static bool getAutoStartCombinationRecognition(FubiPredefinedGestures.Combinations combinationID = FubiPredefinedGestures.Combinations.NUM_COMBINATIONS)
        {
            return FubiInternal.getAutoStartCombinationRecognition(combinationID);
        }

        public static void enableCombinationRecognition(FubiPredefinedGestures.Combinations combinationID, UInt32 userID, bool enable)
        {
            FubiInternal.enableCombinationRecognition(combinationID, userID, enable);
        }

        public static FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(FubiPredefinedGestures.Combinations combinationID, UInt32 userID, bool restart = true)
        {
            return FubiInternal.getCombinationRecognitionProgressOn(combinationID, userID, new IntPtr(0), restart);
        }

        public static FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(FubiPredefinedGestures.Combinations combinationID, UInt32 userID, out FubiTrackingData[] userStates, bool restart = true)
        {
            IntPtr vec = FubiInternal.createTrackingDataVector();
            FubiUtils.RecognitionResult recognized = FubiInternal.getCombinationRecognitionProgressOn(combinationID, userID, vec, restart);
            if (recognized == FubiUtils.RecognitionResult.RECOGNIZED)
            {
                UInt32 size = FubiInternal.getTrackingDataVectorSize(vec);
                userStates = new FubiTrackingData[size];
                for (UInt32 i = 0; i < size; i++)
                {
                    IntPtr tInfo = FubiInternal.getTrackingData(vec, i);
                    FubiTrackingData info = new FubiTrackingData();
                    for (UInt32 j = 0; j < (uint)FubiUtils.SkeletonJoint.NUM_JOINTS; ++j)
                    {
                        FubiInternal.getSkeletonJointPosition(tInfo, (FubiUtils.SkeletonJoint)j, out info.jointPositions[j].x, out info.jointPositions[j].y, out info.jointPositions[j].z, out info.jointPositions[j].confidence, out info.timeStamp);
                        double timeStamp = 0;
                        FubiInternal.getSkeletonJointOrientation(tInfo, (FubiUtils.SkeletonJoint)j, info.jointOrientations[j].rotMat, out info.jointPositions[j].confidence, out timeStamp);
                        info.timeStamp = Math.Max(timeStamp, info.timeStamp);
                    }
                    userStates[i] = info;
                }
                FubiInternal.releaseTrackingDataVector(vec);
            }
            else
                userStates = null;
            return recognized;
        }

        public static bool loadRecognizersFromXML(string fileName)
        {
            IntPtr fileNamePtr = Marshal.StringToHGlobalAnsi(fileName);
            bool ret = FubiInternal.loadRecognizersFromXML(fileNamePtr);
            Marshal.FreeHGlobal(fileNamePtr);
            return ret;
        }

        public static FubiUtils.RecognitionResult recognizeGestureOn(string recognizerName, UInt32 userID)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(recognizerName);
            FubiUtils.RecognitionResult ret = FubiInternal.recognizeGestureOn(namePtr, userID);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static bool isInitialized()
        {
          return FubiInternal.isInitialized();
        }

        public static UInt32 getNumUserDefinedRecognizers()
        {
            return FubiInternal.getNumUserDefinedRecognizers();
        }

        public static void getDepthResolution(out Int32 width, out Int32 height)
        {
            FubiInternal.getDepthResolution(out width, out height);
        }

        public static void getRgbResolution(out Int32 width, out Int32 height)
        {
            FubiInternal.getRgbResolution(out width, out height);
        }

        public static string getUserDefinedRecognizerName(UInt32 recognizerIndex)
        {
            IntPtr namePtr = FubiInternal.getUserDefinedRecognizerName(recognizerIndex);
            return Marshal.PtrToStringAnsi(namePtr);
        }

        public static UInt32 getUserDefinedRecognizerIndex(string recognizerName)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(recognizerName);
            UInt32 ret = FubiInternal.getUserDefinedRecognizerIndex(namePtr);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static Int32 getFingerCount(UInt32 userID, bool leftHand)
        {
            return FubiInternal.getFingerCount(userID, leftHand);
        }

        public static FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(string recognizerName, UInt32 userID, bool restart = true)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(recognizerName);
            FubiUtils.RecognitionResult ret = FubiInternal.getCombinationRecognitionProgressOn(namePtr, userID, new IntPtr(0), restart);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static FubiUtils.RecognitionResult getCombinationRecognitionProgressOn(string recognizerName, UInt32 userID, out FubiTrackingData[] userStates, bool restart = true)
        {
            IntPtr vec = FubiInternal.createTrackingDataVector();
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(recognizerName);
            FubiUtils.RecognitionResult recognized = FubiInternal.getCombinationRecognitionProgressOn(namePtr, userID, vec, restart);
            Marshal.FreeHGlobal(namePtr);
            if (recognized == FubiUtils.RecognitionResult.RECOGNIZED)
            {
                UInt32 size = FubiInternal.getTrackingDataVectorSize(vec);
                userStates = new FubiTrackingData[size];
                for (UInt32 i = 0; i < size; i++)
                {
                    IntPtr tInfo = FubiInternal.getTrackingData(vec, i);
                    FubiTrackingData info = new FubiTrackingData();
                    for (UInt32 j = 0; j < (uint)FubiUtils.SkeletonJoint.NUM_JOINTS; ++j)
                    {
                        FubiInternal.getSkeletonJointPosition(tInfo, (FubiUtils.SkeletonJoint)j, out info.jointPositions[j].x, out info.jointPositions[j].y, out info.jointPositions[j].z, out info.jointPositions[j].confidence, out info.timeStamp);
                        double timeStamp = 0;
                        FubiInternal.getSkeletonJointOrientation(tInfo, (FubiUtils.SkeletonJoint)j, info.jointOrientations[j].rotMat, out info.jointPositions[j].confidence, out timeStamp);
                        info.timeStamp = Math.Max(timeStamp, info.timeStamp);
                    }
                    userStates[i] = info;
                }
            }
            else
                userStates = null;

            FubiInternal.releaseTrackingDataVector(vec);
            return recognized;
        }

        public static Int32 getUserDefinedCombinationRecognizerIndex(string recognizerName)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(recognizerName);
            Int32 ret = FubiInternal.getUserDefinedCombinationRecognizerIndex(namePtr);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 getNumUserDefinedCombinationRecognizers()
        {
            return FubiInternal.getNumUserDefinedCombinationRecognizers();
        }

        public static string getUserDefinedCombinationRecognizerName(UInt32 recognizerIndex)
        {
            return Marshal.PtrToStringAnsi(FubiInternal.getUserDefinedCombinationRecognizerName(recognizerIndex));
        }

        public static void enableCombinationRecognition(string combinationName, UInt32 userID, bool enable)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(combinationName);
            FubiInternal.enableCombinationRecognition(namePtr, userID, enable);
            Marshal.FreeHGlobal(namePtr);
        }

        public static void getColorForUserID(UInt32 id, out float r, out float g, out float b)
        {
            FubiInternal.getColorForUserID(id, out r, out g, out b);
        }


        public static bool saveImage(string fileName, int jpegQuality /*0-100*/,
            FubiUtils.ImageType type, FubiUtils.ImageNumChannels numChannels, FubiUtils.ImageDepth depth,
            uint renderOptions = (uint)FubiUtils.RenderOptions.Default,
            FubiUtils.DepthImageModification depthModifications = FubiUtils.DepthImageModification.UseHistogram,
            UInt32 userId = 0, FubiUtils.SkeletonJoint jointOfInterest = FubiUtils.SkeletonJoint.NUM_JOINTS)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(fileName);
            bool ret = FubiInternal.saveImage(namePtr, jpegQuality, type, numChannels, depth, renderOptions, depthModifications, userId, jointOfInterest);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 addJointRelationRecognizer(FubiUtils.SkeletonJoint joint, FubiUtils.SkeletonJoint relJoint,
            float minX = -FubiUtils.Math.MaxFloat, float minY = -FubiUtils.Math.MaxFloat, float minZ = -FubiUtils.Math.MaxFloat,
            float maxX = FubiUtils.Math.MaxFloat, float maxY = FubiUtils.Math.MaxFloat, float maxZ = FubiUtils.Math.MaxFloat,
            float minDistance = 0, float maxDistance = FubiUtils.Math.MaxFloat,
            bool useLocalPositions = false,
            Int32 atIndex = -1, string name = null,
            float minConfidene = -1.0f, FubiUtils.BodyMeasurement measuringUnit = FubiUtils.BodyMeasurement.NUM_MEASUREMENTS)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(name);
            UInt32 ret = FubiInternal.addJointRelationRecognizer(joint, relJoint, minX, minY, minZ, maxX, maxY, maxZ, minDistance, maxDistance, useLocalPositions, atIndex, namePtr, minConfidene, measuringUnit);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 addFingerCountRecognizer(FubiUtils.SkeletonJoint handJoint,
            UInt32 minFingers, UInt32 maxFingers,
            Int32 atIndex = -1,
            string name = null,
            float minConfidence = -1.0f,
            bool useMedianCalculation = false)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(name);
            UInt32 ret = FubiInternal.addFingerCountRecognizer(handJoint, minFingers, maxFingers, atIndex, namePtr, minConfidence, useMedianCalculation);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 addJointOrientationRecognizer(FubiUtils.SkeletonJoint joint,
            float minX = -180.0f, float minY = -180.0f, float minZ = -180.0f,
            float maxX = 180.0f, float maxY = 180.0f, float maxZ = 180.0f,
            bool useLocalOrientations = true,
            int atIndex = -1,
            string name = null,
            float minConfidence = -1)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(name);
            UInt32 ret = FubiInternal.addJointOrientationRecognizer(joint, minX, minY, minZ, maxX, maxY, maxZ, useLocalOrientations, atIndex, namePtr, minConfidence);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 addLinearMovementRecognizer(FubiUtils.SkeletonJoint joint,
            float dirX, float dirY, float dirZ, float minVel = 0, float maxVel = FubiUtils.Math.MaxFloat,
            bool useLocalPositions = false,
            int atIndex = -1, string name = null, float maxAngleDifference = 45.0f, bool useOnlyCorrectDirectionComponent = true)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(name);
            UInt32 ret = FubiInternal.addLinearMovementRecognizer(joint, dirX, dirY, dirZ, minVel, maxVel, useLocalPositions, atIndex, namePtr, maxAngleDifference, useOnlyCorrectDirectionComponent);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static UInt32 addLinearMovementRecognizer(FubiUtils.SkeletonJoint joint, FubiUtils.SkeletonJoint relJoint,
            float dirX, float dirY, float dirZ, float minVel = 0, float maxVel = FubiUtils.Math.MaxFloat,
            bool useLocalPositions = false,
            int atIndex = -1, string name = null, float maxAngleDifference = 45.0f, bool useOnlyCorrectDirectionComponent = true)
        {
            IntPtr namePtr = Marshal.StringToHGlobalAnsi(name);
            UInt32 ret = FubiInternal.addLinearMovementRecognizer(joint, relJoint, dirX, dirY, dirZ, minVel, maxVel, useLocalPositions, atIndex, namePtr, maxAngleDifference, useOnlyCorrectDirectionComponent);
            Marshal.FreeHGlobal(namePtr);
            return ret;
        }

        public static bool addCombinationRecognizer(string xmlDefinition)
        {
            IntPtr stringPtr = Marshal.StringToHGlobalAnsi(xmlDefinition);
            bool ret = FubiInternal.addCombinationRecognizer(stringPtr);
            Marshal.FreeHGlobal(stringPtr);
            return ret;
        }

        public static bool isUserInScene(UInt32 userID)
        {
            return FubiInternal.isUserInScene(userID);
        }

        public static bool isUserTracked(UInt32 userID)
        {
            return FubiInternal.isUserTracked(userID);
        }


        public static void getCurrentSkeletonJointPosition(UInt32 userID, FubiUtils.SkeletonJoint joint, out float x, out float y, out float z, out float confidence, out double timeStamp,bool useLocalPositions = false)
        {
            IntPtr info = FubiInternal.getCurrentTrackingData(userID);
             FubiInternal.getSkeletonJointPosition(info, joint, out x, out y, out z, out confidence, out timeStamp, useLocalPositions);
        }

        public static void getCurrentSkeletonJointOrientation(UInt32 userID, FubiUtils.SkeletonJoint joint, float[] mat, out float confidence, out double timeStamp)
        {
            IntPtr info = FubiInternal.getCurrentTrackingData(userID);
            if (info.ToInt32() != 0)
            {
                FubiInternal.getSkeletonJointOrientation(info, joint, mat, out confidence, out timeStamp);
            }
            else
            {
                mat = new float[] {1,0,0,0,1,0,0,0,1};
                confidence = 0;
                timeStamp = 0;
            }
        }

        public static void clearUserDefinedRecognizers()
        {
            FubiInternal.clearUserDefinedRecognizers();
        }

        public static UInt32 getClosestUserID()
        {
            return FubiInternal.getClosestUserID();
        }

        public static UInt16 getNumUsers()
        {
            return FubiInternal.getCurrentUsers(new IntPtr(0));
        }

        public static void realWorldToProjective(float realWorldX, float realWorldY, float realWorldZ, out float screenX, out float screenY, out float screenZ)
        {
            FubiInternal.realWorldToProjective(realWorldX, realWorldY, realWorldZ, out screenX, out screenY, out screenZ);
        }

        public static void resetTracking()
        {
            FubiInternal.resetTracking();
        }

        public static double getCurrentTime()
        {
            return FubiInternal.getCurrentTime();
        }
    }

}

