/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __WRITER_VID_H
#define __WRITER_VID_H

#ifdef WRITEVIDEO

#include <QFile>
#include <QTextStream>
#include "QKinectWrapper.h"
#include "QStreamSrv.h"
#include "keyfilter.h"
#include "videoencoderthreaded.h"

class KWriterVideo : public QObject
{
   Q_OBJECT
public:
   KWriterVideo();
   virtual ~KWriterVideo();

   int start(QString fname,unsigned bitrate,bool vfr,int vmaxbuf,QKinect::QKinectWrapper *k);
   void stop();

   unsigned getEncodedFramesCount();
   unsigned getEncodedSize();
   unsigned getUnencodedFramesCount();

   //void setdel(int value);

private:
   QKinect::QKinectWrapper *kinect;
   QString fname;
   QThread thread;
   VideoEncoderThreaded encoderth;
   QImage frame;
   QPainter painter;
   QFont font;
   int fontheight;
   int encodesize;      // Number of bytes encoded



private slots:
   void dataNotification();
   void stopt();
};



#endif

#endif // __WRITER_VID_H
