#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/MessageBox.hpp>
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

	m_title = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_title->setPosition((bindWidth(m_background) - bindWidth(m_title))/2.f, bindHeight(m_background, 0.1f));
	m_title->setTextSize(80);
	m_background->add(m_title);

	m_resolutionLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_resolutionLabel->setTextSize(30);
	m_resolutionLabel->setPosition(10.f, bindHeight(m_background, 0.3f));
	m_background->add(m_resolutionLabel);

	m_resolutionComboBox = tgui::ComboBox::create(getContext().parameters.guiConfigFile);
	m_resolutionComboBox->addItem("360p", "0");
	m_resolutionComboBox->addItem("576p", "1");
	m_resolutionComboBox->addItem("720p", "2");
	m_resolutionComboBox->addItem("900p", "3");
	m_resolutionComboBox->addItem("1080p", "4");
	m_resolutionComboBox->setSelectedItemById(std::to_string(getContext().parameters.scaleIndex));
	m_resolutionComboBox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.3f));
	m_resolutionComboBox->setSize(bindWidth(m_background, 0.4f), 30.f);
	m_resolutionComboBox->connect("itemSelected", [this](){
								tgui::MessageBox::Ptr box = tgui::MessageBox::create(getContext().parameters.guiConfigFile);
								box->setText("Takes effects only after restart");
								box->setTextSize(15);
								box->setPosition(bindWidth(m_background, 0.25f), bindHeight(m_background, 0.4f));
								box->setSize(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.2f));
								m_background->add(box);
								});
	m_background->add(m_resolutionComboBox);

	m_langLabel = tgui::Label::copy(m_resolutionLabel);
	m_langLabel->setPosition(10.f, bindHeight(m_background, 0.3666f));
	m_background->add(m_langLabel);

	m_langComboBox = tgui::ComboBox::create(getContext().parameters.guiConfigFile);
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

	m_bloomCheckbox = tgui::Checkbox::create(getContext().parameters.guiConfigFile);
	if(getContext().parameters.bloomEnabled)
		m_bloomCheckbox->check();
	m_bloomCheckbox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.4333f));
	m_background->add(m_bloomCheckbox);

	m_fullscreenLabel = tgui::Label::copy(m_resolutionLabel);
	m_fullscreenLabel->setPosition(10.f, bindHeight(m_background, 0.5f));
	m_background->add(m_fullscreenLabel);

	m_fullscreenCheckbox = tgui::Checkbox::create(getContext().parameters.guiConfigFile);
	if(getContext().parameters.fullscreen)
		m_fullscreenCheckbox->check();
	m_fullscreenCheckbox->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.5));
	m_fullscreenCheckbox->connect("checked", [this](){
								m_fullscreenComboBox->enable();
								m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 255)");
								m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(255, 255, 255, 255)");
								});
	m_fullscreenCheckbox->connect("unchecked", [this](){
								m_fullscreenComboBox->disable();
								m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(200, 200, 200, 255)");
								m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(200, 200, 200, 255)");
								});
	m_background->add(m_fullscreenCheckbox);

	m_fullscreenComboBox = tgui::ComboBox::create(getContext().parameters.guiConfigFile);
	const auto& videoModes(sf::VideoMode::getFullscreenModes());
	unsigned int desktopDepth{sf::VideoMode::getDesktopMode().bitsPerPixel};
	for(size_t i{0}; i < videoModes.size(); ++i)
	{
		if(videoModes[i].bitsPerPixel == desktopDepth)
		{
			m_fullscreenComboBox->addItem(std::to_string(videoModes[i].width) + "x" + std::to_string(videoModes[i].height), std::to_string(i));
			if(m_fullscreenComboBox->getSelectedItem() == "")
				m_fullscreenComboBox->setSelectedItemById(std::to_string(i));
		}
	}
	m_fullscreenComboBox->setPosition(bindWidth(m_background, 0.6f), bindHeight(m_background, 0.5));
	m_fullscreenComboBox->setSize(bindWidth(m_background, 0.3f), 30.f);
	if(not getContext().parameters.fullscreen)
	{
		m_fullscreenComboBox->disable();
		m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(200, 200, 200, 255)");
		m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(200, 200, 200, 255)");
	}
	m_background->add(m_fullscreenComboBox);

	m_mainVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	m_mainVolumeLabel->setPosition(10.f, bindHeight(m_background, 0.5666f));
	m_background->add(m_mainVolumeLabel);

	m_mainVolumeSlider = tgui::Slider::create(getContext().parameters.guiConfigFile);
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

	m_controlsButton = tgui::Button::create(getContext().parameters.guiConfigFile);
	m_controlsButton->setPosition(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.8333f));
	m_controlsButton->setSize(bindWidth(m_background, 0.4f), 30.f);
	m_background->add(m_controlsButton);

	//Buttons
	m_applyButton = tgui::Button::create(getContext().parameters.guiConfigFile);
	m_applyButton->setPosition(bindWidth(m_background, 0.f), bindHeight(m_background, 0.9f));
	m_applyButton->setSize(bindWidth(m_background, 1.f/3.f), bindHeight(m_background, 0.1f));
	m_applyButton->connect("pressed", &ParametersState::applyChanges, this);
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
	StateStack::Context context{getContext()};
	bool langChanged{context.langManager.getLang() != fromString(m_langComboBox->getSelectedItem())};
	bool bloomEnabledChanged{context.parameters.bloomEnabled != m_bloomCheckbox->isChecked()};
	bool fullscreenChanged{context.parameters.fullscreen != m_fullscreenCheckbox->isChecked()};
	const unsigned int scaleIndex{unsigned(std::stoi(m_resolutionComboBox->getSelectedItemId().toAnsiString()))};
	bool resolutionChanged{context.parameters.scaleIndex != scaleIndex};
	if(langChanged)
		context.langManager.setLang(fromString(m_langComboBox->getSelectedItem()));
	if(bloomEnabledChanged)
	{
		if(m_bloomCheckbox->isChecked())
		{
			context.postEffectsTexture.setView(context.window.getView());
			context.window.setView(context.window.getDefaultView());
		}
		else
		{
			context.window.setView(context.postEffectsTexture.getView());
			context.postEffectsTexture.setView(context.postEffectsTexture.getDefaultView());
		}
		context.parameters.bloomEnabled = m_bloomCheckbox->isChecked();
	}
	if(fullscreenChanged)
	{
		sf::VideoMode videoMode;
		uint32 style{sf::Style::Default};
		if(m_fullscreenCheckbox->isChecked())
		{
			style = sf::Style::Fullscreen;
			videoMode = sf::VideoMode::getFullscreenModes()[std::stoul(m_fullscreenComboBox->getSelectedItemId().toAnsiString())];
		}
		else
			videoMode = {unsigned(1920.f*context.parameters.scale), unsigned(1080.f*context.parameters.scale)};
		context.window.create(videoMode, "The Lost Girl", style);
		context.parameters.fullscreen = m_fullscreenCheckbox->isChecked();
		handleResize(context.window, context.parameters.bloomEnabled, context.parameters.scale, context.postEffectsTexture, context.gui);
	}
	if(langChanged or bloomEnabledChanged or resolutionChanged)
		context.eventManager.emit<ParametersChange>(langChanged, bloomEnabledChanged, resolutionChanged, scaleIndex, fullscreenChanged);
}

void ParametersState::resetTexts()
{
	m_title->setText(getContext().langManager.tr("Parameters"));
	m_resolutionLabel->setText(getContext().langManager.tr("Resolution"));
	m_langLabel->setText(getContext().langManager.tr("Lang"));
	m_bloomLabel->setText(getContext().langManager.tr("Bloom effect"));
	m_fullscreenLabel->setText(getContext().langManager.tr("Fullscreen"));
	m_mainVolumeLabel->setText(getContext().langManager.tr("Main volume"));
	m_musicVolumeLabel->setText(getContext().langManager.tr("Music volume"));
	m_effectsVolumeLabel->setText(getContext().langManager.tr("Effects volume"));
	m_ambianceVolumeLabel->setText(getContext().langManager.tr("Ambiance volume"));
	m_controlsLabel->setText(getContext().langManager.tr("Controls"));
	m_controlsButton->setText(getContext().langManager.tr("Configure"));
	m_applyButton->setText(getContext().langManager.tr("Apply"));
	m_cancelButton->setText(getContext().langManager.tr("Cancel"));
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

