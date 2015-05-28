#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/states/KeyConfigurationState.h>

KeyConfigurationState::KeyConfigurationState(StateStack& stack) :
	State(stack),
	m_actionStrings{{Player::Action::Bend, getContext().langManager.tr("Bend")},
			{Player::Action::ChangeArrow, getContext().langManager.tr("Change arrow")},
			{Player::Action::Concentrate, getContext().langManager.tr("Concentrate")},
			{Player::Action::FurtherView, getContext().langManager.tr("Further view")},
			{Player::Action::GenericAction, getContext().langManager.tr("Action")},
			{Player::Action::HandToHand, getContext().langManager.tr("Hand to hand")},
			{Player::Action::Inventory, getContext().langManager.tr("Inventory")},
			{Player::Action::Jump, getContext().langManager.tr("Jump")},
			{Player::Action::MoveDown, getContext().langManager.tr("Move down")},
			{Player::Action::MoveLeft, getContext().langManager.tr("Move left")},
			{Player::Action::MoveRight, getContext().langManager.tr("Move right")},
			{Player::Action::MoveUp, getContext().langManager.tr("Move up")},
			{Player::Action::PickUp, getContext().langManager.tr("Pick up arrow")},
			{Player::Action::Roulade, getContext().langManager.tr("Roulade")},
			{Player::Action::SearchCorpse, getContext().langManager.tr("Search in corpse")},
			{Player::Action::Sneak, getContext().langManager.tr("Sneak")}}
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_title = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_title->setPosition((bindWidth(m_background) - bindWidth(m_title))/2.f, bindHeight(m_background, 0.1f));
	m_title->setTextSize(50);
	m_title->setText(getContext().langManager.tr("Key configuration"));
	m_background->add(m_title);

	m_nameLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_nameLabel->setPosition(bindWidth(m_background, 0.01f + 0.15f*0.98f) - bindWidth(m_nameLabel, 0.5f), bindHeight(m_background, 0.25f));
	m_nameLabel->setTextSize(30);
	m_nameLabel->setText(getContext().langManager.tr("Action"));
	m_background->add(m_nameLabel);

	m_key1Label = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_key1Label->setPosition(bindWidth(m_background, (0.3f+0.075f)*0.98f+0.01f) - bindWidth(m_key1Label, 0.5f), bindHeight(m_background, 0.25f));
	m_key1Label->setTextSize(30);
	m_key1Label->setText(getContext().langManager.tr("Key 1"));
	m_background->add(m_key1Label);

	m_key2Label = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_key2Label->setPosition(bindWidth(m_background, (0.475f+0.075f)*0.98f+0.01f) - bindWidth(m_key2Label, 0.5f), bindHeight(m_background, 0.25f));
	m_key2Label->setTextSize(30);
	m_key2Label->setText(getContext().langManager.tr("Key 2"));
	m_background->add(m_key2Label);

	m_mouseLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_mouseLabel->setPosition(bindWidth(m_background, (0.65f+0.075f)*0.98f+0.01f) - bindWidth(m_mouseLabel, 0.5f), bindHeight(m_background, 0.25f));
	m_mouseLabel->setTextSize(30);
	m_mouseLabel->setText(getContext().langManager.tr("Mouse"));
	m_background->add(m_mouseLabel);

	m_joystickLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_joystickLabel->setPosition(bindWidth(m_background, (0.825f+0.075f)*0.98f+0.01f) - bindWidth(m_joystickLabel, 0.5f), bindHeight(m_background, 0.25f));
	m_joystickLabel->setTextSize(30);
	m_joystickLabel->setText(getContext().langManager.tr("Joystick"));
	m_background->add(m_joystickLabel);

	m_actionsPanel = tgui::Panel::create();
	m_actionsPanel->setPosition(bindWidth(m_background, 0.01f), bindHeight(gui, 0.3f));
	m_actionsPanel->setSize(bindWidth(m_background, 0.98f), bindHeight(m_background, 0.7f));
	m_actionsPanel->setBackgroundColor(sf::Color(0, 0, 0, 0));
	m_background->add(m_actionsPanel);

	m_scrollbar = tgui::Scrollbar::create(getContext().parameters.guiConfigFile);
	m_scrollbar->setPosition(bindWidth(m_actionsPanel, 0.975f), 0.f);
	m_scrollbar->setSize(bindWidth(m_actionsPanel, 0.025f), bindHeight(m_actionsPanel));
    m_scrollbar->setArrowScrollAmount(30);
    m_scrollbar->setLowValue(int(m_actionsPanel->getSize().y));
    m_scrollbar->setMaximum(int(m_actionsPanel->getSize().y*0.1f)*m_actionStrings.size());
    m_scrollbar->connect("valuechanged", &KeyConfigurationState::scrollArea, this);
    m_actionsPanel->add(m_scrollbar);

	float i{0.f};
	for(auto& actionPair : m_actionStrings)
	{
		tgui::Label::Ptr label = tgui::Label::create(getContext().parameters.guiConfigFile);
		m_actionLabels.emplace(actionPair.first, label);
		label->setText(actionPair.second);
		label->setTextSize(25);
		label->setPosition(0.f, bindHeight(m_actionsPanel, i*0.1f+0.05f)-bindHeight(label, 0.5f));
		m_actionsPanel->add(label);
		const auto keyBindings = getContext().player.getAssignedKeys(actionPair.first);
		const auto mouseBindings = getContext().player.getAssignedMouseButtons(actionPair.first);
		const auto joystickBindings = getContext().player.getAssignedJoystickButtons(actionPair.first);
		tgui::Button::Ptr key1Button = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_key1Buttons.emplace(actionPair.first, key1Button);
		key1Button->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		key1Button->setPosition(bindWidth(m_actionsPanel, 0.3f), bindHeight(m_actionsPanel, i*0.1f));
		key1Button->setText(getContext().langManager.tr(keyBindings.empty() ? "[None]" : toString(keyBindings[0])));
		m_actionsPanel->add(key1Button);

		tgui::Button::Ptr key2Button = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_key2Buttons.emplace(actionPair.first, key2Button);
		key2Button->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		key2Button->setPosition(bindWidth(m_actionsPanel, 0.475f), bindHeight(m_actionsPanel, i*0.1f));
		key2Button->setText(getContext().langManager.tr(keyBindings.size() < 2 ? "[None]" : toString(keyBindings[1])));
		m_actionsPanel->add(key2Button);

		tgui::Button::Ptr mouseButton = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_mouseButtons.emplace(actionPair.first, mouseButton);
		mouseButton->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		mouseButton->setPosition(bindWidth(m_actionsPanel, 0.65f), bindHeight(m_actionsPanel, i*0.1f));
		mouseButton->setText(getContext().langManager.tr(mouseBindings.empty() ? "[None]" : toString(mouseBindings[0])));
		m_actionsPanel->add(mouseButton);

		tgui::Button::Ptr joystickButton = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_joystickButtons.emplace(actionPair.first, joystickButton);
		joystickButton->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		joystickButton->setPosition(bindWidth(m_actionsPanel, 0.825f), bindHeight(m_actionsPanel, i*0.1f));
		joystickButton->setText(getContext().langManager.tr(joystickBindings.empty() ? "[None]" : std::to_string(joystickBindings[0])));
		m_actionsPanel->add(joystickButton);

		++i;
	}

}

KeyConfigurationState::~KeyConfigurationState()
{
	getContext().gui.remove(m_background);
}

void KeyConfigurationState::draw()
{
}

bool KeyConfigurationState::update(sf::Time)
{
	return false;
}

bool KeyConfigurationState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPop();
	return false;
}

void KeyConfigurationState::resetTexts()
{
}

void KeyConfigurationState::scrollArea(int newScrollValue)
{
	if(m_scrollbar and m_actionsPanel)
	{
		float i{0.f};
		for(auto& actionPair : m_actionStrings)
		{
			m_actionLabels[actionPair.first]->setPosition(0.f, bindHeight(m_actionsPanel, i*0.1f+0.05f)-bindHeight(m_actionLabels[actionPair.first], 0.5f)-newScrollValue);
			m_key1Buttons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.3f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_key2Buttons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.475f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_mouseButtons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.65f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_joystickButtons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.825f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			++i;
		}
	}
}

sf::String KeyConfigurationState::toString(sf::Keyboard::Key key)
{
	switch (key)
	{
		case sf::Keyboard::Unknown   : return "Unknown";
		case sf::Keyboard::A         : return "A";
		case sf::Keyboard::B         : return "B";
		case sf::Keyboard::C         : return "C";
		case sf::Keyboard::D         : return "D";
		case sf::Keyboard::E         : return "E";
		case sf::Keyboard::F         : return "F";
		case sf::Keyboard::G         : return "G";
		case sf::Keyboard::H         : return "H";
		case sf::Keyboard::I         : return "I";
		case sf::Keyboard::J         : return "J";
		case sf::Keyboard::K         : return "K";
		case sf::Keyboard::L         : return "L";
		case sf::Keyboard::M         : return "M";
		case sf::Keyboard::N         : return "N";
		case sf::Keyboard::O         : return "O";
		case sf::Keyboard::P         : return "P";
		case sf::Keyboard::Q         : return "Q";
		case sf::Keyboard::R         : return "R";
		case sf::Keyboard::S         : return "S";
		case sf::Keyboard::T         : return "T";
		case sf::Keyboard::U         : return "U";
		case sf::Keyboard::V         : return "V";
		case sf::Keyboard::W         : return "W";
		case sf::Keyboard::X         : return "X";
		case sf::Keyboard::Y         : return "Y";
		case sf::Keyboard::Z         : return "Z";
		case sf::Keyboard::Num0      : return "Num 0";
		case sf::Keyboard::Num1      : return "Num 1";
		case sf::Keyboard::Num2      : return "Num 2";
		case sf::Keyboard::Num3      : return "Num 3";
		case sf::Keyboard::Num4      : return "Num 4";
		case sf::Keyboard::Num5      : return "Num 5";
		case sf::Keyboard::Num6      : return "Num 6";
		case sf::Keyboard::Num7      : return "Num 7";
		case sf::Keyboard::Num8      : return "Num 8";
		case sf::Keyboard::Num9      : return "Num 9";
		case sf::Keyboard::Escape    : return "Escape";
		case sf::Keyboard::LControl  : return "Left Ctrl";
		case sf::Keyboard::LShift    : return "Left Shift";
		case sf::Keyboard::LAlt      : return "Left Alt";
		case sf::Keyboard::LSystem   : return "Left System";
		case sf::Keyboard::RControl  : return "Right Ctrl";
		case sf::Keyboard::RShift    : return "Right Shift";
		case sf::Keyboard::RAlt      : return "Right Alt";
		case sf::Keyboard::RSystem   : return "Right System";
		case sf::Keyboard::Menu      : return "Menu";
		case sf::Keyboard::LBracket  : return "[";
		case sf::Keyboard::RBracket  : return "]";
		case sf::Keyboard::SemiColon : return ";";
		case sf::Keyboard::Comma     : return ",";
		case sf::Keyboard::Period    : return ".";
		case sf::Keyboard::Quote     : return "'";
		case sf::Keyboard::Slash     : return "/";
		case sf::Keyboard::BackSlash : return "\\";
		case sf::Keyboard::Tilde     : return "~";
		case sf::Keyboard::Equal     : return "=";
		case sf::Keyboard::Dash      : return "-";
		case sf::Keyboard::Space     : return "Space";
		case sf::Keyboard::Return    : return "Return";
		case sf::Keyboard::BackSpace : return "BackSpace";
		case sf::Keyboard::Tab       : return "Tab";
		case sf::Keyboard::PageUp    : return "Page Up";
		case sf::Keyboard::PageDown  : return "Page Down";
		case sf::Keyboard::End       : return "End";
		case sf::Keyboard::Home      : return "Home";
		case sf::Keyboard::Insert    : return "Insert";
		case sf::Keyboard::Delete    : return "Delete";
		case sf::Keyboard::Add       : return "+";
		case sf::Keyboard::Subtract  : return "-";
		case sf::Keyboard::Multiply  : return "*";
		case sf::Keyboard::Divide    : return "/";
		case sf::Keyboard::Left      : return "Left";
		case sf::Keyboard::Right     : return "Right";
		case sf::Keyboard::Up        : return "Up";
		case sf::Keyboard::Down      : return "Down";
		case sf::Keyboard::Numpad0   : return "Numpad 0";
		case sf::Keyboard::Numpad1   : return "Numpad 1";
		case sf::Keyboard::Numpad2   : return "Numpad 2";
		case sf::Keyboard::Numpad3   : return "Numpad 3";
		case sf::Keyboard::Numpad4   : return "Numpad 4";
		case sf::Keyboard::Numpad5   : return "Numpad 5";
		case sf::Keyboard::Numpad6   : return "Numpad 6";
		case sf::Keyboard::Numpad7   : return "Numpad 7";
		case sf::Keyboard::Numpad8   : return "Numpad 8";
		case sf::Keyboard::Numpad9   : return "Numpad 9";
		case sf::Keyboard::F1        : return "F1";
		case sf::Keyboard::F2        : return "F2";
		case sf::Keyboard::F3        : return "F3";
		case sf::Keyboard::F4        : return "F4";
		case sf::Keyboard::F5        : return "F5";
		case sf::Keyboard::F6        : return "F6";
		case sf::Keyboard::F7        : return "F7";
		case sf::Keyboard::F8        : return "F8";
		case sf::Keyboard::F9        : return "F9";
		case sf::Keyboard::F10       : return "F10";
		case sf::Keyboard::F11       : return "F11";
		case sf::Keyboard::F12       : return "F12";
		case sf::Keyboard::F13       : return "F13";
		case sf::Keyboard::F14       : return "F14";
		case sf::Keyboard::F15       : return "F15";
		case sf::Keyboard::Pause     : return "Pause";
		default: break;
	}
	return "";
}

sf::String KeyConfigurationState::toString(sf::Mouse::Button button)
{
	switch (button)
	{
		case sf::Mouse::Left: return "Left Button";
		case sf::Mouse::Right: return "Right Button";
		case sf::Mouse::Middle: return "Middle Button";
		case sf::Mouse::XButton1: return "Extra Button 1";
		case sf::Mouse::XButton2: return "Extra Button 2";
		default: break;
	}
	return "";
}
