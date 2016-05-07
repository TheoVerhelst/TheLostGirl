cmake_minimum_required(VERSION 2.8)
project(TheLostGirl)

#Define a macro that helps defining an option
macro(tlg_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

#Add an option to build the tests
tlg_set_option(TLG_BUILD_TESTS FALSE BOOL "TRUE to build the tests")

#Add an option to profile the code
tlg_set_option(TLG_PROFILE FALSE BOOL "TRUE to enable profiling")

#Add an option to enable coverage report
tlg_set_option(TLG_COVERAGE FALSE BOOL "TRUE to enable coverage report")

#Add an option to build the documentation
tlg_set_option(TLG_BUILD_DOC FALSE BOOL "TRUE to build the documentation")

#Set path to Find scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#Dependencies finding
find_package(SFML 2.3 COMPONENTS system window graphics audio REQUIRED)
find_package(TGUI 0.7 REQUIRED)
find_package(Box2D REQUIRED)
find_package(EntityX REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

#Set the include directory of the project
include_directories(${TheLostGirl_SOURCE_DIR}/include)
include_directories(SYSTEM
	${SFML_INCLUDE_DIR}
	${TGUI_INCLUDE_DIR}
	${BOX2D_INCLUDE_DIR}
	${ENTITYX_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	)

#Set compilator flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra -pedantic -pedantic-errors -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Winline -Wunreachable-code -Wmissing-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG")
if(TLG_COVERAGE)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -coverage")
endif()

#Set a variable with all needed libraries
set(TLG_EXTERNAL_LIBRARIES
	"stdc++fs"
	${SFML_LIBRARIES}
	${TGUI_LIBRARY}
	${BOX2D_LIBRARY}
	${ENTITYX_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${X11_LIBRARIES}
	)

#Place the executable files in the root directory instead of in src
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)

#Add profile flag to compiler if requested
if(TLG_PROFILE)
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        message(WARNING "TLG_PROFILE should only be enabled when CMAKE_BUILD_TYPE is Release")
    endif()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pg")
endif()

if(TLG_BUILD_DOC)
	add_subdirectory(doxygen)
endif()

#Copy the resources directory next to the executable
file(COPY ${TheLostGirl_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#Create the saves directory next to the executable (subject to change)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/saves)
