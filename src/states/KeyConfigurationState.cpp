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
	m_nameLabel->setPosition(0.f, bindHeight(m_background, 0.25f));
	m_nameLabel->setTextSize(30);
	m_nameLabel->setText(getContext().langManager.tr("Action"));
	m_background->add(m_nameLabel);

	m_key1Label = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_key1Label->setPosition(bindWidth(m_background, 0.3f), bindHeight(m_background, 0.25f));
	m_key1Label->setTextSize(30);
	m_key1Label->setText(getContext().langManager.tr("Key 1"));
	m_background->add(m_key1Label);

	m_key2Label = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_key2Label->setPosition(bindWidth(m_background, 0.475f), bindHeight(m_background, 0.25f));
	m_key2Label->setTextSize(30);
	m_key2Label->setText(getContext().langManager.tr("Key 2"));
	m_background->add(m_key2Label);

	m_mouseLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_mouseLabel->setPosition(bindWidth(m_background, 0.65f), bindHeight(m_background, 0.25f));
	m_mouseLabel->setTextSize(30);
	m_mouseLabel->setText(getContext().langManager.tr("Mouse"));
	m_background->add(m_mouseLabel);

	m_joystickLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_joystickLabel->setPosition(bindWidth(m_background, 0.825f), bindHeight(m_background, 0.25f));
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
		label->setPosition(0.f, bindHeight(m_actionsPanel, i*0.1f));
		m_actionsPanel->add(label);

		tgui::Button::Ptr key1Button = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_key1Buttons.emplace(actionPair.first, key1Button);
		key1Button->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		key1Button->setPosition(bindWidth(m_actionsPanel, 0.3f), bindHeight(m_actionsPanel, i*0.1f));
		m_actionsPanel->add(key1Button);

		tgui::Button::Ptr key2Button = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_key2Buttons.emplace(actionPair.first, key2Button);
		key2Button->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		key2Button->setPosition(bindWidth(m_actionsPanel, 0.475f), bindHeight(m_actionsPanel, i*0.1f));
		m_actionsPanel->add(key2Button);

		tgui::Button::Ptr mouseButton = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_mouseButtons.emplace(actionPair.first, mouseButton);
		mouseButton->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		mouseButton->setPosition(bindWidth(m_actionsPanel, 0.65f), bindHeight(m_actionsPanel, i*0.1f));
		m_actionsPanel->add(mouseButton);

		tgui::Button::Ptr joystickButton = tgui::Button::create(getContext().parameters.guiConfigFile);
		m_joystickButtons.emplace(actionPair.first, joystickButton);
		joystickButton->setSize(bindWidth(m_actionsPanel, 0.15f), bindHeight(m_actionsPanel, 0.1f));
		joystickButton->setPosition(bindWidth(m_actionsPanel, 0.825f), bindHeight(m_actionsPanel, i*0.1f));
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
			m_actionLabels[actionPair.first]->setPosition(0.f, bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_key1Buttons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.3f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_key2Buttons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.475f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_mouseButtons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.65f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			m_joystickButtons[actionPair.first]->setPosition(bindWidth(m_actionsPanel, 0.825f), bindHeight(m_actionsPanel, i*0.1f)-newScrollValue);
			++i;
		}
	}
}
