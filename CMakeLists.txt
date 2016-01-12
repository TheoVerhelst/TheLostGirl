cmake_minimum_required(VERSION 3.2)
project(TheLostGirl)

# Define a macro that helps defining an option
macro(tlg_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

#Set path to Find scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#Dependencies finding
find_package(Boost 1.57 COMPONENTS system filesystem unit_test_framework REQUIRED)
find_package(SFML 2.3 COMPONENTS system window graphics audio REQUIRED)
find_package(TGUI 0.7 REQUIRED)
find_package(Box2D REQUIRED)
find_package(EntityX REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

#Set compilator flags
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra -pedantic -pedantic-errors -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Winline -Wunreachable-code -Wmissing-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG")

#Set the include directory of the project
include_directories(
	${TheLostGirl_SOURCE_DIR}/include
	${SFML_INCLUDE_DIR}
	${TGUI_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	${BOX2D_INCLUDE_DIR}
	${ENTITYX_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	)

#Add an option to build the tests
tlg_set_option(TLG_BUILD_TESTS FALSE BOOL "TRUE to build the tests")

#Add an option to profile the code
tlg_set_option(TLG_PROFILE FALSE BOOL "TRUE to enable profiling")

add_subdirectory(src)

#Build the tests if requested
if(TLG_BUILD_TESTS)
	add_subdirectory(tests)
	enable_testing()
	add_test(NAME Tests COMMAND TheLostGirlTests)
endif()

if(TLG_PROFILE)
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        message(WARNING "TLG_PROfilE should only be enabled when CMAKE_BUILD_TYPE is Release")
    endif()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pg")
endif()
