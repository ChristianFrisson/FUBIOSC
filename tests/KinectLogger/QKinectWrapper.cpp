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


#include "QKinectWrapper.h"
#include <math.h>
//#include <cmath>
//#include "precisetimer.h"

namespace QKinect
{

int BodyColors[][3] =
{
   {0,127,127},
   {0,0,127},
   {0,127,0},
   {127,127,0},
   {127,0,0},
   {127,63,0},
   {63,127,0},
   {0,63,127},
   {63,0,127},
   {127,127,63},
   {127,127,127}
};


/******************************************************************************
*******************************************************************************
* QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/


/**
   \brief Constructor - does nothing with the kinect driver but inits some internal vars
**/
QKinectWrapper::QKinectWrapper()
{
   // Register types used in signal/slot mechanism
   qRegisterMetaType<QKinect::KinectStatus>( "QKinect::KinectStatus" );
   qRegisterMetaType<QKinect::CalibrationStatus>( "QKinect::CalibrationStatus" );

   status=QKinect::Idle;
   g_bNeedPose=FALSE;
   strcpy(g_strPose,"");
   displayInfoDepth = true;
   displayInfoImage = true;
   displaySkeletonDepth = true;
   displaySkeletonImage = true;

   setSkeletonPen(QPen(QBrush(Qt::white),5));
   setTextPen(QPen(QBrush(Qt::white),5));
   QFont font;
   font.setPointSize(16);
   setFont(font);
}

/**
   \brief Destructor - calls stop - which terminates the kinect-dedicated reading thread - blocks until terminated.
**/
QKinectWrapper::~QKinectWrapper()
{
   stop();
}
void QKinectWrapper::start()
{
   t_requeststop=false;
   status=QKinect::Idle;
   QThread::start();
}

/**
   \brief Request the the kinect-dedicated reading thread to terminate, and thus
   release all kinect handles.
   Sets requeststop, which makes the main reading loop terminate (thread ended) at
   some later time. Blocks until terminated.
**/
void QKinectWrapper::stop()
{
   t_requeststop=true;

   wait();
}


/**
   \brief Sets the pen used to draw the skeleton
**/
void QKinectWrapper::setSkeletonPen(const QPen & pen)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::skeletonPen = pen;
}

/**
   \brief Sets the pen used to draw text
**/
void QKinectWrapper::setTextPen(const QPen & pen)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::textPen = pen;
}

/**
   \brief Sets the font used to draw the skeleton
**/
void QKinectWrapper::setFont(const QFont &font)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::font = font;
}

/**
   \brief Display info about the state of the objects (tracked, ...) on the depth map
**/
void QKinectWrapper::setDisplayInfoDepth(bool draw)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::displayInfoDepth = draw;
}

/**
   \brief Display info about the state of the objects (tracked, ...) on the camera image
**/
void  QKinectWrapper::setDisplayInfoImage(bool draw)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::displayInfoImage = draw;
}

/**
   \brief Display the skeletonon the depth image
**/
void QKinectWrapper::setDisplaySkeletonDepth(bool draw)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::displaySkeletonDepth = draw;
}

/**
   \brief Display the skeletonon the camera image
**/
void QKinectWrapper::setDisplaySkeletonImage(bool draw)
{
   QMutexLocker locker(&mutex);
   QKinectWrapper::displaySkeletonImage = draw;
}


unsigned QKinectWrapper::getFrameID()
{
   QMutexLocker locker(&mutex);
   return frameid;
}
double QKinectWrapper::getTimestamp()
{
    QMutexLocker locker(&mutex);
    return timestamp;
}

/**
   \brief Return the latest depth image
   Call this afer a dataNotification signal
**/
QImage QKinectWrapper::getDepth()
{
   QMutexLocker locker(&mutex);
   return imageDepth;
}

/**
   \brief Return the latest camera image
   Call this afer a dataNotification signal
**/
QImage QKinectWrapper::getCamera()
{
   QMutexLocker locker(&mutex);
   return imageCamera;
}

/**
   \brief Return the latest bodies
   Call this afer a dataNotification signal
**/
Bodies QKinectWrapper::getBodies()
{
   QMutexLocker locker(&mutex);
   return bodies;
}

/**
   \brief Return the latest camera and depth images and bodies, together with their timestamp.
   Call this afer a dataNotification signal
**/
void QKinectWrapper::getCameraDepthBodies(QImage &camera,QImage &depth,Bodies &bodies, double &ts, unsigned &fid)
{
   QMutexLocker locker(&mutex);
   camera = imageCamera;
   depth = imageDepth;
   bodies = QKinectWrapper::bodies;
   ts = timestamp;
   fid = frameid;
}
/**
   \brief Return the latest camera and depth images, together with their timestamp.
   Call this afer a dataNotification signal
**/
void QKinectWrapper::getCameraDepth(QImage &camera,QImage &depth, double &ts, unsigned &fid)
{
   QMutexLocker locker(&mutex);
   camera = imageCamera;
   depth = imageDepth;
   ts = timestamp;
   fid = frameid;
}

/**
   \brief Return the last error message.
   This should only be called if statusNotification is called and indicates an error
**/
QString QKinectWrapper::getErrorMsg()
{
   QMutexLocker locker(&mutex);
   return errorMsg;

}

/**
   \brief Returns if running
   Runing comprises initializing and reading data (i.e. not running is idle or error)
   We query the thread state to get this.
**/
bool QKinectWrapper::isRunning()
{
   if(QThread::isRunning())
      return true;
   return false;

}


/**
   \brief Returns if stopped
   Runing comprises initializing and reading data (i.e. not running is idle or error)
   We query the thread state to get this.
**/
bool QKinectWrapper::isStopped()
{
   if(QThread::isRunning())
      return false;
   return true;

}

/******************************************************************************
* PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED
******************************************************************************/


/**
   \brief Starts the kinect-dedicated reading thread.
   Does the following:
   1. Notify the start of the initialization and initialize
   2. Notify the initialization outcomes (error, or running if initialization was successful)
   3. If successful, does a continous:
   3.1. Wait/update kinect data
   3.2. Protected by a mutex, generate the images and data structures, made available to the user
   3.3. Check if a stop has been requested
   4. Stop the kinect reading and release resources, notify.

   The variable accessed outside of this thread are protected by mutexes. This includes:
   - The status
   - The QImages for depth, camera
   - The bodies
   - etc.
**/
void QKinectWrapper::run()
{
   mutex.lock();
   status=QKinect::Initializing;
   emit statusNotification(status);
   mutex.unlock();

   bool ok = initialize();

   if(!ok)
   {
      mutex.lock();
      status = QKinect::ErrorStop;
      emit statusNotification(status);
      mutex.unlock();
      return;
   }

   mutex.lock();
   status = QKinect::OkRun;
   emit statusNotification(status);
   mutex.unlock();

   frameid=0;
   while(!t_requeststop)
   {
      //double t1,t2;
      //t1 = PreciseTimer::QueryTimer();
      XnStatus status = g_Context.WaitAndUpdateAll();
      //msleep(100+(rand()%100));   // simulate some shit delay
      //if( (frameid%100) > 50)
        // msleep(((frameid%100)-50)*10);           // simulate some slowing down delay delay
      //t2 = PreciseTimer::QueryTimer();
      //printf("Waitandupdate: %lf. %s\n",(t2-t1)*1000.0,xnGetStatusString(status));

      // Prepare the data to export outside of the thread
      mutex.lock();
      xn::DepthMetaData depthMD;
      g_DepthGenerator.GetMetaData(depthMD);
      //frameid = depthMD.FrameID();
      frameid++;
      //printf("frame id: %d %d\n",frameid,depthMD.FrameID());
      timestamp = (double)depthMD.Timestamp()/1000000.0;
      // Must create the bodies first
      bodies = createBodies();
      // Then can create the images, which may overlay the bodies
      imageCamera = createCameraImage();
      imageDepth = createDepthImage();
      emit dataNotification();
      mutex.unlock();


   }
   g_Context.Shutdown();

   mutex.lock();
   status = QKinect::Idle;
   emit statusNotification(status);
   mutex.unlock();
}


/**
   \brief Setups and initializes the kinect/openni
   Returns false if any step fails.
**/
bool QKinectWrapper::initialize()
{
   XnStatus nRetVal = XN_STATUS_OK;
   XnMapOutputMode outputMode;
   XnCallbackHandle hUserCallbacks;
   //XnCallbackHandle hCalibrationCallbacks, hPoseCallbacks;
   XnCallbackHandle hCalibrationStart, hCalibrationComplete,hPoseDetected;
   XnCallbackHandle hHandCreate, hHandUpdate, hHandDestroy, hHand;

   // Initialize the context
   nRetVal = g_Context.Init();
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Context creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }

   // Create a depth generator
   nRetVal = g_DepthGenerator.Create(g_Context);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Depth generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }
   outputMode.nXRes = 640;
   outputMode.nYRes = 480;
   outputMode.nFPS = 30;
   nRetVal = g_DepthGenerator.SetMapOutputMode(outputMode);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Depth generator SetMapOutputMode failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }


   // Create an image generator
   nRetVal = g_ImageGenerator.Create(g_Context);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Image generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }
   outputMode.nXRes = 640;
   outputMode.nYRes = 480;
   outputMode.nFPS = 30;
   nRetVal = g_ImageGenerator.SetMapOutputMode(outputMode);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Image generator SetMapOutputMode failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }

   // Creates a user generator
   nRetVal = g_UserGenerator.Create(g_Context);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("User generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }

   // See if we've skeleton capabilities
   if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
   {
      mutex.lock();
      errorMsg = QString("user generator doesn't support skeleton");
      mutex.unlock();
      return false;
   }

   g_UserGenerator.RegisterUserCallbacks(QKinectWrapper::User_NewUser, QKinectWrapper::User_LostUser, this, hUserCallbacks);
   // API v 1.0:
   //g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(QKinectWrapper::UserCalibration_CalibrationStart, QKinectWrapper::UserCalibration_CalibrationEnd, this, hCalibrationCallbacks);
   // API v 1.1+:
   g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(QKinectWrapper::UserCalibration_CalibrationStart,this,hCalibrationStart);
   g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(QKinectWrapper::UserCalibration_CalibrationComplete,this,hCalibrationComplete);

   if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
   {
      g_bNeedPose = TRUE;
      if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
      {
         mutex.lock();
         errorMsg = QString("User generator needs pose, but not supported\n");
         mutex.unlock();
         return false;
      }

      // API v 1.0:
      //g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(QKinectWrapper::UserPose_PoseDetected, NULL, this, hPoseCallbacks);
      // API v 1.1+:
      g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(QKinectWrapper::UserPose_PoseDetected, this, hPoseDetected);

      g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
   }

   g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);


   /*
   // Creates a hand generator
   printf("Before hand create\n");
   nRetVal = g_HandsGenerator.Create(g_Context);
   printf("After hand create\n");
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }

   nRetVal = g_HandsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this,hHand);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator can't register callbacks: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }


   printf("Before gesture create\n");
   nRetVal = g_GestureGenerator.Create(g_Context);
   printf("After gesture create\n");
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Gesture generator creation failed: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }





   nRetVal = g_GestureGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this,hHand);
   if(nRetVal!=XN_STATUS_OK)
   {
      mutex.lock();
      errorMsg = QString("Hands generator can't register callbacks: %1").arg(xnGetStatusString(nRetVal));
      mutex.unlock();
      return false;
   }
*/


   // Start producting data
   nRetVal = g_Context.StartGeneratingAll();



   return true;

}

/**
   \brief User found
   Emits a user found notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   if (pthis->g_bNeedPose)
      pthis->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(pthis->g_strPose, nId);
   else
      pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
   emit pthis->userNotification(nId,true);
}

/**
   \brief User lost
   Emits a user lost notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   emit pthis->userNotification(nId,false);
}

/**
   \brief Pose detected
   Emits a pose notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{

   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   pthis->g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
   pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
   emit pthis->poseNotification(nId,QString(strPose));
}


/**
   \brief Start calibration
   Emits a calibration notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   //printf("----------------- CALIBRATION STARTED for USER %d\n", nId);
   emit pthis->calibrationNotification(nId,CalibrationStart);
}

/**
   \brief Finished calibration
   Emits a calibration notification
**/
void XN_CALLBACK_TYPE QKinectWrapper::UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus calibrationError, void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   if (calibrationError == XN_CALIBRATION_STATUS_OK)
   {
      pthis->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
      emit pthis->calibrationNotification(nId,CalibrationEndSuccess);
      return;
   }
   // Calibration failed
   if (pthis->g_bNeedPose)
   {
      pthis->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(pthis->g_strPose, nId);
   }
   else
   {
      pthis->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
   }
   emit pthis->calibrationNotification(nId,CalibrationEndFail);
}

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Create(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;

   printf("Hand create. id %d. position: %f %f %f. time: %f\n",nId,pPosition->X,pPosition->Y,pPosition->Z,time);
}*/

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Destroy(xn::HandsGenerator& generator, XnUserID nId, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;

   printf("Hand create. id %d. time: %f\n",nId,time);
}*/

/**
  \brief
**/
/*void XN_CALLBACK_TYPE QKinectWrapper::Hand_Update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat time,void* pCookie)
{
   QKinectWrapper *pthis = (QKinectWrapper*)pCookie;
   printf("Hand update. id %d. position: %f %f %f. time: %f\n",nId,pPosition->X,pPosition->Y,pPosition->Z,time);
}*/

/**
  \brief Converts joint to the projective, marks if the projection is valid (joint confidence is high enough)
**/
void QKinectWrapper::getJointProj(XnSkeletonJointPosition joint,XnPoint3D &proj,bool &valid)
{
   if(joint.fConfidence<0.5)
   {
      valid = false;
   }
   else
   {
      valid = true;
      g_DepthGenerator.ConvertRealWorldToProjective(1, &joint.position, &proj);
   }
}

/**
  \brief Fill the Bodies data structure holding all the infos about the detected users.
**/
Bodies QKinectWrapper::createBodies()
{

   Bodies bodies;

   Body b;


   XnUserID aUsers[15];
   XnUInt16 nUsers = 15;
   g_UserGenerator.GetUsers(aUsers, nUsers);
   for (int i = 0; i < nUsers; ++i)
   {
      // id
      b.id = aUsers[i];
      // Get the center of mass and its projection
      g_UserGenerator.GetCoM(aUsers[i], b.com);
      g_DepthGenerator.ConvertRealWorldToProjective(1, &b.com, &b.proj_com);
      if(isnan(b.proj_com.X) || isnan(b.proj_com.Y) || isnan(b.proj_com.Z))
         b.proj_com_valid=false;
      else
         b.proj_com_valid=true;

      if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
      {
         // If the user is tracked, get the skeleton
         b.status=QKinect::Tracking;
         //b.tracked=true;
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD, b.joints[QKinect::Head]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_NECK, b.joints[QKinect::Neck]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_SHOULDER, b.joints[QKinect::LeftShoulder]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_ELBOW, b.joints[QKinect::LeftElbow]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, b.joints[QKinect::LeftHand]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_SHOULDER, b.joints[QKinect::RightShoulder]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_ELBOW, b.joints[QKinect::RightElbow]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, b.joints[QKinect::RightHand]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, b.joints[QKinect::Torso]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HIP, b.joints[QKinect::LeftHip]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_KNEE, b.joints[QKinect::LeftKnee]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_FOOT, b.joints[QKinect::LeftFoot]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HIP, b.joints[QKinect::RightHip]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_KNEE, b.joints[QKinect::RightKnee]);
         g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_FOOT, b.joints[QKinect::RightFoot]);

         // Get the projection of the skeleton
         for(unsigned i=QKinect::Head;i<=QKinect::RightFoot;i++)
            getJointProj(b.joints[i],b.proj_joints[i],b.proj_joints_valid[i]);
      }
      else
      {
         //b.tracked=false;
         if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUsers[i]))
            b.status=QKinect::Calibrating;
         else
            b.status=QKinect::LookingForPose;
      }
      bodies.push_back(b);
   }
   return bodies;
}

/**
  \brief Creates a QImage comprising the depth map
**/
QImage QKinectWrapper::createDepthImage()
{
   // Here must mutex / run also access the data
   xn::SceneMetaData smd;
   xn::DepthMetaData dmd;
   g_DepthGenerator.GetMetaData(dmd);
   g_UserGenerator.GetUserPixels(0, smd);

   XnUInt16 g_nXRes = dmd.XRes();
   XnUInt16 g_nYRes = dmd.YRes();

   QImage image(g_nXRes,g_nYRes,QImage::Format_RGB32);


   const XnDepthPixel* pDepth = dmd.Data();
   const XnLabel* pLabels = smd.Data();

   // Compute stats
   /*unsigned max,min;
   max = pDepth[0];
   min = 0;
   for (unsigned i=0; i<g_nYRes*g_nXRes; i++)
   {
      if(pDepth[i]>max)
         max = pDepth[i];
      if(pDepth[i]!=0)
      {
         if(min==0)
            min = pDepth[i];
         else
            if(pDepth[i]<min)
               min = pDepth[i];
      }
   }
   printf("Depth min/max: %u %u\n",min,max);*/

   for (unsigned nY=0; nY<g_nYRes; nY++)
   {
      uchar *imageptr = image.scanLine(nY);

      for (unsigned nX=0; nX < g_nXRes; nX++)
      {
         unsigned depth = *pDepth;
         unsigned label = *pLabels;


         unsigned maxdist=10000;
         if(depth>maxdist) depth=maxdist;
         if(depth)
         {
            depth = (maxdist-depth)*255/maxdist+1;
         }
         // depth: 0: invalid
         // depth: 255: closest
         // depth: 1: furtherst (maxdist distance)


         if(label)
         {
            imageptr[0] = BodyColors[label][0]*2*depth/255;
            imageptr[1] = BodyColors[label][1]*2*depth/255;
            imageptr[2] = BodyColors[label][2]*2*depth/255;
            imageptr[3] = 0xff;
         }
         else
         {
            // Here we could do depth*color, to show the colored depth
            imageptr[0] = depth;
            imageptr[1] = depth;
            imageptr[2] = depth;
            imageptr[3] = 0xff;
         }
         pDepth++;
         imageptr+=4;
         pLabels++;
      }
   }


   QPainter painter;
   painter.begin(&image);
   if(displayInfoDepth)
   {
      painter.setPen(textPen);
      painter.setFont(font);
      drawInfo(&painter);
   }
   if(displaySkeletonDepth)
   {
      painter.setPen(skeletonPen);
      drawSkeleton(&painter);
   }
   painter.end();
   return image;

}


/**
  \brief Creates a QImage comprising the depth map
**/
QImage QKinectWrapper::createCameraImage()
{

   // Here must mutex / run also access the data
   xn::DepthMetaData dmd;
   xn::ImageMetaData imd;
   g_DepthGenerator.GetMetaData(dmd);
   g_ImageGenerator.GetMetaData(imd);

   XnUInt16 g_nXRes = dmd.XRes();
   XnUInt16 g_nYRes = dmd.YRes();

   QImage image(g_nXRes,g_nYRes,QImage::Format_RGB32);


   const XnUInt8 *idata = imd.Data();
   for (unsigned nY=0; nY<g_nYRes; nY++)
   {
      uchar *imageptr = image.scanLine(nY);

      for (unsigned nX=0; nX < g_nXRes; nX++)
      {
         imageptr[0] = idata[2];
         imageptr[1] = idata[1];
         imageptr[2] = idata[0];
         imageptr[3] = 0xff;

         imageptr+=4;
         idata+=3;

      }
   }
   QPainter painter;
   painter.begin(&image);
   if(displayInfoImage)
   {
      painter.setPen(textPen);
      painter.setFont(font);
      drawInfo(&painter);
   }
   if(displaySkeletonImage)
   {
      painter.setPen(skeletonPen);
      drawSkeleton(&painter);
   }
   painter.end();

   return image;

}



/**
  \brief Display infos about the detected bodies
**/
void QKinectWrapper::drawInfo(QPainter *painter)
{
   char str[256];
   for(unsigned i=0;i<bodies.size();i++)
   {
      if(bodies[i].status == QKinect::Tracking)
      {
         sprintf(str, "%d: tracking", bodies[i].id);
      }
      if(bodies[i].status == QKinect::Calibrating)
      {
         sprintf(str, "%d: calibrating...", bodies[i].id);
      }
      if(bodies[i].status == QKinect::LookingForPose)
      {
         sprintf(str, "%d: looking for pose", bodies[i].id);
      }
      painter->drawText(bodies[i].proj_com.X, bodies[i].proj_com.Y,QString(str));
   }
}

/**
  \brief Draws on painter the skeleton of all bodies
**/

void QKinectWrapper::drawSkeleton(QPainter *painter)
{
   for(unsigned i=0;i<bodies.size();i++)
   {
      drawLimb(painter,bodies[i],QKinect::Head,QKinect::Neck);

      drawLimb(painter,bodies[i],QKinect::Neck,QKinect::LeftShoulder);
      drawLimb(painter,bodies[i],QKinect::LeftShoulder,QKinect::LeftElbow);
      drawLimb(painter,bodies[i],QKinect::LeftElbow,QKinect::LeftHand);

      drawLimb(painter,bodies[i],QKinect::Neck,QKinect::RightShoulder);
      drawLimb(painter,bodies[i],QKinect::RightShoulder,QKinect::RightElbow);
      drawLimb(painter,bodies[i],QKinect::RightElbow,QKinect::RightHand);

      drawLimb(painter,bodies[i],QKinect::LeftShoulder,QKinect::Torso);
      drawLimb(painter,bodies[i],QKinect::RightShoulder,QKinect::Torso);

      drawLimb(painter,bodies[i],QKinect::Torso,QKinect::LeftHip);
      drawLimb(painter,bodies[i],QKinect::LeftHip,QKinect::LeftKnee);
      drawLimb(painter,bodies[i],QKinect::LeftKnee,QKinect::LeftFoot);

      drawLimb(painter,bodies[i],QKinect::Torso,QKinect::RightHip);
      drawLimb(painter,bodies[i],QKinect::RightHip,QKinect::RightKnee);
      drawLimb(painter,bodies[i],QKinect::RightKnee,QKinect::RightFoot);

      drawLimb(painter,bodies[i],QKinect::LeftHip,QKinect::RightHip);
   }
}


/**
  \brief Draws on painter the limb j1-j2 of body
**/
void QKinectWrapper::drawLimb(QPainter *painter,const Body &body, BodyJoints j1,BodyJoints j2)
{
   if(body.proj_joints_valid[j1]==false || body.proj_joints_valid[j2]==false)
      return;
   painter->drawLine(body.proj_joints[j1].X,body.proj_joints[j1].Y,body.proj_joints[j2].X,body.proj_joints[j2].Y);
}


}     // End namespace




