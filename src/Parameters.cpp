#include <TheLostGirl/Parameters.h>

Parameters::Parameters():
	debugMode{false},
	imagePath{"resources/images/"},
	pixelByMeter{50.f},
	gravity{0.0f, 9.80665f},
	bloomEnabled{false},
	fullscreen{true},
	guiConfigFile{"resources/gui.conf"}
{
}
