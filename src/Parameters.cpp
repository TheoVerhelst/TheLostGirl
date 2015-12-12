#include <TheLostGirl/Parameters.h>

Parameters::Parameters():
	debugMode{false},
	scaleIndex{0},
	scale{scales[scaleIndex]},
	pixelByMeter{50.f},
	scaledPixelByMeter{scale*pixelByMeter},
	gravity{0.0f, 9.80665f},
	bloomEnabled{false},
	fullscreen{true},
	guiConfigFile{"resources/gui.conf"}
{
}
