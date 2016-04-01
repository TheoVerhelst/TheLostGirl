# Look for a version of EntityX on the local machine
#
# By default, this will look in all common places. If EntityX is built or
# installed in a custom location, you're able to either modify the
# CMakeCache.txt file yourself or simply pass the path to CMake using either the
# environment variable `ENTITYX_ROOT` or the CMake define with the same name.

set(ENTITYX_PATHS
    ${ENTITYX_ROOT}
	$ENV{ENTITYX_ROOT}
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

find_path(ENTITYX_INCLUDE_DIR entityx/entityx.h PATH_SUFFIXES include PATHS ${ENTITYX_PATHS})
find_library(ENTITYX_LIBRARY_RELEASE NAMES entityx PATH_SUFFIXES lib PATHS ${ENTITYX_PATHS})
find_library(ENTITYX_LIBRARY_DEBUG NAMES entityx-d PATH_SUFFIXES lib PATHS ${ENTITYX_PATHS})

if(ENTITYX_LIBRARY_DEBUG OR ENTITYX_LIBRARY_RELEASE)
    # library found
    set(ENTITYX_FOUND TRUE)

    # if both are found, set ENTITYX_LIBRARY to contain both
    if(ENTITYX_LIBRARY_DEBUG AND ENTITYX_LIBRARY_RELEASE)
        set(ENTITYX_LIBRARY debug ${ENTITYX_LIBRARY_DEBUG} optimized ${ENTITYX_LIBRARY_RELEASE})
    endif()

    # if only one debug/release variant is found, set the other to be equal to the found one
    if(ENTITYX_LIBRARY_DEBUG AND NOT ENTITYX_LIBRARY_RELEASE)
        # debug and not release
        set(ENTITYX_LIBRARY_RELEASE ${ENTITYX_LIBRARY_DEBUG})
        set(ENTITYX_LIBRARY ${ENTITYX_LIBRARY_DEBUG})
    endif()
    if(ENTITYX_LIBRARY_RELEASE AND NOT ENTITYX_LIBRARY_DEBUG)
        # release and not debug
        set(ENTITYX_LIBRARY_DEBUG ${ENTITYX_LIBRARY_RELEASE})
        set(ENTITYX_LIBRARY ${ENTITYX_LIBRARY_RELEASE})
    endif()
else()
    # library not found
    set(ENTITYX_FOUND FALSE)
    set(ENTITYX_LIBRARY "")
endif()

set(ENTITYX_LIBRARIES ${ENTITYX_LIBRARY})
set(ENTITYX_INCLUDE_DIRS ${ENTITYX_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set ENTITYX_FOUND to TRUE if
# all listed variables are TRUE
find_package_handle_standard_args(entityx DEFAULT_MSG ENTITYX_LIBRARY ENTITYX_INCLUDE_DIR)

mark_as_advanced(ENTITYX_INCLUDE_DIR
		ENTITYX_LIBRARY
		ENTITYX_LIBRARY_DEBUG
		ENTITYX_LIBRARY_RELEASE)
