/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VIDEOENCODERTHREADED_H
#define VIDEOENCODERTHREADED_H

//#ifdef WRITEVIDEO

#include <QThread>
#include <QMutex>

#include "QVideoEncoder.h"

#include <list>



typedef struct
{
   QImage img;
   double t;
} VideoEncoderThreadedData;

class VideoEncoderThreaded : public QThread
{
   Q_OBJECT
public:
   VideoEncoderThreaded();
   ~VideoEncoderThreaded();

   bool createFile(QString filename,unsigned width,unsigned height,unsigned bitrate,unsigned gop,unsigned fps=25,bool vfr=false, int vmaxbuf=-1);
   virtual bool close();

   virtual void encodeImage(const QImage &,double t);
   virtual unsigned getEncodedFramesCount();
   virtual unsigned getEncodedSize();
   virtual unsigned getUnencodedFramesCount();

   void run();

   void setdel(int value);



protected:
   QMutex mutex;
   QVideoEncoder encoder;
   bool t_requeststop;
   bool firstframe;
   double timefirstframe;
   unsigned fps;
   bool vfr;
   int vmaxbuf;
   unsigned encodedsize;
   unsigned encodedframes;
   std::list<VideoEncoderThreadedData> workload;
   int delay;

};


//#endif

#endif // VIDEOENCODERTHREADED_H
