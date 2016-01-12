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
	LangManagerTests.cpp
	main.cpp
	MusicPlayerTests.cpp
	PlayerTests.cpp
	ResourceManagerTests.cpp
	scripts/InterpreterTests.cpp
	scripts/ScriptErrorTests.cpp
	scripts/scriptsFunctionsTests.cpp
	scripts/TreeTests.cpp
	serializationTests.cpp
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
	)

#Create the main test executable
add_executable(TheLostGirlTests ${TLG_TEST_SOURCES})

#Link the executable with the game binaries and other libraries
target_link_libraries(TheLostGirlTests
	TLG
	${SFML_LIBRARIES}
	${TGUI_LIBRARY}
	${BOX2D_LIBRARY}
	${ENTITYX_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${X11_LIBRARIES}
	${Boost_LIBRARIES}
	)
