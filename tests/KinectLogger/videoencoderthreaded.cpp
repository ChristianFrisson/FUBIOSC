/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "videoencoderthreaded.h"
#include <QMutexLocker>




/******************************************************************************
*******************************************************************************
* VideoEncoderThreaded   VideoEncoderThreaded   VideoEncoderThreaded   VideoEncoderThreaded
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/

VideoEncoderThreaded::VideoEncoderThreaded()
{
   encodedframes=0;
   encodedsize=0;

   delay=1000;
}

VideoEncoderThreaded::~VideoEncoderThreaded()
{
   close();
}

/**
   \brief Start to encode videos in the given filename

   Returns true in case of success.

   fps:
      fps!=0: fixed frame rate. With fixed frame rate, the time stamp passed to encodeImage is ignored.
      fps==0: variable frame rate. In that case the time base is 1 millisecond, and the frames pts is set with
      millisecond accuracy to the time stamp passed to encodeImage.

   vmaxbuf: maximum video buffer size. -1 for unlimited.

**/
bool VideoEncoderThreaded::createFile(QString filename,unsigned width,unsigned height,unsigned bitrate,unsigned gop,unsigned fps,bool vfr,int vmaxbuf)
{
   bool ok;
   // if we're already running, then first terminate the existing task
   if(isRunning())
      close();

   t_requeststop=false;

   // If vfr: variable presentation time->set time base to 1000fps (1millisecond), increase the bitrate since the rate control allocates bits to frames, and
   // there are is 'fps' true frames per 1000 logical.
   if(vfr)
      ok = encoder.createFile(filename,width,height,bitrate*1000/fps,gop,1000);
   else
      ok = encoder.createFile(filename,width,height,bitrate,gop,fps);
   if(!ok)
      return ok;

   // Keep some params
   VideoEncoderThreaded::vfr = vfr;
   VideoEncoderThreaded::fps = fps;
   VideoEncoderThreaded::vmaxbuf = vmaxbuf;
   firstframe = true;


   // Successful -> start the thread for the encoding
   start();
   return true;
}

bool VideoEncoderThreaded::close()
{
   // If we aren't running then we return false
   if(!isRunning())
      return false;

   // Signal the thread that we want it to complete the encoding task
   t_requeststop=true;

   // Wait for the thread to complete it's encoding
   wait();

   // Reset the counters
   encodedframes=0;
   encodedsize=0;
   firstframe = false;

   return encoder.close();

}


/**
   \brief Adds a frame to encode.

   img: frame to encode
   t: timestamp of the frame in second. This is ignored for a fixed frame rate video.
   For a variable frame rate video this is used to set the presentation time stamp.
   t must be monotonously increasing.

   The method takes care to set presentation time stamps relative to the time of the first frame.
   Thus the first frame always gets a pts of 0.

**/
void VideoEncoderThreaded::encodeImage(const QImage &img,double t)
{
   QMutexLocker locker(&mutex);

   if(firstframe)
   {
      timefirstframe = t;
      firstframe = false;
   }

   // If vmaxbuf>=0 (there's a limit to buffering), and the work queue is full, then do nothing (drop frame, no buffering)
   // vmaxbuf=0: size must be 0 to push a frame
   // vmaxbuf>=1: size must be < vmaxbuf to push a frame
   if(vmaxbuf>=0 && ((vmaxbuf==0 && workload.size()>0) || (vmaxbuf>0 && workload.size()>=vmaxbuf)))
   {
       printf("return w/o encoding: maxbuf: %d size: %d\n",vmaxbuf,workload.size());
       return;
   }
   VideoEncoderThreadedData v;
   v.img = img;
   v.t = t;
   workload.push_back(v);
}

unsigned VideoEncoderThreaded::getEncodedFramesCount()
{
   QMutexLocker locker(&mutex);
   return encodedframes;
}

unsigned VideoEncoderThreaded::getEncodedSize()
{
   QMutexLocker locker(&mutex);
   return encodedsize;
}

unsigned VideoEncoderThreaded::getUnencodedFramesCount()
{
   QMutexLocker locker(&mutex);
   return workload.size();

}

/*void VideoEncoderThreaded::setdel(int value)
{
   QMutexLocker locker(&mutex);
   delay = value*1000;
}*/

/******************************************************************************
* INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL
******************************************************************************/

void VideoEncoderThreaded::run()
{
   VideoEncoderThreadedData data;
   unsigned size;
   int s;
   while(1)
   {
      // Get a reference to the picture to encode
      mutex.lock();
      size = workload.size();
      if(size)
      {
         data = workload.front();
         workload.erase(workload.begin());
      }
      mutex.unlock();

      if(size==0)
      {
         // Nothing to do: sleep a bit
         usleep(10000);
      }
      else
      {
         //printf("VideoEncoderThreaded::run. Workload: %d. Proc %.5f. %d %d %d\n",size,data.t,data.img.width(),data.img.height(),data.img.format());
         // Encode the video
         if(vfr)
            s = encoder.encodeImagePts(data.img,(data.t-timefirstframe)*1000.0);
         else
            s = encoder.encodeImage(data.img);
         encodedsize += s;
         encodedframes++;

         //printf("Encoded size: %d\n",s);
         // Simulate lengthy encodings
         // usleep(delay);
         //msleep(100);
      }

      // Exit only if we have no workload
      if(t_requeststop && workload.size()==0)
         break;
   }
}


