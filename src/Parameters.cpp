#include <TheLostGirl/Parameters.h>

Parameters::Parameters():
	debugMode{false},
	resourcesPath{"resources/"},
	pixelByMeter{50.f},
	gravity{0.0f, 9.80665f},
	bloomEnabled{false},
	fullscreen{true},
	guiTheme{std::make_shared<tgui::Theme>(resourcesPath + "gui.conf")}
{
}
