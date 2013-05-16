SET(TARGET_NAME "qtffmpegwrapper")
IF(USE_QT4 AND USE_FFMPEG AND FFMPEG_FOUND)

	INCLUDE_DIRECTORIES(QTFFmpegWrapper)

	FILE(GLOB ${TARGET_NAME}_SRCS QTFFmpegWrapper/*.cpp)
	FILE(GLOB ${TARGET_NAME}_HDRS QTFFmpegWrapper/*.h)

	# Generates ui_*.h files 
	#QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
	# Generates moc_*.cxx files
	QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS} ${${TARGET_NAME}_RCC_SRCS})
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${FFMPEG_LIBRARIES})
	
	#IF ( APPLE )
	#	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${COREFOUNDATION_LIBRARY})
	#ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
