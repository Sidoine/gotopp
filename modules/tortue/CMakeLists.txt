FIND_LIBRARY(X11_LIB_PATH NAMES X11 X11.dll PATHS /usr/X11R6/lib 
	/usr/local/lib 
	/usr/openwin/lib 
	/usr/lib)
FIND_PATH(X11_INCLUDE_PATH X11/Xlib.h /usr/X11R6/include 
    /usr/local/include 
    /usr/include/X11
    /usr/openwin/include 
    /usr/openwin/share/include 
    /opt/graphics/OpenGL/include
    /usr/include
  )

IF(X11_LIB_PATH)
    IF(X11_INCLUDE_PATH)
	MESSAGE(STATUS "X11 found\n")
      SET(X11_FOUND 1)
   ENDIF(X11_INCLUDE_PATH)
ENDIF(X11_LIB_PATH)

IF (X11_FOUND)
	ADD_LIBRARY(tortue SHARED tortue.cpp)
	TARGET_LINK_LIBRARIES(tortue ${X11_LIB_PATH})
	INCLUDE_DIRECTORIES(${X11_INCLUDE_PATH})
	INSTALL_TARGETS(/lib/gpp/ RUNTIME_DIRECTORY /lib/gpp tortue tortue)
ELSE(X11_FOUND)
	IF(WIN32)
		ADD_LIBRARY(tortue SHARED tortue.cpp)
		INSTALL_TARGETS(/lib/gpp/ RUNTIME_DIRECTORY /lib/gpp tortue tortue)	
	ENDIF(WIN32)
ENDIF(X11_FOUND)