using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FubiNET
{
    public class FubiTrackingData
    {
        public FubiTrackingData()
        {
            uint numJoints = (uint)FubiUtils.SkeletonJoint.NUM_JOINTS;
            jointOrientations = new JointOrientation[numJoints];
            for (uint i = 0; i < numJoints; ++i)
                jointOrientations[i] = new JointOrientation();
            jointPositions = new JointPosition[numJoints];
            for (uint i = 0; i < numJoints; ++i)
                jointPositions[i] = new JointPosition();
        }
        public class JointPosition
        {
            public float x, y, z;
            public float confidence;
        };
        public class JointOrientation
        {
            public JointOrientation()
            {
                rotMat = new float[9];
            }
            public float[] rotMat;
            public float confidence;
        };

        public JointOrientation[] jointOrientations;
        public JointPosition[] jointPositions;

        public double timeStamp;
    }
}
