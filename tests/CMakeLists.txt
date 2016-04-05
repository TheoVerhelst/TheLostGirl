#Find the unit test framework library
find_package(Boost 1.57 COMPONENTS unit_test_framework REQUIRED)

#Add the library name to the project linking dependencies
set(TLG_EXTERNAL_LIBRARIES ${TLG_EXTERNAL_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

#The define neede by the Boost unit test framework
add_definitions(-DBOOST_TEST_DYN_LINK)

#All test source files
set(TLG_TEST_SOURCES
	actions/ArrowPickerTests.cpp
	actions/ArrowShooterTests.cpp
	actions/BowBenderTests.cpp
	actions/CorpseSearcherTests.cpp
	actions/DeathTests.cpp
	actions/HandToHandTests.cpp
	actions/InventoryOpenerTests.cpp
	actions/JumperTests.cpp
	actions/MoverTests.cpp
	ActionTests.cpp
	AnimationsManagerTests.cpp
	CommandTests.cpp
	contactListeners/ArrowHitListenerTests.cpp
	contactListeners/FallingListenerTests.cpp
	ContactListenerTests.cpp
	functionsTests.cpp
	HashEntityTests.cpp
	JsonHelperTests.cpp
	LangManagerTests.cpp
	main.cpp
	MusicPlayerTests.cpp
	PlayerTests.cpp
	ResourceManagerTests.cpp
	scripts/InterpreterTests.cpp
	scripts/ScriptErrorTests.cpp
	scripts/scriptsFunctionsTests.cpp
	scripts/TreeTests.cpp
	SerializerTests.cpp
	SkyAnimationTests.cpp
	SpriteSheetAnimationTests.cpp
	states/GameStateTests.cpp
	states/HUDStateTests.cpp
	states/IntroStateTests.cpp
	states/KeyConfigurationStateTests.cpp
	states/LoadingStateTests.cpp
	states/MainMenuStateTests.cpp
	states/OpenInventoryStateTests.cpp
	states/ParametersStateTests.cpp
	states/PauseStateTests.cpp
	StateStackTests.cpp
	StateTests.cpp
	systems/AnimationsSystemTests.cpp
	systems/DragAndDropSystemTests.cpp
	systems/PendingChangesSystemTests.cpp
	systems/PhysicsSystemTests.cpp
	systems/RenderSystemTests.cpp
	systems/ScriptsSystemTests.cpp
	systems/ScrollingSystemTests.cpp
	systems/StatsSystemTests.cpp
	systems/TimeSystemTests.cpp
	TestsFixture.cpp
	)

#Create the main test executable
add_executable(TheLostGirlTests ${TLG_TEST_SOURCES})

#Link the executable with the game binaries and other libraries
target_link_libraries(TheLostGirlTests TLG ${TLG_EXTERNAL_LIBRARIES})

#Copy the test resources directory next to the executable
file(COPY ${TheLostGirl_SOURCE_DIR}/testsResources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#Enable ctest
enable_testing()
add_test(NAME Tests COMMAND TheLostGirlTests)
