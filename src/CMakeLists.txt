add_subdirectory(dist)

#Build the tests if requested
if(TLG_BUILD_TESTS)
	add_subdirectory(tests)
endif()

add_subdirectory(TheLostGirl)
