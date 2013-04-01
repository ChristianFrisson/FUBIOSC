using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FubiNET
{
    public class FubiUtils
    {
        // Options for image rendering
        public enum ImageType
        {
            /*	The possible image types
            */
            Color = 0,
            Depth,
            IR
        };
        public enum ImageNumChannels
        {
            /*	The number of channels in the image
            */
            C1 = 1,
            C3 = 3,
            C4 = 4
        };
        public enum ImageDepth
        {
            /*	The depth of each channel
            */
            D8 = 8,
            D16 = 16
        };
        public enum DepthImageModification
        {
            /*	How the depth image should be modified for depth differences
                being easier to distinguish by the human eye
            */
            Raw = 0,
            UseHistogram,
            StretchValueRange,
            ConvertToRGB
        };
        public enum RenderOptions
        {
            /*	The possible formats for the tracking info rendering
            */
            None = 0,
            Shapes =                0x000001,
            Skeletons =             0x000002,
            UserCaptions =          0x000004,
            LocalOrientCaptions =   0x000008,
            GlobalOrientCaptions =  0x000010,
            LocalPosCaptions =      0x000020,
            GlobalPosCaptions =     0x000040,
            Background =            0x000080,
            SwapRAndB =             0x000100,
            FingerShapes =          0x000200,
            DetailedFaceShapes =    0x000400,
            BodyMeasurements =      0x000800,
            Default = FubiUtils.RenderOptions.Shapes | FubiUtils.RenderOptions.Skeletons | FubiUtils.RenderOptions.UserCaptions
        };

	    // Maximum depth value that can occure in the depth image
	    public const int MaxDepth = 10000;
	    // Maximum number of tracked users
	    public const int MaxUsers = 15;

        public enum RecognitionResult
        {
            /*	Result of a gesture recognition
            */
            TRACKING_ERROR = -1,
            NOT_RECOGNIZED = 0,
            RECOGNIZED = 1,
            WAITING_FOR_LAST_STATE_TO_FINISH = 2	// Only for combinations with waitUntilLastStateRecognizersStop flag
        };

        public enum SkeletonJoint
        {
            HEAD			= 0,
			NECK			= 1,
			TORSO			= 2,
			WAIST			= 3,

			LEFT_SHOULDER	= 4,
			LEFT_ELBOW		= 5,
			LEFT_WRIST		= 6,
			LEFT_HAND		= 7,

			RIGHT_SHOULDER	=8,
			RIGHT_ELBOW		=9,
			RIGHT_WRIST		=10,
			RIGHT_HAND		=11,

			LEFT_HIP		=12,
			LEFT_KNEE		=13,
			LEFT_ANKLE		=14,
			LEFT_FOOT		=15,

			RIGHT_HIP		=16,
			RIGHT_KNEE		=17,
			RIGHT_ANKLE		=18,
			RIGHT_FOOT		=19,

            FACE_NOSE       =20,
            FACE_LEFT_EAR   =21,
            FACE_RIGHT_EAR  =22,
            FACE_FOREHEAD   =23,
            FACE_CHIN       =24,

			NUM_JOINTS		=25
        };

        public enum BodyMeasurement
        {
            BODY_HEIGHT = 0,
            TORSO_HEIGHT = 1,
            SHOULDER_WIDTH = 2,
            HIP_WIDTH = 3,
            ARM_LENGTH = 4,
            UPPER_ARM_LENGTH = 5,
            LOWER_ARM_LENGTH = 6,
            LEG_LENGTH = 7,
            UPPER_LEG_LENGTH = 8,
            LOWER_LEG_LENGTH = 9,
            NUM_MEASUREMENTS = 10
        };

        public enum SkeletonProfile
        {
            NONE = 1,

            ALL = 2,

            UPPER_BODY = 3,

            LOWER_BODY = 4,

            HEAD_HANDS = 5,
        };

        public enum SensorType
        {
            /** No sensor in use **/
            NONE = 0,
            /** Sensor based on OpenNI 2.x**/
            OPENNI2 = 0x01,
            /** Sensor based on OpenNI 1.x**/
            OPENNI1 = 0x02,
            /** Sensor based on the Kinect for Windows SDK 1.x**/
            KINECTSDK = 0x04
        };

        public class StreamOptions
	    {
		    public StreamOptions(int width = 640, int height = 480, int fps = 30)
			{
                m_width =width;
                m_height =height;
                m_fps = fps;
		    }
            public void invalidate()
		    {
                m_width = -1; m_height = -1; m_fps = -1;
            }
            public bool isValid()
		    {
                return m_width > 0 && m_height > 0 && m_fps > 0;
            }

            public int m_width;
            public int m_height;
            public int m_fps;
	    };

	    public class SensorOptions
	    {
		    public SensorOptions(StreamOptions depthOptions,
                StreamOptions rgbOptions, StreamOptions irOptions,
                SensorType sensorType = SensorType.OPENNI2,
			    FubiUtils.SkeletonProfile profile = FubiUtils.SkeletonProfile.ALL,
			    bool mirrorStreams = true, float smoothing = 0)
			{
                m_depthOptions = depthOptions;
                m_irOptions = irOptions;
                m_rgbOptions = rgbOptions;
			    m_profile = profile;
                m_mirrorStreams = mirrorStreams;
                m_smoothing = smoothing;
                m_type = sensorType;
		    }
		    public StreamOptions m_depthOptions;
            public StreamOptions m_irOptions;
            public StreamOptions m_rgbOptions;

            public FubiUtils.SkeletonProfile m_profile;
            public float m_smoothing;
            public bool m_mirrorStreams;
            public SensorType m_type;
	    };


        // Constants
        public class Math
        {
	        public const UInt32 MaxUInt32 = 0xFFFFFFFF;
            public const int MinInt32 = 0x8000000;
            public const int MaxInt32 = 0x7FFFFFFF;
	        public const float MaxFloat = 3.402823466e+38F;
            public const float MinPosFloat = 1.175494351e-38F;

            public const float Pi = 3.141592654f;
            public const float TwoPi = 6.283185307f;
            public const float PiHalf = 1.570796327f;

            public const float Epsilon = 0.000001f;
            public const float ZeroEpsilon = 32.0f * MinPosFloat;  // Very small epsilon for checking against 0.0f

            public const float NaN = 0xFFFFFFFF;

            public static float degToRad( float f ) 
            {
	            return f * 0.017453293f;
            }

            public static float radToDeg(float f) 
            {
	            return f * 57.29577951f;
            }

            public static bool rotMatToRotation(float[] mat, out float rx, out float ry, out float rz)
            {
                if (mat.Length == 9)
                {
                    rx = radToDeg((float)System.Math.Sin(-mat[7]));

                    // Special case: Cos[x] == 0 (when Sin[x] is +/-1)
                    float f = (float)System.Math.Abs(mat[7]);
                    if (f > 0.999f && f < 1.001f)
                    {
                        // Pin arbitrarily one of y or z to zero
                        // Mathematical equivalent of gimbal lock
                        ry = 0;

                        // Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
                        // => m[0][0] = Cos[z] and m[1][0] = Sin[z]
                        rz = radToDeg((float)System.Math.Atan2(-mat[3], mat[0]));
                    }
                    // Standard case
                    else
                    {
                        ry = radToDeg((float)System.Math.Atan2(mat[6], mat[8]));
                        rz = radToDeg((float)System.Math.Atan2(mat[1], mat[4]));
                    }
                    return true;
                }

                rx = ry = rz = 0;
                return false;
            }
        };


    }
}
