find_package(Doxygen REQUIRED)

#Copy the doxygen directory to the build directory
file(COPY ${TheLostGirl_SOURCE_DIR}/doxygen DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#Set the input and output documentation paths
set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}")
set(DOXYGEN_OUTPUT_DIR "${PROJECT_BINARY_DIR}/doxygen")
set(DOXYGEN_CONFIGURED_INPUT "${DOXYGEN_OUTPUT_DIR}/Doxyfile")

#Target setup
add_custom_target(doc ALL
	COMMAND ${CMAKE_COMMAND} -E echo_append "Building documentation..."
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
	COMMAND ${CMAKE_COMMAND} -E echo "Done."
	WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR})
