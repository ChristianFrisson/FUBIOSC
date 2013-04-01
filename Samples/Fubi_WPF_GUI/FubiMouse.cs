using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using FubiNET;

namespace Fubi_WPF_GUI
{
    class FubiMouse
    {
        [DllImport("user32.dll", SetLastError = true)]
        static extern Int32 GetSystemMetrics(Int32 metric);

        public delegate int HookProc(int nCode, IntPtr wParam, IntPtr lParam);

        //Declare the mouse hook constant.
        //For other hook types, you can obtain these values from Winuser.h in the Microsoft SDK.
        const int WH_MOUSE = 7;


        //Declare the wrapper managed POINT class.
        [StructLayout(LayoutKind.Sequential)]
        class POINT
        {
            public int x;
            public int y;
        }

        //Declare the wrapper managed MouseHookStruct class.
        [StructLayout(LayoutKind.Sequential)]
        class MouseHookStruct
        {
            public POINT pt;
            public int hwnd;
            public int wHitTestCode;
            public int dwExtraInfo;
        }

        //This is the Import for the SetWindowsHookEx function.
        //Use this function to install a thread-specific hook.
        [DllImport("user32.dll", CharSet = CharSet.Auto,
         CallingConvention = CallingConvention.StdCall)]
        static extern int SetWindowsHookEx(int idHook, HookProc lpfn,
        IntPtr hInstance, int threadId);

        //This is the Import for the UnhookWindowsHookEx function.
        //Call this function to uninstall the hook.
        [DllImport("user32.dll", CharSet = CharSet.Auto,
         CallingConvention = CallingConvention.StdCall)]
        static extern bool UnhookWindowsHookEx(int idHook);

        //This is the Import for the CallNextHookEx function.
        //Use this function to pass the hook information to the next hook procedure in chain.
        [DllImport("user32.dll", CharSet = CharSet.Auto,
         CallingConvention = CallingConvention.StdCall)]
        static extern int CallNextHookEx(int idHook, int nCode, IntPtr wParam, IntPtr lParam);

        public FubiMouse()
        {
            m_timeStamp = 0;
            m_lastMouseClick = 0;

            // Start mouse pos in the middle
            m_x = 0.5f;
            m_y = 0.5f;

            // Default mapping values
            m_mapX = -100.0f;
            m_mapY = 250.0f;
            m_mapH = 550.0f;

            // Get screen aspect
            Int32 screenWidth = GetSystemMetrics(0) - 1;
            Int32 screenHeight = GetSystemMetrics(1) - 1;
            m_aspect = (float)screenWidth / (float)screenHeight;

            // Calculated Map width with aspect
            m_mapW = m_mapH / m_aspect;

            m_smoothingFactor = 0.5f;
        }

        public void applyHandPositionToMouse(uint userID, bool leftHand = false)
        {
            float x, y;
            applyHandPositionToMouse(userID, out x, out y, leftHand);
        }

        public void applyHandPositionToMouse(uint userID, out float x, out float y, bool leftHand = false)
        {
            x = float.NaN;
            y = float.NaN;

            float handX, handY, handZ, confidence;

            FubiUtils.SkeletonJoint joint = FubiUtils.SkeletonJoint.RIGHT_HAND;
            FubiUtils.SkeletonJoint relJoint = FubiUtils.SkeletonJoint.RIGHT_SHOULDER;
            if (leftHand)
            {
                joint = FubiUtils.SkeletonJoint.LEFT_HAND;
                relJoint = FubiUtils.SkeletonJoint.LEFT_SHOULDER;
            }

            double timeStamp;
            Fubi.getCurrentSkeletonJointPosition(userID, joint, out handX, out handY, out handZ, out confidence, out timeStamp);

            if (confidence > 0.5f)
            {
                float relX, relY, relZ;
                Fubi.getCurrentSkeletonJointPosition(userID, relJoint, out relX, out relY, out relZ, out confidence, out timeStamp);

                if (confidence > 0.5f)
                {
                    // Take relative coordinates
                    float rawX = handX - relX;
                    float rawY = handY - relY;

                    // Convert to screen coordinates
                    float newX, newY;
                    float mapX = m_mapX;
                    if (leftHand)
                        // Mirror x  area for left hand
                        mapX = -m_mapX - m_mapW;
                    newX = (rawX - mapX) / m_mapW;
                    newY = (m_mapY - rawY) / m_mapH; // Flip y for the screen coordinates

                    // Filtering
                    // New coordinate is weighted more if it represents a longer distance change
                    // This should reduce the lagging of the cursor on higher distances, but still filter out small jittering
                    float changeX = newX - m_x;
                    float changeY = newY - m_y;

                    if (changeX != 0 || changeY != 0 && timeStamp != m_timeStamp)
                    {
                        double changeLength = Math.Sqrt(changeX * changeX + changeY * changeY);
                        double changeDur = timeStamp - m_timeStamp;
                        float filterFactor = (float)Math.Sqrt(changeLength) * m_smoothingFactor;
                        if (filterFactor > 1.0f)
                            filterFactor = 1.0f;

                        // Apply the tracking to the current position with the given filter factor
                        m_x = (1.0f - filterFactor) * m_x + filterFactor * newX;
                        m_y = (1.0f - filterFactor) * m_y + filterFactor * newY;
                        m_timeStamp = timeStamp;

                        // Send it
                        MouseSimulator.sendMousePos(m_x, m_y);
                        x = m_x;
                        y = m_x;
                    }

                    // Check for mouse click
                    if (Fubi.recognizeGestureOn("mouseClick", userID) == FubiUtils.RecognitionResult.RECOGNIZED
                        || Fubi.recognizeGestureOn("mouseClick1", userID) == FubiUtils.RecognitionResult.RECOGNIZED)
                    {
                        if (m_timeStamp - m_lastMouseClick > 1)
                        {
                            MouseSimulator.sendMouseButton(true, true);
                            MouseSimulator.sendMouseButton(true, false);
                            m_lastMouseClick = m_timeStamp;
                        }
                    }

                }
            }
        }

        public void autoCalibrateMapping(bool leftHand)
        {
            uint id = Fubi.getClosestUserID();
            if (id > 0)
            {
                FubiUtils.SkeletonJoint elbow = FubiUtils.SkeletonJoint.RIGHT_ELBOW;
                FubiUtils.SkeletonJoint shoulder = FubiUtils.SkeletonJoint.RIGHT_SHOULDER;
                FubiUtils.SkeletonJoint hand = FubiUtils.SkeletonJoint.RIGHT_HAND;

                if (leftHand)
                {
                    elbow = FubiUtils.SkeletonJoint.LEFT_ELBOW;
                    shoulder = FubiUtils.SkeletonJoint.LEFT_SHOULDER;
                    hand = FubiUtils.SkeletonJoint.LEFT_HAND;
                }

                float confidence;
                double timeStamp;
                float elbowX, elbowY, elbowZ;
                Fubi.getCurrentSkeletonJointPosition(id, elbow, out elbowX, out elbowY, out elbowZ, out confidence, out timeStamp);
                if (confidence > 0.5f)
                {
                    float shoulderX, shoulderY, shoulderZ;
                    Fubi.getCurrentSkeletonJointPosition(id, shoulder, out shoulderX, out shoulderY, out shoulderZ, out confidence, out timeStamp);
                    if (confidence > 0.5f)
                    {
                        double dist1 = Math.Sqrt(Math.Pow(elbowX - shoulderX, 2) + Math.Pow(elbowY - shoulderY, 2) + Math.Pow(elbowZ - shoulderZ, 2));
                        float handX, handY, handZ;
                        Fubi.getCurrentSkeletonJointPosition(id, hand, out handX, out handY, out handZ, out confidence, out timeStamp);
                        if (confidence > 0.5f)
                        {
                            double dist2 = Math.Sqrt(Math.Pow(elbowX - handX, 2) + Math.Pow(elbowY - handY, 2) + Math.Pow(elbowZ - handZ, 2));
                            MapH = (float)(dist1 + dist2);
                            // Calculate all others in depence of maph
                            MapY = 250.0f / 550.0f * MapH;
                            MapW = MapH / m_aspect;
                            MapX = -100.0f / (550.0f / m_aspect) * MapW;
                        }
                    }
                }
            }
        }

        public float SmoothingFactor
        {
            get { return m_smoothingFactor; }
            set
            {
                m_smoothingFactor = value;
                if (m_smoothingFactor > 1.0f)
                    m_smoothingFactor = 1.0f;
                if (m_smoothingFactor < 0.00001f)
                    m_smoothingFactor = 0.00001f;
            }
        }
        public float MapH
        {
            get { return m_mapH; }
            set { m_mapH = value; }
        }

        public float MapW
        {
            get { return m_mapW; }
            set { m_mapW = value; }
        }

        public float MapY
        {
            get { return m_mapY; }
            set { m_mapY = value; }
        }

        public float MapX
        {
            get { return m_mapX; }
            set { m_mapX = value; }
        }

        float m_mapX, m_mapY, m_mapW, m_mapH;
        float m_x, m_y;
        float m_aspect;
        float m_smoothingFactor;
        double m_timeStamp;
        double m_lastMouseClick;
    }
}
