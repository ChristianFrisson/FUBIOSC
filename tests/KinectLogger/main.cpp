/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <QtGui/QApplication>
#include <QMessageBox>
#include <QFileInfo>
#include "mainwindow.h"

#include "cmdline.h"


#define BITRATEMIN       100000
#define BITRATEMAX     20000000
#define BITRATEDEFAULT  2000000

void Syntax(QString progname,QString errmsg="");

void Syntax(QString progname,QString errmsg)
{

   QString str = QString(
"<html><pre>"
"\n\n"
"%1"
"\n\n"
"Usage:\n\n"
#ifdef WRITEVIDEO
"      %2  [-?|-h]  [-pi]  [-pd]  [-n &lt;numuser&gt;]  [-s &lt;port&gt;]  [-vfr] [-vmaxbuf &lt;maxbuf&gt;]\n"
"      [-v &lt;video&gt;] [-b &lt;bitrate&gt;] [&lt;file&gt;]\n\n"
#else
"      %2  [-?|-h]  [-pi]  [-pd]  [-n &lt;numuser&gt;]  [-s &lt;port&gt;]  [&lt;file&gt;]\n\n"
#endif
"   ?|h        display this help\n"
"   pi         plain image: do not overlay the skeleton on the image\n"
"   pd         plain depth map: do not overlay the skeleton on the depth map\n"
"   &lt;numuser&gt;  maximum number of users to store/serve (\"NaN\" are used for padding when less users than &lt;numuser&gt; are seen)\n"
"   &lt;port&gt;     starts a streaming server on &lt;port&gt;\n"
"   &lt;file&gt;     store data in &lt;file&gt;\n"
#ifdef WRITEVIDEO
"   vfr        variable frame rate video encoding\n"
"   maxbuf     maximum frames in the video encoding buffer\n"
"   &lt;video&gt;    store video in &lt;video&gt;\n"
"   &lt;bitrate&gt;  video &lt;bitrate&gt; ranging from %3 to %4 (default %5)\n"
#endif
"\n"
"If neither a port nor a file is specified, the program displays the Kinect data without logging.\n"
"It is advisable to use -vfr to ensure that the video frames are properly timestamped even when frames are dropped.</pre></html>\n").arg(errmsg).arg(progname).arg(BITRATEMIN).arg(BITRATEMAX).arg(BITRATEDEFAULT);

   QMessageBox::information(0,progname,str);

}

int main(int argc, char *argv[])
{
   unsigned numtrack,bitrate;
   unsigned help1,help2,plainimage,plaindepth;
   char s_num[256],s_file[256],s_port[256],s_filevid[256],s_bitrate[256],s_vmaxbuf[256];
   int num,port;
   int vfr,vmaxbuf;


   QFileInfo info(argv[0]);
   QString progname = info.baseName();



   QApplication a(argc, argv);

   // Scan command line
#ifdef WRITEVIDEO
   int rv = ScanCommandLine("-n@ -s@ @ -h -? -pi -pd -vfr -vmaxbuf@ -v@ -b@",argc,argv,s_num,s_port,s_file,&help1,&help2,&plainimage,&plaindepth,&vfr,s_vmaxbuf,s_filevid,s_bitrate);
#else
   int rv = ScanCommandLine("-n@ -s@ @ -h -? -pi -pd",argc,argv,s_num,s_port,s_file,&help1,&help2,&plainimage,&plaindepth);
#endif

   // Check for error or help request
   if(rv || help1 || help2)
   {
      Syntax(progname,"Program syntax");
      return 0;
   }

   // Check optional number
   if(strlen(s_num))
   {
      // Convert
      rv = sscanf(s_num,"%d",&num);
      if(rv!=1 || num<1)
      {
         Syntax(progname,"Invalid number of users");
         return 0;
      }
   }
   else
      num=1;

   // Check server port number
   if(strlen(s_port))
   {
      // Convert
      rv = sscanf(s_port,"%d",&port);
      if(rv!=1 || port<1)
      {
         Syntax(progname,"Invalid port number");
         return 0;
      }
   }
   else
      port=0;

   // Check optional bitrate
   if(strlen(s_bitrate))
   {
      // Convert
      rv = sscanf(s_bitrate,"%u",&bitrate);
      if(rv!=1 || bitrate<BITRATEMIN || bitrate>BITRATEMAX)
      {
         Syntax(progname,"Invalid bitrate");
         return 0;
      }
   }
   else
      bitrate=4000000;

   // Check optional maxbuf
   if(strlen(s_vmaxbuf))
   {
      // Convert
      rv = sscanf(s_vmaxbuf,"%u",&vmaxbuf);
      if(rv!=1)
      {
         Syntax(progname,"Invalid maximum video encoding buffer size");
         return 0;
      }
   }
   else
      vmaxbuf=-1;     // unlimited

   
   MainWindow w(progname,plainimage,plaindepth,QString(s_file),QString(s_filevid),bitrate,num,port,vfr,vmaxbuf);
   //CF
    //MainWindow w(progname,plainimage,plaindepth,QString(s_file),QString("test"),bitrate,num,port,vfr,vmaxbuf);
   w.show();
   rv = a.exec();

   return rv;
}
