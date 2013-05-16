/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QWidget>
#include <QEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QResource>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include "helpdialog.h"
#include "precisetimer.h"
#include "cio.h"

/**
   \brief Mainwindow UI initialization. Reader/writer initialization in init slot.
**/
MainWindow::MainWindow(QString progname,bool plainimage,bool plaindepth,QString fname,QString fnamevideo,unsigned bitrate,unsigned numuser,quint16 port,bool vfr,int vmaxbuf,QWidget *parent,Qt::WindowFlags f) :
   QMainWindow(parent,f),
   ui(new Ui::MainWindow)
{
   //ConsoleInit();

   // Keep the params
   MainWindow::progname=progname;
   MainWindow::fname=fname;
   MainWindow::numuser=numuser;
   MainWindow::port=port;
   MainWindow::vfr=vfr;
   MainWindow::vmaxbuf=vmaxbuf;
#ifdef WRITEVIDEO
   MainWindow::fnamevideo=fnamevideo;
   MainWindow::bitrate=bitrate;
#endif

   // Set the overlay on the camera
   if(plainimage)
   {
      kreader.setDisplaySkeletonImage(false);
      kreader.setDisplayInfoImage(false);
   }
   if(plaindepth)
   {
      kreader.setDisplaySkeletonDepth(false);
      kreader.setDisplayInfoDepth(false);
   }

   // Not yet received data
   firstData=true;

   // Setup ui
   ui->setupUi(this);

   // Complete ui setup with status bar labels
   sbKinectStatus = new QLabel(statusBar());
   sbKinectFrame = new QLabel(statusBar());
   sbKinectTime = new QLabel(statusBar());
   sbKinectFPS = new QLabel(statusBar());
   sbKinectNumBody = new QLabel(statusBar());
   sbFile = new QLabel(statusBar());
   sbServer = new QLabel(statusBar());
   sbClients = new QLabel(statusBar());
   sbSystime = new QLabel(statusBar());
   sbRuntime = new QLabel(statusBar());
   sbLabel = new QLabel(statusBar());
#ifdef WRITEVIDEO
   sbFileVideo = new QLabel(statusBar());
   sbFileVideoSize = new QLabel(statusBar());
   sbVideoWorkload = new QLabel(statusBar());
#endif

   ui->statusBar->addWidget(sbKinectStatus);
   ui->statusBar->addWidget(sbKinectFrame);
   ui->statusBar->addWidget(sbKinectTime);
   ui->statusBar->addWidget(sbKinectFPS);
   ui->statusBar->addWidget(sbKinectNumBody);
   ui->statusBar->addWidget(sbFile);
#ifdef WRITEVIDEO
   ui->statusBar->addWidget(sbFileVideo);
   ui->statusBar->addWidget(sbFileVideoSize);
   ui->statusBar->addWidget(sbVideoWorkload);
#endif
   ui->statusBar->addWidget(sbServer);
   ui->statusBar->addWidget(sbClients);
   ui->statusBar->addWidget(sbSystime);
   ui->statusBar->addWidget(sbRuntime);
   ui->statusBar->addWidget(sbLabel);

   // Menu
   menuBar()->addAction("&About",this,SLOT(about()));
   menuBar()->addAction("&Help",this,SLOT(help()));


   // Fire the rest of the initialization once the window is displayed
   QTimer::singleShot(0, this, SLOT(init()));
}


/**
   \brief Destructor: terminates the readers and writers.
   NOTE: changed: the readers/writers are terminated in the closeevent now
**/
MainWindow::~MainWindow()
{
   delete ui;
}

/**
   \brief Reader/writer initialization here
**/
void MainWindow::init()
{
   // Image holder sizes
   ui->labelDepth->setMinimumSize(400,300);
   ui->labelImage->setMinimumSize(400,300);
   ui->labelDepth->setMaximumSize(640,480);
   ui->labelImage->setMaximumSize(640,480);

   connect(&kreader,SIGNAL(dataNotification()),this,SLOT(kinectData()));
   connect(&kreader,SIGNAL(statusNotification(QKinect::KinectStatus)),this,SLOT(kinectStatus(QKinect::KinectStatus)));
   connect(&kreader,SIGNAL(userNotification(unsigned,bool)),this,SLOT(kinectUser(unsigned,bool)));
   connect(&kreader,SIGNAL(poseNotification(unsigned,QString)),this,SLOT(kinectPose(unsigned,QString)));
   connect(&kreader,SIGNAL(calibrationNotification(unsigned,QKinect::CalibrationStatus)),this,SLOT(kinectCalibration(unsigned,QKinect::CalibrationStatus)));

   // Start the kinect reader. Error or success is notified by a signal
   kreader.start();
   xnFPSInit(&xnFPS, 180);

   // Attach the keyboard to one label and install a key event filter
   connect(&ke,SIGNAL(Key(int)),this,SLOT(key(int)));
   ui->labelDepth->grabKeyboard();
   ui->labelDepth->installEventFilter(&ke);
   key(0);

   // Start the writer (file, server). Close the application in case of failure
   int rv = writer.start(fname,port,numuser,&kreader,&ke);
   if(rv!=0)
   {
      QString err;
      if(rv&1)
         err+=QString("Cannot write to file '%1'").arg(fname);
      if(rv&2)
         err+=QString("Cannot open server on port %1").arg(port);
      QMessageBox::critical(this,progname,err);
      // Close app
      close();
      return;
   }

#ifdef WRITEVIDEO
   if(!fnamevideo.isEmpty())
   {
      rv = writervideo.start(fnamevideo,bitrate,vfr,vmaxbuf,&kreader);
      if(rv!=0)
      {
         QString err;
         err=QString("Cannot write video to file '%1'").arg(fnamevideo);
         QMessageBox::critical(this,progname,err);
         // Close app
         close();
         return;
      }
      sbFileVideo->setText(QString("Vid: %1").arg(fnamevideo));
   }
   else
   {
      sbFileVideo->setText("No video");
      sbFileVideoSize->setText("VS: -");
   }

#endif




   // Status bar info
   if(port==0)
      sbServer->setText("No server");
   else
      sbServer->setText(QString("Port: %1").arg(port));

   if(fname==QString(""))
      sbFile->setText("No logging");
   else
      sbFile->setText(QString("Log: %1").arg(fname));



}




/**
  \brief Called upon kinect data reception: display images, updates status bar
**/
void MainWindow::kinectData()
{
   // Mark time when receiving first data, to compute runtime.
   if(firstData)
   {
      firstData=false;
      timeFirstData = PreciseTimer::QueryTimer();
   }

   // Get the images, resize, and display
   QImage img,depth,target;
   QKinect::Bodies bodies;
   double ts;
   unsigned fid;

   kreader.getCameraDepthBodies(img,depth,bodies,ts,fid);


   target = QImage(ui->labelDepth->size(),QImage::Format_RGB32);
   QPainter painter;
   painter.begin(&target);
   painter.drawImage(QRect(QPoint(0,0),ui->labelDepth->size()),depth,QRect(QPoint(0,0),img.size()));
   painter.end();
   ui->labelDepth->setPixmap(QPixmap::fromImage(target));

   target = QImage(ui->labelImage->size(),QImage::Format_RGB32);
   painter.begin(&target);
   painter.drawImage(QRect(QPoint(0,0),ui->labelImage->size()),img,QRect(QPoint(0,0),img.size()));
   painter.end();
   ui->labelImage->setPixmap(QPixmap::fromImage(target));

   sbKinectNumBody->setText(QString("Body: %1").arg(bodies.size()));

   xnFPSMarkFrame(&xnFPS);

   sbKinectFrame->setText(QString("F#: %1").arg(fid));
   sbKinectTime->setText(QString().sprintf("TS: %.2f",ts));
   sbSystime->setText(QString().sprintf("Systime: %.2f",PreciseTimer::QueryTimer()));
   sbRuntime->setText(QString().sprintf("Runtime: %.2f",PreciseTimer::QueryTimer()-timeFirstData));
   sbKinectFPS->setText(QString().sprintf("FPS: %.2f",xnFPSCalc(&xnFPS)));

   if(writer.getNumClients()==-1)
      sbClients->setText(QString("Clients: -"));
   else
      sbClients->setText(QString("Clients: %1").arg(writer.getNumClients()));

#ifdef WRITEVIDEO
   if(!fnamevideo.isEmpty())
   {
      sbFileVideoSize->setText(QString("VS: %1 MB").arg(writervideo.getEncodedSize()/1024/1024));
      sbVideoWorkload->setText(QString().sprintf("Enc: %05d Backlog: %05d",writervideo.getEncodedFramesCount(),writervideo.getUnencodedFramesCount()));
   }
#endif


}


/**
  \brief Called upon kinect status change: display status, check errors
**/
void MainWindow::kinectStatus(QKinect::KinectStatus s)
{

   QString str("Kinect: ");
   if(s==QKinect::Idle)
      str += "Idle";
   if(s==QKinect::Initializing)
      str += "Initializing";
   if(s==QKinect::OkRun)
      str += "Running";
   if(s==QKinect::ErrorStop)
      str += "Error";
   sbKinectStatus->setText(str);

   if(s==QKinect::ErrorStop)
   {
      QMessageBox::critical(this,progname,"Cannot initialize the Kinect");
      close();
   }
}

void MainWindow::kinectPose(unsigned id,QString pose)
{
}

void MainWindow::kinectUser(unsigned id, bool found)
{
}

void MainWindow::kinectCalibration(unsigned id, QKinect::CalibrationStatus s)
{
}

void MainWindow::key(int k)
{
   sbLabel->setText(QString("L: %1").arg(k));
}

void MainWindow::help()
{
   ui->labelDepth->releaseKeyboard();
   // Load the text from the resource
   QFile file(":/help.html");
   file.open(QIODevice::ReadOnly | QIODevice::Text);
   QByteArray filedata = file.readAll();

   HelpDialog *dlg = new HelpDialog(QString(filedata),this);
   dlg->exec();
   delete dlg;

   ui->labelDepth->grabKeyboard();
}

void MainWindow::about()
{
   ui->labelDepth->releaseKeyboard();
   QMessageBox::about(this, "About",
   "<p><b>KinectLogger</b></p>\n"
   "<p>Version 17.03.2012</p>"
   "<p>(c) 2011-2012 Daniel Roggen</p>");
   ui->labelDepth->grabKeyboard();
}

/**
   \brief Terminates the readers and writers
**/
void MainWindow::stop()
{
   ui->labelDepth->removeEventFilter(&ke);
   ui->labelDepth->releaseKeyboard();

#ifdef WRITEVIDEO
   writervideo.stop();
#endif

   writer.stop();

   kreader.stop();




}

void MainWindow::closeEvent( QCloseEvent * event)
{
   stop();
}
