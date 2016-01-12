#Make a library with all the project sources
#This is useful for building the tests
add_library(TLG
	Action.cpp
	actions/ArrowPicker.cpp
	actions/ArrowShooter.cpp
	actions/BowBender.cpp
	actions/CorpseSearcher.cpp
	actions/Death.cpp
	actions/HandToHand.cpp
	actions/InventoryOpener.cpp
	actions/Jumper.cpp
	actions/Mover.cpp
	Application.cpp
	BloomEffect.cpp
	Command.cpp
	ContactListener.cpp
	contactListeners/ArrowHitListener.cpp
	contactListeners/FallingListener.cpp
	Context.cpp
	DebugDraw.cpp
	DestructionListener.cpp
	functions.cpp
	HashEntity.cpp
	LangManager.cpp
	MusicPlayer.cpp
	Parameters.cpp
	Player.cpp
	PostEffect.cpp
	scripts/Interpreter.cpp
	scripts/scriptsFunctions.cpp
	scripts/Variant.cpp
	serialization.cpp
	SkyAnimation.cpp
	SpriteSheetAnimation.cpp
	State.cpp
	states/GameState.cpp
	states/HUDState.cpp
	states/IntroState.cpp
	states/KeyConfigurationState.cpp
	states/LoadingState.cpp
	states/MainMenuState.cpp
	states/OpenInventoryState.cpp
	states/ParametersState.cpp
	states/PauseState.cpp
	StateStack.cpp
	systems/AnimationsSystem.cpp
	systems/DragAndDropSystem.cpp
	systems/PendingChangesSystem.cpp
	systems/PhysicsSystem.cpp
	systems/RenderSystem.cpp
	systems/ScriptsSystem.cpp
	systems/ScrollingSystem.cpp
	systems/StatsSystem.cpp
	systems/TimeSystem.cpp
	${TheLostGirl_SOURCE_DIR}/include/dist/jsoncpp.cpp
	)
#Create the main executable
add_executable(TheLostGirl main.cpp)

#Link external librairies
target_link_libraries(TheLostGirl TLG ${TLG_EXTERNAL_LIBRARIES})
