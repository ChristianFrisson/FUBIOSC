/*
   QStreamSrv

   Copyright (C) 2011:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <QThread>
#include "QStreamSrv.h"


QStreamSrv::QStreamSrv(QObject *parent) :
   QObject(parent),
   TcpServer(parent)
{
   running=false;
}

QStreamSrv::~QStreamSrv()
{
   for(unsigned i=0;i<TcpSockets.size();i++)
      delete TcpSockets[i];
   TcpSockets.clear();
   TcpServer.close();
}
bool QStreamSrv::start(quint16 port)
{
   if(running)
      return false;

   bool ok;
   QHostAddress ha(QHostAddress::Any);
   ok = TcpServer.listen(ha,port);
   if(!ok)
   {
     //printf("Can't create server\n");
     return false;
   }

   connect(&TcpServer,SIGNAL(newConnection()),this,SLOT(newConnection()));
   running=true;
   return true;
}

bool QStreamSrv::isRunning()
{
   return running;
}
unsigned QStreamSrv::getNumClients()
{
   return TcpSockets.size();
}

bool QStreamSrv::write(QString str)
{
   if(!isRunning())
      return false;

   bool success=true;
   for(unsigned i=0;i<TcpSockets.size();i++)
   {
      qint64 rv;
      rv = TcpSockets[i]->write(str.toAscii());
      if(rv==-1)
         success=false;
   }
   return success;
}

void QStreamSrv::newConnection()
{
   printf("----------------- New connection (thread %p)!\n",QThread::currentThread());
   QTcpSocket * s = TcpServer.nextPendingConnection();
   TcpSockets.push_back(s);

   // Set-up a disconnection handler
   connect(s,SIGNAL(disconnected()),this,SLOT(disconnected()));

   /*QHostAddress ha;
   printf("Peer name: %s\n",s->peerName().toStdString().c_str());
   ha = s->peerAddress();
   printf("Peer address: %s\n",ha.toString().toStdString().c_str());
   ha = s->localAddress();
   printf("Local address: %s\n",ha.toString().toStdString().c_str());*/
}
void QStreamSrv::disconnected()
{
   printf("--------------------------------Disconnection (thread %p)!\n",QThread::currentThread());

   QObject *s = sender();
   for(unsigned i=0;i<TcpSockets.size();i++)
   {
      if(s == TcpSockets[i])
      {
         TcpSockets[i]->deleteLater();
         TcpSockets.erase(TcpSockets.begin()+i);
         break;
      }
   }

}
