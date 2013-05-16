/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Only do the stuff if requested to
#ifdef WRITEVIDEO

#include <QTimer>
#include "writer_vid.h"
#include "precisetimer.h"



KWriterVideo::KWriterVideo() :
   frame(1280,480,QImage::Format_RGB32),
   painter(&frame)
{
   kinect=0;

   encodesize=0;

   painter.setPen(QPen(QBrush(Qt::white),5));
   painter.setBackground(QBrush(Qt::black));
   painter.setBackgroundMode(Qt::OpaqueMode);
   font.setPointSize(16);
   painter.setFont(font);
   QFontMetrics fm(font);
   fontheight = fm.height();

}

KWriterVideo::~KWriterVideo()
{
   stop();
}

void KWriterVideo::stop()
{
   // Disconnect the kinect data notification to this object
   disconnect(kinect);
   stopt();
}
void KWriterVideo::stopt()
{
   // First we must terminate the thread and only after close the encoders
   thread.exit();
   thread.wait();

   encoderth.close();
}

/**
  \brief Start accepting kinect data and serving / storing them

  Returns a bitmask:
   0:    OK
   1:    File creation error
**/
int KWriterVideo::start(QString fname,unsigned bitrate,bool vfr, int vmaxbuf, QKinect::QKinectWrapper *k)
{
   KWriterVideo::fname=fname;
   kinect=k;

   unsigned gop = 20;         // Number of frames between key frames
   unsigned fps = 30;         // Frame per second. Kinect is at 30.

   // Create the encoder
   if(!encoderth.createFile(fname,frame.width(),frame.height(),bitrate,gop,fps,vfr,vmaxbuf))
   {
      return 1;
   }


   // Connect the kinect data notification to this object
   connect(kinect,SIGNAL(dataNotification()),this,SLOT(dataNotification()));


   moveToThread(&thread);
   thread.start();

   return 0;

}



/**
  Format of the data in the file:

      LTime KTime FID <Body> <Body> ....
   Body:
      id status tracking

**/
void KWriterVideo::dataNotification()
{
   double t1,t2,t3,t4;
   t1 = PreciseTimer::QueryTimer();


   // The image on which we draw the frames
   QImage cam,depth;
   double ts;
   unsigned fid;

   kinect->getCameraDepth(cam,depth,ts,fid);

   painter.drawImage(0,0,depth);
   painter.drawImage(640,0,cam);

   // Write additional infos
   QString info;
   info.sprintf("%06.3f %06.3f %06u",PreciseTimer::QueryTimer(),ts,fid);

   painter.drawText(0,fontheight,info);


   t2 = PreciseTimer::QueryTimer();
   encoderth.encodeImage(frame,ts);
   t3 = PreciseTimer::QueryTimer();


   //printf("%05d %.4f. Setup: %.4f. EncInThrd: %.4f. Tot%.4f.\n",fid,ts,t2-t1,t3-t2,t3-t1);




}


unsigned KWriterVideo::getEncodedFramesCount()
{
   return encoderth.getEncodedFramesCount();
}

unsigned KWriterVideo::getEncodedSize()
{
   return encoderth.getEncodedSize();
}
unsigned KWriterVideo::getUnencodedFramesCount()
{
   return encoderth.getUnencodedFramesCount();
}

/*void KWriterVideo::setdel(int value)
{
   encoderth.setdel(value);
}*/

#endif


