find_package(Doxygen REQUIRED)

#Set the input and output documentation paths
set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}")
set(DOXYGEN_OUTPUT_DIR "${PROJECT_BINARY_DIR}/doxygen")
set(DOXYGEN_CONFIGURED_INPUT "${DOXYGEN_OUTPUT_DIR}/Doxyfile")

#Configure the source Doxyfile by copying it and replacing all @variables@
configure_file(${DOXYGEN_INPUT_DIR}/doxygen/Doxyfile.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

#Target setup
add_custom_target(doc ALL
	COMMAND ${CMAKE_COMMAND} -E echo_append "Building documentation..."
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
	COMMAND ${CMAKE_COMMAND} -E echo "Done."
	WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR})
