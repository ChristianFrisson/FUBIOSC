MESSAGE("\n3rdparty:")

FILE(GLOB DIRLIST * )

SET(FOLDERNAME "3rdparty")
FOREACH(3RDPARTYDIR ${DIRLIST})
    IF(IS_DIRECTORY ${3RDPARTYDIR})
        ADD_SUBDIRECTORY(${3RDPARTYDIR})
    ENDIF()
ENDFOREACH()
