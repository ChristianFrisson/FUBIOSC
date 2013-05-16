/*
   Copyright (C) 2011:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __WRITER_H
#define __WRITER_H

#include <QFile>
#include <QTextStream>
#include "QKinectWrapper.h"
#include "QStreamSrv.h"
#include "keyfilter.h"


class KWriter : public QObject
{
   Q_OBJECT
public:
   KWriter();
   virtual ~KWriter();

   int start(QString fname,quint16 port,unsigned numuser,QKinect::QKinectWrapper *k,KeyPressEater *ke);
   void stop();
   int getNumClients();


private:
   QKinect::QKinectWrapper *kinect;
   KeyPressEater *ke;
   QFile file;
   QTextStream stream;
   QStreamSrv streamSrv;
   QString fname;
   quint16 port;
   unsigned numuser;
   QThread thread;
   int label;



private slots:
      void dataNotification();
      void key(int k);

};

#endif // __WRITER_H
