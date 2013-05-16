#-------------------------------------------------
#
# Project created by QtCreator 2011-09-04T10:49:16
#
#-------------------------------------------------

# Set this to include the video generation capability. Requires QtFFmpegWrapper
DEFINES += WRITEVIDEO

QT       += core gui network

#CONFIG += console

TARGET = KinectLogger
TEMPLATE = app



SOURCES += main.cpp\
        mainwindow.cpp \
    cmdline/cmdline.cpp \
    QStreamSrv.cpp \
    writer.cpp \
    precisetimer.cpp \
    keyfilter.cpp \
    helpdialog.cpp \
    cio.cpp \
    videoencoderthreaded.cpp



HEADERS  += mainwindow.h \
    cmdline/cmdline.h \
    QStreamSrv.h \
    writer.h \
    precisetimer.h \
    keyfilter.h \
    helpdialog.h \
    cio.h \
    videoencoderthreaded.h


FORMS    += mainwindow.ui \
    helpdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    help.html \
    kinectlogger.rc

# icon
win32: RC_FILE = kinectlogger.rc

#
contains(DEFINES, WRITEVIDEO)  {
   message(Adding files for video generation)
   SOURCES += writer_vid.cpp
   HEADERS += writer_vid.h
}

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# ##############################################################################
# ##############################################################################
# Kinect: START OF CONFIGURATION BELOW ->
#
# Copy these lines at the end of your the project file of your application (.pro )
# Check the lines indicated "Modify below" and adapt the path to:
# 1) QtKinectWrapper sources (i.e. where the QKinectWrapper.* files lie),
# 2) OpenNI path
# 3) Make sure the OpenNI, PrimeSense NITE, PrimeSense Kinect Sensor are all in your path
#
# -------------------------------- Modify below --------------------------------

# Set QTKINECTWRAPPER_SOURCE_PATH to point to the directory containing the QtKinectWrapper classes
QTKINECTWRAPPER_SOURCE_PATH = ../../QtKinectWrapper

# Set OPENNI_LIBRARY_PATH to point to the directory where openNI is installed. Assumption: the import libraries are in $$OPENNI_LIBRARY_PATH/lib
# Multiple path can be specified (e.g. to handle x86 / x64 differences)
# Quotation marks with spaces
win32 {
OPENNI_LIBRARY_PATH = "C:/Program Files (x86)/OpenNI"
OPENNI_LIBRARY_PATH += "C:/Program Files/OpenNI"
}

# ------------------------------ Copy as-is below ------------------------------
# ##############################################################################
# ##############################################################################
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# ##############################################################################
# Do not modify: default settings
# ##############################################################################
# Path for Qt wrapper
INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH
# Sources for Qt wrapper
SOURCES += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.cpp
HEADERS += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.h



win32 {
   # Set the path to the patched openni/include
   INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH/OpenNI/Include
   # Set the path to the Qt wrapper
   INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH

   # Iterate through
   for(l, OPENNI_LIBRARY_PATH):LIBS+=-L$$l/lib
   #LIBS += -L$$OPENNI_LIBRARY_PATH/lib
   LIBS += -lopenNI
}

# ##############################################################################
# ##############################################################################
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



contains(DEFINES, WRITEVIDEO)  {
   message(Adding ffmpeg config)
# ##############################################################################
# ##############################################################################
# FFMPEG: START OF CONFIGURATION BELOW ->
# Copy these lines into your own project
# Make sure to set the path variables for:
# 1) QTFFmpegWrapper sources (i.e. where the QVideoEncoder.cpp and QVideoDecoder.cpp lie),
# 2) FFMPEG include path (i.e. where the directories libavcodec, libavutil, etc. lie),
# 3) the binary FFMPEG libraries (that must be compiled separately).
# Under Linux path 2 and 3 may not need to be set as these are usually in the standard include and lib path.
# Under Windows, path 2 and 3 must be set to the location where you placed the FFMPEG includes and compiled binaries
# Note that the FFMPEG dynamic librairies (i.e. the .dll files) must be in the PATH
# ##############################################################################
# ##############################################################################

# ##############################################################################
# Modify here: set FFMPEG_LIBRARY_PATH and FFMPEG_INCLUDE_PATH
# ##############################################################################

# Set QTFFMPEGWRAPPER_SOURCE_PATH to point to the directory containing the QTFFmpegWrapper sources
QTFFMPEGWRAPPER_SOURCE_PATH = ../../../qtffmpegwrapper_trunk/QTFFmpegWrapper

# Set FFMPEG_LIBRARY_PATH to point to the directory containing the FFmpeg import libraries (if needed - typically for Windows), i.e. the dll.a files
FFMPEG_LIBRARY_PATH = ../../../qtffmpegwrapper_trunk/ffmpeg_lib_win32

# Set FFMPEG_INCLUDE_PATH to point to the directory containing the FFMPEG includes (if needed - typically for Windows)
FFMPEG_INCLUDE_PATH = ../../../qtffmpegwrapper_trunk/QTFFmpegWrapper

# ##############################################################################
# Do not modify: FFMPEG default settings
# ##############################################################################
# Sources for QT wrapper
SOURCES += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp
HEADERS += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

# Add the path
LIBS += -L$$FFMPEG_LIBRARY_PATH
INCLUDEPATH += QVideoEncoder
INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################


}
