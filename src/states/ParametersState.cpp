#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/states/PauseState.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/states/ParametersState.h>

ParametersState::ParametersState(StateStack& stack) :
	State(stack)
{
	getContext().eventManager.subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_title = tgui::Label::create();
	m_title->setPosition((bindWidth(m_background) - bindWidth(m_title))/2.f, bindHeight(m_background, 0.1f));
	m_title->setTextSize(80);
	m_title->setTextColor(sf::Color::Black);
	m_background->add(m_title);

	m_resolutionLabel = tgui::Label::create();
	m_resolutionLabel->setTextSize(30);
	m_resolutionLabel->setPosition(10.f, bindHeight(m_background, 0.3f));
	m_resolutionLabel->setTextColor(sf::Color::Black);
	m_background->add(m_resolutionLabel);

	m_resolutionComboBox = tgui::ComboBox::create();
	m_resolutionComboBox->addItem("360p", "0");
	m_resolutionComboBox->addItem("576p", "1");
	m_resolutionComboBox->addItem("720p", "2");
	m_resolutionComboBox->addItem("900p", "3");
	m_resolutionComboBox->addItem("1080p", "4");
	m_resolutionComboBox->setSelectedItemById(std::to_string(getContext().parameters.scaleIndex));
	m_resolutionComboBox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.3f));
	m_resolutionComboBox->setSize(bindWidth(m_background, 0.4f), 30.f);
	m_background->add(m_resolutionComboBox);

	m_langLabel = tgui::Label::copy(m_resolutionLabel);
	m_langLabel->setPosition(10.f, bindHeight(m_background, 0.3666f));
	m_background->add(m_langLabel);

	m_langComboBox = tgui::ComboBox::create();
	m_langComboBox->addItem(toString(FR));
	m_langComboBox->addItem(toString(EN));
	m_langComboBox->addItem(toString(IT));
	m_langComboBox->addItem(toString(NL));
	m_langComboBox->setSelectedItem(toString(getContext().langManager.getLang()));
	m_langComboBox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.3666f));
	m_langComboBox->setSize(bindWidth(m_background, 0.4f), 30.f);
	m_background->add(m_langComboBox);

	m_bloomLabel = tgui::Label::copy(m_resolutionLabel);
	m_bloomLabel->setPosition(10.f, bindHeight(m_background, 0.4333f));
	m_background->add(m_bloomLabel);

	m_bloomCheckbox = tgui::Checkbox::create();
	if(getContext().parameters.bloomEnabled)
		m_bloomCheckbox->check();
	m_bloomCheckbox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.4333f));
	m_background->add(m_bloomCheckbox);

	m_fullscreenLabel = tgui::Label::copy(m_resolutionLabel);
	m_fullscreenLabel->setPosition(10.f, bindHeight(m_background, 0.5f));
	m_background->add(m_fullscreenLabel);

	m_fullscreenCheckbox = tgui::Checkbox::create();
	m_fullscreenCheckbox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.5));
	m_background->add(m_fullscreenCheckbox);

	m_fullscreenComboBox = tgui::ComboBox::create();
	for(sf::VideoMode videoMode : sf::VideoMode::getFullscreenModes())
		if(std::abs(float(videoMode.width)/float(videoMode.height) - 1920.f/1080.f) < 0.01f)
			m_fullscreenComboBox->addItem(std::to_string(videoMode.width) + "x" + std::to_string(videoMode.height));
	m_fullscreenComboBox->setPosition(bindWidth(m_background, 0.6f), bindHeight(m_background, 0.5));
	m_fullscreenComboBox->setSize(bindWidth(m_background, 0.3f), 30.f);
	m_background->add(m_fullscreenComboBox);

	m_mainVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	m_mainVolumeLabel->setPosition(10.f, bindHeight(m_background, 0.5666f));
	m_background->add(m_mainVolumeLabel);

	m_mainVolumeSlider = tgui::Slider::create();
	m_mainVolumeSlider->setMaximum(100);
	m_mainVolumeSlider->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.5666f));
	m_mainVolumeSlider->setSize(bindWidth(m_background, 0.4f), 20.f);
	m_background->add(m_mainVolumeSlider);

	m_musicVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	m_musicVolumeLabel->setPosition(10.f, bindHeight(m_background, 0.6333f));
	m_background->add(m_musicVolumeLabel);

	m_musicVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	m_musicVolumeSlider->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.6333f));
	m_background->add(m_musicVolumeSlider);

	m_effectsVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	m_effectsVolumeLabel->setPosition(10.f, bindHeight(m_background, 0.7f));
	m_background->add(m_effectsVolumeLabel);

	m_effectsVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	m_effectsVolumeSlider->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.7f));
	m_background->add(m_effectsVolumeSlider);

	m_ambianceVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	m_ambianceVolumeLabel->setPosition(10.f, bindHeight(m_background, 0.7666f));
	m_background->add(m_ambianceVolumeLabel);

	m_ambianceVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	m_ambianceVolumeSlider->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.7666f));
	m_background->add(m_ambianceVolumeSlider);

	m_controlsLabel = tgui::Label::copy(m_resolutionLabel);
	m_controlsLabel->setPosition(10.f, bindHeight(m_background, 0.8333f));
	m_background->add(m_controlsLabel);

	m_controlsButton = tgui::Button::create();
	m_controlsButton->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.8333f));
	m_controlsButton->setSize(bindWidth(m_background, 0.4f), 30.f);
	prettifyButton(m_controlsButton);
	m_background->add(m_controlsButton);

	//Buttons
	m_applyButton = tgui::Button::create();
	m_applyButton->setPosition(bindWidth(m_background, 0.f), bindHeight(m_background, 0.9f));
	m_applyButton->setSize(bindWidth(m_background, 1.f/3.f), bindHeight(m_background, 0.1f));
	m_applyButton->connect("pressed", &ParametersState::applyChanges, this);
	prettifyButton(m_applyButton);
	m_background->add(m_applyButton);

	m_cancelButton = tgui::Button::copy(m_applyButton);
	m_cancelButton->setPosition(bindWidth(m_background, 1.f/3.f), bindHeight(m_background, 0.9f));
	m_cancelButton->connect("pressed", &ParametersState::backToPause, this);
	m_background->add(m_cancelButton);

	m_okButton = tgui::Button::copy(m_applyButton);
	m_okButton->setPosition(bindWidth(m_background, 2.f/3.f), bindHeight(m_background, 0.9f));
	m_okButton->connect("pressed", [this](){applyChanges(); backToPause();});
	m_background->add(m_okButton);

	resetTexts();
}

ParametersState::~ParametersState()
{
	getContext().gui.remove(m_background);
}

void ParametersState::draw()
{}

bool ParametersState::update(sf::Time)
{
	return false;
}

bool ParametersState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		backToPause();
	return false;
}

void ParametersState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}

void ParametersState::backToPause()
{
	requestStackPop();
	requestStackPush<PauseState>();
}

void ParametersState::applyChanges()
{
	bool langChanged{getContext().langManager.getLang() != fromString(m_langComboBox->getSelectedItem())};
	bool bloomEnabledChanged{getContext().parameters.bloomEnabled != m_bloomCheckbox->isChecked()};
	const unsigned int scaleIndex{unsigned(std::stoi(m_resolutionComboBox->getSelectedItemId().toAnsiString()))};
	bool resolutionChanged{getContext().parameters.scaleIndex != scaleIndex};
	if(langChanged)
		getContext().langManager.setLang(fromString(m_langComboBox->getSelectedItem()));
	if(bloomEnabledChanged)
	{
		if(m_bloomCheckbox->isChecked())
		{
			getContext().postEffectsTexture.setView(getContext().window.getView());
			getContext().window.setView(getContext().window.getDefaultView());
		}
		else
		{
			getContext().window.setView(getContext().postEffectsTexture.getView());
			getContext().postEffectsTexture.setView(getContext().postEffectsTexture.getDefaultView());
		}
		getContext().parameters.bloomEnabled = m_bloomCheckbox->isChecked();
	}
	if(langChanged or bloomEnabledChanged or resolutionChanged)
		getContext().eventManager.emit<ParametersChange>(langChanged, bloomEnabledChanged, resolutionChanged, scaleIndex);
}

void ParametersState::resetTexts()
{
	if(m_title)
		m_title->setText(getContext().langManager.tr("Parameters"));
	if(m_resolutionLabel)
		m_resolutionLabel->setText(getContext().langManager.tr("Resolution"));
	if(m_langLabel)
		m_langLabel->setText(getContext().langManager.tr("Lang"));
	if(m_bloomLabel)
		m_bloomLabel->setText(getContext().langManager.tr("Bloom effect"));
	if(m_fullscreenLabel)
		m_fullscreenLabel->setText(getContext().langManager.tr("Fullscreen"));
	if(m_mainVolumeLabel)
		m_mainVolumeLabel->setText(getContext().langManager.tr("Main volume"));
	if(m_musicVolumeLabel)
		m_musicVolumeLabel->setText(getContext().langManager.tr("Music volume"));
	if(m_effectsVolumeLabel)
		m_effectsVolumeLabel->setText(getContext().langManager.tr("Effects volume"));
	if(m_ambianceVolumeLabel)
		m_ambianceVolumeLabel->setText(getContext().langManager.tr("Ambiance volume"));
	if(m_controlsLabel)
		m_controlsLabel->setText(getContext().langManager.tr("Controls"));
	if(m_controlsButton)
		m_controlsButton->setText(getContext().langManager.tr("Configure"));
	if(m_applyButton)
		m_applyButton->setText(getContext().langManager.tr("Apply"));
	if(m_cancelButton)
		m_cancelButton->setText(getContext().langManager.tr("Cancel"));
	if(m_okButton)
		m_okButton->setText(getContext().langManager.tr("OK"));
}

sf::String ParametersState::toString(Lang lang)
{
	switch(lang)
	{
		case FR:
			return L"Français";
		case IT:
			return "Italiano";
		case NL:
			return "Neederlands";
		case EN:
		default:
			return "English";
	}
}

Lang ParametersState::fromString(sf::String string)
{
	if(string == L"Français")
		return FR;
	else if(string == "Italiano")
		return IT;
	else if(string == "Nederlands")
		return NL;
	else
		return EN;
}

