cmake_minimum_required(VERSION 2.8)

SET(LIBTK_SRC TK_DebugServer.cpp TK_sock.cpp TK_Tools.cpp)
#SET(LIBRARY_OUTPUT_PATH new_path)
ADD_LIBRARY(tk SHARED ${LIBTK_SRC})
ADD_LIBRARY(tk_static STATIC ${LIBTK_SRC}) 
SET_TARGET_PROPERTIES(tk_static PROPERTIES OUTPUT_NAME "tk")
#GET_TARGET_PROPERTY(OUTPUT_VALUE tk_static OUTPUT_NAME)
#MESSAGE(STATUS �This is the tk_static OUTPUT_NAME:�${OUTPUT_VALUE})
SET_TARGET_PROPERTIES(tk PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(tk_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(tk PROPERTIES VERSION 1.0 SOVERSION 0)
INSTALL(TARGETS tk tk_static
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)
INSTALL(FILES *.h DESTINATION include/tk)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
