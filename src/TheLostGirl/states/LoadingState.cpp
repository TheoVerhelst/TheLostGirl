#include <random>
#include <fstream>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/states/LoadingState.hpp>

LoadingState::LoadingState():
	m_sentenceChanged{false},
	m_sentenceTimer{0.f}
{
	std::ifstream fileStream(Context::getParameters().resourcesPath + "hints");
	if(not fileStream.is_open())//If failed to open the file
		throw std::runtime_error("Unable to open hints file: \"" + Context::getParameters().resourcesPath + "hints\"");

	long unsigned int numberOfHints;
	getline(fileStream, m_hint);//The first line is the number of hints
	try
	{
		numberOfHints = stoul(m_hint);
	}
	catch(std::invalid_argument& e)
	{
		std::cerr << "Unable to convert content of line 0 to number in file \"" + Context::getParameters().resourcesPath + "lang/hints\"\n";
		throw;//Rethrow the exception
	}

	std::random_device rd;
	std::mt19937 gen{rd()};
	std::uniform_int_distribution<long unsigned int> dis(0, numberOfHints-1);
	long unsigned int hintToDisplay{dis(gen)};

	for(long unsigned int i{0}; i <= hintToDisplay; ++i)
		getline(fileStream, m_hint);
	//m_hint contains now the hint to display

	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(Context::getGui());

	m_background = std::make_shared<tgui::Panel>();
	m_background->setPosition(bindWidth(gui) * 0.f, bindHeight(gui) * 0.3f);
	m_background->setSize(bindWidth(gui), bindHeight(gui) * 0.35f);
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_sentenceLabel = std::make_shared<tgui::Label>();
	m_sentenceLabel->setTextSize(15);
	gui.add(m_sentenceLabel);

	m_hintLabel = std::make_shared<tgui::Label>();
	m_hintLabel->setTextSize(20);
	gui.add(m_hintLabel);

	resetTexts();
	Context::getEventManager().subscribe<LoadingStateChange>(*this);
	Context::getEventManager().subscribe<ParametersChange>(*this);
}

LoadingState::~LoadingState()
{
	Context::getGui().remove(m_background);
	Context::getGui().remove(m_sentenceLabel);
	Context::getGui().remove(m_hintLabel);
}

void LoadingState::draw()
{
}

bool LoadingState::update(sf::Time)
{
	if(m_sentenceChanged)
	{
		resetTexts();
		m_sentenceChanged = false;
	}
	return false;
}

bool LoadingState::handleEvent(const sf::Event&)
{
	return false;
}

void LoadingState::receive(const LoadingStateChange& loadingStateChange)
{
	m_sentence = loadingStateChange.sentence;
	m_sentenceChanged = true;
}

void LoadingState::resetTexts()
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(Context::getGui());
	if(m_sentenceLabel)
	{
		m_sentenceLabel->setText(Context::getLangManager().tr(m_sentence) + L"...");
		//Center the label
		m_sentenceLabel->setPosition((bindWidth(gui) - bindWidth(m_sentenceLabel))/2.f, bindHeight(gui) * 0.5f);
	}
	if(m_hintLabel)
	{
		m_hintLabel->setText(Context::getLangManager().tr(m_hint));
		//Center the label
		m_hintLabel->setPosition((bindWidth(gui) - bindWidth(m_hintLabel))/2.f, bindHeight(gui) * 0.38f);
	}
}


void LoadingState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}
