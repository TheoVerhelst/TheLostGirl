#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/states/PauseState.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/states/KeyConfigurationState.hpp>
#include <TheLostGirl/states/ParametersState.hpp>

ParametersState::ParametersState()
{
	getEventManager().subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getGui());

	m_background = std::make_shared<tgui::Panel>();
	m_background->setPosition(bindWidth(gui) * 0.25f, bindHeight(gui) * 0.f);
	m_background->setSize(bindWidth(gui) * 0.5f, bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	tgui::VerticalLayout::Ptr mainLayout = std::make_shared<tgui::VerticalLayout>();
	mainLayout->setPosition(bindWidth(m_background) * 0.1f, 0.f);
	mainLayout->setSize(bindWidth(m_background) * 0.8f, bindHeight(m_background));
	mainLayout->setBackgroundColor(sf::Color::Transparent);
	m_background->add(mainLayout);

	mainLayout->addSpace(0.5f);

	m_title = getParameters().guiTheme->load("Label");
	m_title->setTextSize(40);
	mainLayout->add(m_title);
	mainLayout->setRatio(m_title, 3.f);

	mainLayout->addSpace(0.5f);

	tgui::HorizontalLayout::Ptr layout = std::make_shared<tgui::HorizontalLayout>();
	m_langLabel = tgui::Label::copy(m_title);
	m_langLabel->setTextSize(15);
	layout->add(m_langLabel);

	m_langComboBox = getParameters().guiTheme->load("ComboBox");
	for(auto& lang : getLangManager().getAvailableLangs())
		m_langComboBox->addItem(lang);
	m_langComboBox->setSelectedItem(getLangManager().getLang());
	layout->add(m_langComboBox);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_bloomLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_bloomLabel);
	layout->setRatio(m_bloomLabel, 0.5f);

	m_bloomCheckBox = getParameters().guiTheme->load("CheckBox");
	if(getParameters().bloomEnabled)
		m_bloomCheckBox->check();
	layout->add(m_bloomCheckBox);
	layout->setRatio(m_bloomCheckBox, 0.075f);
	layout->addSpace(0.425f);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_fullscreenLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_fullscreenLabel);
	layout->setRatio(m_fullscreenLabel, 0.5f);

	m_fullscreenCheckBox = getParameters().guiTheme->load("CheckBox");
	if(getParameters().fullscreen)
		m_fullscreenCheckBox->check();
	m_fullscreenCheckBox->connect("checked", [this]()
	{
		m_fullscreenComboBox->enable();
		m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 255)");
		m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(255, 255, 255, 255)");
	});
	m_fullscreenCheckBox->connect("unchecked", [this]()
	{
		m_fullscreenComboBox->disable();
		m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(200, 200, 200, 255)");
		m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(200, 200, 200, 255)");
	});
	layout->add(m_fullscreenCheckBox);
	layout->setRatio(m_fullscreenCheckBox, 0.075f);
	layout->addSpace(0.025f);

	m_fullscreenComboBox = getParameters().guiTheme->load("ComboBox");
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
	if(not getParameters().fullscreen)
	{
		m_fullscreenComboBox->disable();
		m_fullscreenComboBox->getRenderer()->setProperty("backgroundcolor", "(200, 200, 200, 255)");
		m_fullscreenComboBox->getRenderer()->setProperty("arrowbackgroundcolor", "(200, 200, 200, 255)");
	}
	layout->add(m_fullscreenComboBox);
	layout->setRatio(m_fullscreenComboBox, 0.4f);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_mainVolumeLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_mainVolumeLabel);

	m_mainVolumeSlider = getParameters().guiTheme->load("Slider");
	m_mainVolumeSlider->setMaximum(100);
	layout->add(m_mainVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_musicVolumeLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_musicVolumeLabel);

	m_musicVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_musicVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_effectsVolumeLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_effectsVolumeLabel);

	m_effectsVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_effectsVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_ambianceVolumeLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_ambianceVolumeLabel);

	m_ambianceVolumeSlider = tgui::Slider::copy(m_mainVolumeSlider);
	layout->add(m_ambianceVolumeSlider);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	layout = std::make_shared<tgui::HorizontalLayout>();
	m_controlsLabel = tgui::Label::copy(m_langLabel);
	layout->add(m_controlsLabel);

	m_controlsButton = getParameters().guiTheme->load("Button");
	unsigned int signal = m_controlsButton->connect("pressed", [this]{m_background->hide(); requestStackPush<KeyConfigurationState>();});
	layout->add(m_controlsButton);
	mainLayout->add(layout);

	mainLayout->addSpace(0.5f);

	//Buttons
	layout = std::make_shared<tgui::HorizontalLayout>();
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
	m_okButton->connect("pressed", [this]()
	{
		applyChanges(); requestStackPop();
	});
	layout->add(m_okButton);
	mainLayout->add(layout);
	mainLayout->setRatio(layout, 3.f);

	resetTexts();
}

ParametersState::~ParametersState()
{
	getGui().remove(m_background);
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

void ParametersState::receive(const ParametersChange& changes)
{
	if(changes.langChanged)
		resetTexts();
}

void ParametersState::applyChanges()
{
	ParametersChange changes;
	changes.langChanged = getLangManager().getLang() != m_langComboBox->getSelectedItem();
	changes.bloomEnabledChanged = getParameters().bloomEnabled != m_bloomCheckBox->isChecked();
	changes.fullscreenChanged = getParameters().fullscreen != m_fullscreenCheckBox->isChecked();
	if(changes.langChanged)
		changes.newLang = m_langComboBox->getSelectedItem();
	if(changes.bloomEnabledChanged)
		changes.newBloomEnabledState = m_bloomCheckBox->isChecked();
	if(changes.fullscreenChanged)
	{
		changes.newFullScreenState = m_fullscreenCheckBox->isChecked();
		changes.videoModeIndex = std::stoul(m_fullscreenComboBox->getSelectedItemId().toAnsiString());
	}
	if(changes.langChanged or changes.bloomEnabledChanged or changes.fullscreenChanged)
		getEventManager().emit<ParametersChange>(changes);
}

void ParametersState::resetTexts()
{
	m_title->setText(getLangManager().tr("Parameters"));
	m_langLabel->setText(getLangManager().tr("Lang"));
	m_bloomLabel->setText(getLangManager().tr("Bloom effect"));
	m_fullscreenLabel->setText(getLangManager().tr("Fullscreen"));
	m_mainVolumeLabel->setText(getLangManager().tr("Main volume"));
	m_musicVolumeLabel->setText(getLangManager().tr("Music volume"));
	m_effectsVolumeLabel->setText(getLangManager().tr("Effects volume"));
	m_ambianceVolumeLabel->setText(getLangManager().tr("Ambiance volume"));
	m_controlsLabel->setText(getLangManager().tr("Controls"));
	m_controlsButton->setText(getLangManager().tr("Configure"));
	m_applyButton->setText(getLangManager().tr("Apply"));
	m_cancelButton->setText(getLangManager().tr("Cancel"));
	m_okButton->setText(getLangManager().tr("OK"));
}
