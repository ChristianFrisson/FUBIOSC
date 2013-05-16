SET(TARGET_NAME "breakpad-qt-test")
IF(USE_BREAKPAD AND USE_QT4 )

	IF(APPLE)
		SET(PLATFORM "mac")
	ELSEIF(UNIX)
		SET(PLATFORM "linux")
	ELSE()
		SET(PLATFORM "windows")
	ENDIF()
	#MESSAGE("PLATFORM ${PLATFORM}")

        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/handler ${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/sender)

        SET(${TARGET_NAME}_PROGRAM_SRCS ${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/demo/program/main.cpp ${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/demo/program/TestThread.cpp)
        SET(${TARGET_NAME}_PROGRAM_HDRS ${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/demo/program/TestThread.h)

        SET(${TARGET_NAME}_REPORTER_SRCS ${CMAKE_SOURCE_DIR}/3rdparty/breakpad-qt/demo/reporter/main.cpp)

	# Generates ui_*.h files 
	#QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
	# Generates moc_*.cxx files
	QT4_WRAP_CPP(${TARGET_NAME}_PROGRAM_MOC_SRCS ${${TARGET_NAME}_PROGRAM_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	
	QT4_WRAP_CPP(${TARGET_NAME}_REPORTER_MOC_SRCS ${${TARGET_NAME}_REPORTER_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed	
	
	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 OR MINGW)
		SET(OS_SPECIFIC "WIN32")
	ELSE ()
		IF ( APPLE )
			SET(OS_SPECIFIC MACOSX_BUNDLE)
		ENDIF()
	ENDIF()

	ADD_EXECUTABLE(${TARGET_NAME}-program ${OS_SPECIFIC} ${${TARGET_NAME}_PROGRAM_SRCS} ${${TARGET_NAME}_PROGRAM_HDRS} ${${TARGET_NAME}_PROGRAM_UIS_H} ${${TARGET_NAME}_PROGRAM_MOC_SRCS} ${${TARGET_NAME}_PROGRAM_RCC_SRCS})
	ADD_EXECUTABLE(${TARGET_NAME}-reporter ${OS_SPECIFIC} ${${TARGET_NAME}_REPORTER_SRCS} ${${TARGET_NAME}_REPORTER_HDRS} ${${TARGET_NAME}_REPORTER_UIS_H} ${${TARGET_NAME}_REPORTER_MOC_SRCS} ${${TARGET_NAME}_REPORTER_RCC_SRCS})

	ADD_DEPENDENCIES(${TARGET_NAME}-program breakpad-qt)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}-program breakpad-qt)
	
	ADD_DEPENDENCIES(${TARGET_NAME}-reporter breakpad-qt)
	TARGET_LINK_LIBRARIES(${TARGET_NAME}-reporter breakpad-qt)

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-lcrypto -framework CoreFoundation")
	ENDIF ( APPLE )
    SET(FOLDERNAME "tests")

	SET_TARGET_PROPERTIES(${TARGET_NAME}-program breakpad-qt PROPERTIES FOLDER "${FOLDERNAME}")
	SET_TARGET_PROPERTIES(${TARGET_NAME}-reporter breakpad-qt PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
