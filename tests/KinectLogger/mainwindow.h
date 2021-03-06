/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QFile>
#include <QTextStream>

#include "QKinectWrapper.h"
#include "QStreamSrv.h"
#include "writer.h"
#include "writer_vid.h"
#include "keyfilter.h"


#include <XnFPSCalculator.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   explicit MainWindow(QString progname,bool plainimage,bool plaindepth,QString fname,QString fnamevideo,unsigned bitrate,unsigned numuser,quint16 port,bool vfr,int vmaxbuf,QWidget *parent = 0, Qt::WindowFlags f = 0);
   ~MainWindow();

public slots:
   void init();
   void kinectData();
   void kinectStatus(QKinect::KinectStatus);
   void kinectUser(unsigned,bool);
   void kinectPose(unsigned,QString);
   void kinectCalibration(unsigned,QKinect::CalibrationStatus);


protected:
   virtual void closeEvent( QCloseEvent * event);
   void stop();

private slots:
   void key(int k);
   void help();
   void about();



private:
   Ui::MainWindow *ui;
   QKinect::QKinectWrapper kreader;
   QLabel *sbKinectStatus;
   QLabel *sbKinectFrame;
   QLabel *sbKinectTime;
   QLabel *sbKinectFPS;
   QLabel *sbKinectNumBody;
   QLabel *sbLabel;
   QLabel *sbFile;
   QLabel *sbServer;
   QLabel *sbClients;
   QLabel *sbSystime;
   QLabel *sbRuntime;
   XnFPSData xnFPS;
   KWriter writer;
   QString progname,fname;
   unsigned numuser;
   quint16 port;
   bool vfr;
   int vmaxbuf;
   double timeFirstData;
   bool firstData;
   KeyPressEater ke;

#ifdef WRITEVIDEO
   QLabel *sbFileVideo;
   QLabel *sbFileVideoSize;
   QLabel *sbVideoWorkload;
   KWriterVideo writervideo;
   unsigned bitrate;
   QString fnamevideo;
#endif

protected:
};

#endif // MAINWINDOW_H
