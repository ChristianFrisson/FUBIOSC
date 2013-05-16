/*
   QtKinectWrapper - Qt Kinect Wrapper Class
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
   Requirements:

      The QKinectWrapper class requires the following to be installed (versions are those tested and known to work):
         Windows:
            - OpenNI-Win32-v1.0.0.25.exe
            - Sensor-Kinect-Win32-5.0.0.exe
            - NITE-Win32-v1.3.0.18.exe
         Other: not checked

   Basics:
      QKinectWrapper provides Qt-friendly wrapper to get body skeleton, as well as camera image and depth map from a Kinect.
      It provides around the logic to 1) initialize the kinect, 2) continuously read the Kinect sensor, including depth map, image frame, and skeleton, 3) perform the necessary calibration, 4) shutdown.
      In principle up to 15 bodies are supported (limited by the Kinect driver)
      QKinectWrapper creates a dedicated thread to read the data of the Kinect. The application is shielded from any thread nastiness:
         - Safe functions are available to get the data from the kinect (taking care of mutexing as required). This includes functions to read image, depth map, and get the body skeleton.
         - Signals are used to indicate to the application a change of state in QKinectWrapper (e.g. initialize, running, error, stop), and the availability of new data (i.e. a new frame/depth/skeleton from the camera)

   Howto:
      1) Instanciate QKinectWrapper
      2) Connect the 'dataNotification' and 'statusNotification' signals to your application
      3) Optionally, set some visualization parameters (e.g. setPen)
      4) Call 'start'
      5) In the data notification slot, get the data from the kinect with methods getImage, getDepth, getJoints
      6) When done, call stop
      Use the statusNotification to get notified of initialization error or change of status of the kinect reader thread
*/

#ifndef __QKINECTWRAPPER_H
#define __QKINECTWRAPPER_H

#include <QWidget>
#include <QPainter>
#include <QThread>
#include <QMutex>
#include <XnOpenNI.h>
#include <XnCppWrapper.h>

namespace QKinect
{
   /**
     \brief Status of the body tracking
   **/
   enum CalibrationStatus
   {
      CalibrationStart = 0,
      CalibrationEndSuccess = 1,
      CalibrationEndFail = 2
   };

   /**
     \brief Status of the body tracking
   **/
   enum KinectStatus
   {
      Idle = 0,
      Initializing = 1,
      OkRun = 2,
      ErrorStop = 3
   };
   /**
     \brief Status of the body tracking
   **/
   enum BodyStatus
   {
      Tracking = 0,
      Calibrating = 1,
      LookingForPose = 2
   };
   /**
     \brief Enumeration of body joints
   **/
   enum BodyJoints
   {
      Head = 0,
      Neck = 1,
      LeftShoulder = 2,
      LeftElbow = 3,
      LeftHand = 4,
      RightShoulder = 5,
      RightElbow = 6,
      RightHand = 7,
      Torso = 8,
      LeftHip = 9,
      LeftKnee = 10,
      LeftFoot = 11,
      RightHip = 12,
      RightKnee = 13,
      RightFoot = 14,
   };
   /**
     \brief Body structure containing the user id, status, skeleton 3D coords, and skeleton projection
   **/
   typedef struct {
      XnUserID id;
      // Status: 0=tracking, 1=calibrating, 2=looking for pose
      BodyStatus status;
      // Center of mass and its projection.
      XnPoint3D com, proj_com;
      // Whether the com projection is valid
      bool proj_com_valid;
      // Whether the body tracked
      //bool tracked;
      // 3D coordinates of the joints
      XnSkeletonJointPosition joints[15];
      // Projected coordinates of the joints
      XnPoint3D proj_joints[15];
      // Whether the projection is valid / has been computed. Projections are not computed if the joint is too uncertain.
      bool proj_joints_valid[15];
   } Body;

   /**
     \brief Bodies: vector of Body - to hold the data of multiple tracked persons
   **/
   typedef std::vector<Body> Bodies;


   class QKinectWrapper : public QThread
   {
      Q_OBJECT
   public:
      QKinectWrapper();
      virtual ~QKinectWrapper();
      void run();

      void start();
      void stop();


      bool isRunning();
      bool isStopped();

      void setSkeletonPen(const QPen & pen);
      void setTextPen(const QPen & pen);
      void setFont(const QFont &font);
      void setDisplayInfoDepth(bool draw);
      void setDisplayInfoImage(bool draw);
      void setDisplaySkeletonDepth(bool draw);
      void setDisplaySkeletonImage(bool draw);
      unsigned getFrameID();
      double getTimestamp();
      void getCameraDepthBodies(QImage &camera,QImage &depth,Bodies &bodies,double &ts,unsigned &fid);
      void getCameraDepth(QImage &camera,QImage &depth,double &ts,unsigned &fid);
      QImage getDepth();
      QImage getCamera();
      Bodies getBodies();
      QString getErrorMsg();

   private:
      // <-- Data to export outside of the thread
      unsigned frameid;
      Bodies bodies;
      QImage imageDepth,imageCamera;
      double timestamp;
      QString errorMsg;
      // --> End data to export outside of the thread

      bool t_requeststop;
      QMutex mutex;
      volatile KinectStatus status;                         // 0: pre-init. 1: init. 2: ok-run. 3:error-stop
      xn::Context g_Context;
      xn::DepthGenerator g_DepthGenerator;
      xn::UserGenerator g_UserGenerator;
      xn::ImageGenerator g_ImageGenerator;
      //xn::HandsGenerator g_HandsGenerator;
      //xn::HandsGenerator g_GestureGenerator;

      XnChar g_strPose[20];



      QPen skeletonPen;                            // Pen used to draw the kinect body
      QPen textPen;                                // Pen used to draw text
      QFont font;
      bool displayInfoDepth;
      bool displayInfoImage;
      bool displaySkeletonDepth;
      bool displaySkeletonImage;
      XnBool g_bNeedPose;


      QImage createDepthImage();
      QImage createCameraImage();
      Bodies createBodies();
      //bool initializexml();
      bool initialize();

      void getJointProj(XnSkeletonJointPosition joint,XnPoint3D &proj,bool &valid);

      void drawInfo(QPainter *painter);
      void drawLimb(QPainter *painter,const Body &body, BodyJoints j1,BodyJoints j2);
      void drawSkeleton(QPainter *painter);

      static void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
      static void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
      static void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie);
      static void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie);
      static void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus calibrationError, void* pCookie);

      //static void XN_CALLBACK_TYPE Hand_Create(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie);
      //static void XN_CALLBACK_TYPE Hand_Destroy(xn::HandsGenerator& generator, XnUserID nId, XnFloat time,void* pCookie);
      //static void XN_CALLBACK_TYPE Hand_Update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie);

   private slots:

   signals:
      void dataNotification();
      void statusNotification(QKinect::KinectStatus);
      void userNotification(unsigned,bool);
      void poseNotification(unsigned,QString);
      void calibrationNotification(unsigned,QKinect::CalibrationStatus);


   };

}  // End namespace

// Register types used in signal/slot mechanism
Q_DECLARE_METATYPE(QKinect::KinectStatus);
Q_DECLARE_METATYPE(QKinect::CalibrationStatus);


#endif // __QKINECTWRAPPER_H
