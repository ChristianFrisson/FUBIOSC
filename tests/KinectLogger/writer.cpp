/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "writer.h"
#include "precisetimer.h"


KWriter::KWriter()
   : streamSrv(this)                      // Required for moveToThread
{
   kinect=0;
   ke=0;

}

KWriter::~KWriter()
{
   stop();

}

void KWriter::stop()
{
   // Disconnect the kinect data notification to this object
   disconnect(kinect);

   // First we must terminate the thread and only after close the file
   thread.exit();
   thread.wait();

   file.close();
}
/**
  \brief Start accepting kinect data and serving / storing them

  Returns a bitmask:
   0:    OK
   1:    File creation error
   2:    Server creation error


**/
int KWriter::start(QString fname,quint16 port,unsigned numuser,QKinect::QKinectWrapper *k,KeyPressEater *ke)
{
   KWriter::fname=fname;
   KWriter::port=port;
   KWriter::numuser=numuser;
   kinect=k;
   KWriter::ke = ke;
   label=0;

   // Start a file stream if requested (file specified)
   if(!fname.isEmpty())
   {
      file.setFileName(fname);
      if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text))
         return 1;
      stream.setDevice(&file);
      stream.setRealNumberNotation(QTextStream::FixedNotation);
      stream.setRealNumberPrecision(12);
   }


   // Start a streaming server if requested (port specified)
   if(port)
   {
      if(!streamSrv.start(port))
         return 2;
   }

   // Connect the kinect data notification to this object
   connect(kinect,SIGNAL(dataNotification()),this,SLOT(dataNotification()));

   // Connect the key generator to this object
   connect(ke,SIGNAL(Key(int)),this,SLOT(key(int)));


   moveToThread(&thread);
   thread.start();

   return 0;

}


int KWriter::getNumClients()
{

   if(!streamSrv.isRunning())
      return -1;
   return streamSrv.getNumClients();
}

/**
  Format of the data in the file:

      LTime KTime FID <Body> <Body> ....
   Body:
      id status tracking

**/
void KWriter::dataNotification()
{
   QString str;
   QTextStream ss(&str);
   ss.setRealNumberNotation(QTextStream::FixedNotation);
   ss.setRealNumberPrecision(12);

   // Label
   ss << label << " ";

   // Local time
   ss << (int)(PreciseTimer::QueryTimer()*1000) << " ";
   // Kinect time
   ss << (int)(kinect->getTimestamp()*1000) << " ";
   // Frame id
   ss << kinect->getFrameID() << " ";

   QKinect::Bodies bodies = kinect->getBodies();

   // Iterate the bodies

   QKinect::Body body;
   for(unsigned i=0;i<numuser;i++)
   {
      if(i<bodies.size())
      {
         ss << bodies[i].id << " ";
         ss << bodies[i].status << " ";

         ss << (int)bodies[i].com.X << " " << (int)bodies[i].com.Y << " " << (int)bodies[i].com.Z << " ";
         if(bodies[i].proj_com_valid)
            //ss << (int)bodies[i].proj_com.X << " " << (int)bodies[i].proj_com.Y << " " << (int)bodies[i].proj_com.Z << " ";
            ss << (int)bodies[i].proj_com.X << " " << (int)bodies[i].proj_com.Y << " ";
         else
            //ss << "NaN NaN NaN ";
            ss << "NaN NaN ";

         if(bodies[i].status==QKinect::Tracking)
         {
            for(unsigned j=0;j<15;j++)
            {
               ss << "\t";
               ss << (int)bodies[i].joints[j].position.X << " " << (int)bodies[i].joints[j].position.Y << " " << (int)bodies[i].joints[j].position.Z << " "  << (int)(bodies[i].joints[j].fConfidence*1000) << "   ";
               if(bodies[i].proj_joints_valid[j])
                  //ss << (int)bodies[i].proj_joints[j].X << " " << (int)bodies[i].proj_joints[j].Y << " " << (int)bodies[i].proj_joints[j].Z << " ";
                  ss << (int)bodies[i].proj_joints[j].X << " " << (int)bodies[i].proj_joints[j].Y << " " ;
               else
                  //ss << "NaN NaN NaN ";
                  ss << "NaN NaN ";
            }
         }
         else
         {
            for(unsigned j=0;j<15;j++)
            {
               ss << "\t";
               //for(unsigned k=0;k<7;k++)
               for(unsigned k=0;k<6;k++)
                  ss << "NaN ";
            }
         }
      }
      else
      {
         //for(unsigned k=0;k<8+15*7;k++)
         for(unsigned k=0;k<7+15*6;k++)
            ss << "NaN ";
      }

   }

   ss << endl;

   // Write / serve

   // Should only do so if a file or server is open - stream and streamSrv handle this okay.
   //printf("status: %d\n",stream.status());

   if(stream.device())
      stream << str;
   streamSrv.write(str);
}

/**
  \brief Receive key presses
**/
void KWriter::key(int k)
{
   label=k;
}


