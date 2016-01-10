cmake_minimum_required(VERSION 3.2)
project(TheLostGirl)

#Set path to Find scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#Set compilator flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14")

#Dependencies finding
find_package(Boost 1.57 COMPONENTS system filesystem REQUIRED)
find_package(SFML 2.3 COMPONENTS system window graphics audio REQUIRED)
find_package(TGUI 0.7 REQUIRED)
find_package(Box2D REQUIRED)
find_package(EntityX REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

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

#Set the path to the src directory
set(TLG_SOURCES_DIR src)
#List of all files of the project
set(TLG_SOURCES
	${TLG_SOURCES_DIR}/Action.cpp
	${TLG_SOURCES_DIR}/actions/ArrowPicker.cpp
	${TLG_SOURCES_DIR}/actions/ArrowShooter.cpp
	${TLG_SOURCES_DIR}/actions/BowBender.cpp
	${TLG_SOURCES_DIR}/actions/CorpseSearcher.cpp
	${TLG_SOURCES_DIR}/actions/Death.cpp
	${TLG_SOURCES_DIR}/actions/HandToHand.cpp
	${TLG_SOURCES_DIR}/actions/InventoryOpener.cpp
	${TLG_SOURCES_DIR}/actions/Jumper.cpp
	${TLG_SOURCES_DIR}/actions/Mover.cpp
	${TLG_SOURCES_DIR}/Application.cpp
	${TLG_SOURCES_DIR}/BloomEffect.cpp
	${TLG_SOURCES_DIR}/Command.cpp
	${TLG_SOURCES_DIR}/ContactListener.cpp
	${TLG_SOURCES_DIR}/contactListeners/ArrowHitListener.cpp
	${TLG_SOURCES_DIR}/contactListeners/FallingListener.cpp
	${TLG_SOURCES_DIR}/Context.cpp
	${TLG_SOURCES_DIR}/DebugDraw.cpp
	${TLG_SOURCES_DIR}/DestructionListener.cpp
	${TLG_SOURCES_DIR}/functions.cpp
	${TLG_SOURCES_DIR}/HashEntity.cpp
	${TLG_SOURCES_DIR}/LangManager.cpp
	${TLG_SOURCES_DIR}/main.cpp
	${TLG_SOURCES_DIR}/MusicPlayer.cpp
	${TLG_SOURCES_DIR}/Parameters.cpp
	${TLG_SOURCES_DIR}/Player.cpp
	${TLG_SOURCES_DIR}/PostEffect.cpp
	${TLG_SOURCES_DIR}/scripts/Interpreter.cpp
	${TLG_SOURCES_DIR}/scripts/scriptsFunctions.cpp
	${TLG_SOURCES_DIR}/scripts/Variant.cpp
	${TLG_SOURCES_DIR}/serialization.cpp
	${TLG_SOURCES_DIR}/SkyAnimation.cpp
	${TLG_SOURCES_DIR}/SpriteSheetAnimation.cpp
	${TLG_SOURCES_DIR}/State.cpp
	${TLG_SOURCES_DIR}/states/GameState.cpp
	${TLG_SOURCES_DIR}/states/HUDState.cpp
	${TLG_SOURCES_DIR}/states/IntroState.cpp
	${TLG_SOURCES_DIR}/states/KeyConfigurationState.cpp
	${TLG_SOURCES_DIR}/states/LoadingState.cpp
	${TLG_SOURCES_DIR}/states/MainMenuState.cpp
	${TLG_SOURCES_DIR}/states/OpenInventoryState.cpp
	${TLG_SOURCES_DIR}/states/ParametersState.cpp
	${TLG_SOURCES_DIR}/states/PauseState.cpp
	${TLG_SOURCES_DIR}/StateStack.cpp
	${TLG_SOURCES_DIR}/systems/AnimationsSystem.cpp
	${TLG_SOURCES_DIR}/systems/DragAndDropSystem.cpp
	${TLG_SOURCES_DIR}/systems/PendingChangesSystem.cpp
	${TLG_SOURCES_DIR}/systems/PhysicsSystem.cpp
	${TLG_SOURCES_DIR}/systems/RenderSystem.cpp
	${TLG_SOURCES_DIR}/systems/ScriptsSystem.cpp
	${TLG_SOURCES_DIR}/systems/ScrollingSystem.cpp
	${TLG_SOURCES_DIR}/systems/StatsSystem.cpp
	${TLG_SOURCES_DIR}/systems/TimeSystem.cpp
	include/dist/jsoncpp.cpp
	)

#Create the main executable
add_executable(TheLostGirl ${TLG_SOURCES})

#Link external librairies
target_link_libraries(TheLostGirl
	${SFML_LIBRARIES}
	${TGUI_LIBRARY}
	${BOX2D_LIBRARY}
	${ENTITYX_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${X11_LIBRARIES}
	${Boost_LIBRARIES}
	)

