#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/HorizontalLayout.hpp>
#include <TGUI/MessageBox.hpp>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/states/PauseState.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/states/KeyConfigurationState.h>
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

	tgui::VerticalLayout::Ptr mainLayout = tgui::VerticalLayout::create();
	mainLayout->setPosition(bindWidth(m_background, 0.1f), 0.f);
	mainLayout->setSize(bindWidth(m_background, 0.8f), bindHeight(m_background));
	mainLayout->setBackgroundColor(sf::Color::Transparent);
	m_background->add(mainLayout);

	mainLayout->addSpace(0.5f);

	m_title = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_title->setTextSize(80);
	mainLayout->add(m_title);
	mainLayout->setRatio(m_title, 3.f);

	mainLayout->addSpace(0.5f);

	tgui::HorizontalLayout::Ptr layout = tgui::HorizontalLayout::create();
	m_resolutionLabel = tgui::Label::copy(m_title);
	m_resolutionLabel->setTextSize(30);
	layout->add(m_resolutionLabel);

	m_resolutionComboBox = tgui::ComboBox::create(getContext().parameters.guiConfigFile);
	m_resolutionComboBox->addItem("360p", "0");
	m_resolutionComboBox->addItem("576p", "1");
	m_resolutionComboBox->addItem("720p", "2");
	m_resolutionComboBox->addItem("900p", "3");
	m_resolutionComboBox->addItem("1080p", "4");
	m_resolutionComboBox->setSelectedItemById(std::to_string(getContext().parameters.scaleIndex));
	m_resolutionComboBox->connect("itemSelected", [this](){
								tgui::MessageBox::Ptr box = tgui::MessageBox::create(getContext().parameters.guiConfigFile);
								box->setText("Takes effects only after restart");
								box->setTextSize(15);
								box->setPosition(bindWidth(m_background, 0.25f), bindHeight(m_background, 0.4f));
								box->setSize(bindWidth(m_background, 0.5f), bindHeight(m_background, 0.2f));
								m_background->add(box);
								});
	layout->add(m_resolutionComboBox);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_langLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_langLabel);

	m_langComboBox = tgui::ComboBox::create(getContext().parameters.guiConfigFile);
	m_langComboBox->addItem(toString(FR));
	m_langComboBox->addItem(toString(EN));
	m_langComboBox->addItem(toString(IT));
	m_langComboBox->addItem(toString(NL));
	m_langComboBox->setSelectedItem(toString(getContext().langManager.getLang()));
	layout->add(m_langComboBox);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_bloomLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_bloomLabel);
	layout->setRatio(m_bloomLabel, 0.5f);

	m_bloomCheckbox = tgui::Checkbox::create(getContext().parameters.guiConfigFile);
	if(getContext().parameters.bloomEnabled)
		m_bloomCheckbox->check();
	layout->add(m_bloomCheckbox);
	layout->setRatio(m_bloomCheckbox, 0.075f);
	layout->addSpace(0.425f);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_fullscreenLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_fullscreenLabel);
	layout->setRatio(m_fullscreenLabel, 0.5f);

	m_fullscreenCheckbox = tgui::Checkbox::create(getContext().parameters.guiConfigFile);
	if(getContext().parameters.fullscreen)
		m_fullscreenCheckbox->check();
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
	layout->add(m_fullscreenCheckbox);
	layout->setRatio(m_fullscreenCheckbox, 0.075f);
	layout->addSpace(0.025f);

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
	if(not getContext().parameters.fullscreen)
	{
		m_fullscreenComboBox->disable();
		m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(200, 200, 200, 255)");
		m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(200, 200, 200, 255)");
	}
	layout->add(m_fullscreenComboBox);
	layout->setRatio(m_fullscreenComboBox, 0.4f);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_mainVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_mainVolumeLabel);

	m_mainVolumeSlider = tgui::Slider::create(getContext().parameters.guiConfigFile);
	m_mainVolumeSlider->setMaximum(100);
	layout->add(m_mainVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_musicVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_musicVolumeLabel);

	m_musicVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_musicVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_effectsVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_effectsVolumeLabel);

	m_effectsVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_effectsVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_ambianceVolumeLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_ambianceVolumeLabel);

	m_ambianceVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_ambianceVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = tgui::HorizontalLayout::create();
	m_controlsLabel = tgui::Label::copy(m_resolutionLabel);
	layout->add(m_controlsLabel);

	m_controlsButton = tgui::Button::create(getContext().parameters.guiConfigFile);
	unsigned int signal = m_controlsButton->connect("pressed", [this]{m_background->hide(); requestStackPush<KeyConfigurationState>();});
	layout->add(m_controlsButton);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	//Buttons
	layout = tgui::HorizontalLayout::create();
	m_applyButton = tgui::Button::copy(m_controlsButton);
	m_applyButton->disconnect(signal);
	m_applyButton->connect("pressed", [this]{applyChanges();});
	layout->add(m_applyButton);

	m_cancelButton = tgui::Button::copy(m_controlsButton);
	m_cancelButton->disconnect(signal);
	m_cancelButton->connect("pressed", [this]{requestStackPop();});
	layout->add(m_cancelButton);

	m_okButton = tgui::Button::copy(m_controlsButton);
	m_okButton->disconnect(signal);
	m_okButton->connect("pressed", [this](){applyChanges(); requestStackPop();});
	layout->add(m_okButton);
	mainLayout->add(layout);
	mainLayout->setRatio(layout, 3.f);

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
		requestStackPop();
	//If the background was hidden, then a state was pushed on top of this one.
	//But if the code here is executed , that means that this state is again at the top.
	m_background->show();
	return false;
}

void ParametersState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
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

