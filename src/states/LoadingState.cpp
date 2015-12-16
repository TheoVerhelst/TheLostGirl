#include <random>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/states/LoadingState.h>

LoadingState::LoadingState()
{
	Context::eventManager->subscribe<LoadingStateChange>(*this);
	Context::eventManager->subscribe<ParametersChange>(*this);
	std::ifstream fileStream("resources/lang/hints");
	if(not fileStream.is_open())//If failed to open the file
		throw std::runtime_error("Unable to open hints file: resources/lang/hints");
	else
	{
		std::string line;
		long unsigned int numberOfHints;
		getline(fileStream, line);//The first line is the number of hints
		try
		{
			numberOfHints = stoul(line);
		}
		catch(std::invalid_argument & e)
		{
			std::cerr << "Unable to convert content of line 0 to number in file \"resources/lang/hints\"\n";
			throw;//Rethrow the exception
		}

		std::random_device rd;
		std::mt19937 gen{rd()};
		std::uniform_int_distribution<long unsigned int> dis(0, numberOfHints-1);
		long unsigned int hintToDisplay{dis(gen)};

		for(long unsigned int i{0}; i <= hintToDisplay; ++i)
			getline(fileStream, line);
        //Line contains now the hint to display

		using tgui::bindWidth;
		using tgui::bindHeight;
		tgui::Gui& gui(*Context::gui);

		m_background = std::make_shared<tgui::Panel>();
		m_background->setPosition(bindWidth(gui) * 0.f, bindHeight(gui) * 0.3f);
		m_background->setSize(bindWidth(gui), bindHeight(gui) * 0.35f);
		m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
		gui.add(m_background);

		m_sentenceLabel = Context::parameters->guiTheme->load("Label");
		m_sentenceLabel->setText(m_sentence);
		m_sentenceLabel->setTextSize(10);
		//Center the label
		m_sentenceLabel->setPosition((bindWidth(gui) - bindWidth(m_sentenceLabel))/2, bindHeight(gui) * 0.5f);
		gui.add(m_sentenceLabel);

		m_hintLabel = Context::parameters->guiTheme->load("Label");
		m_hintLabel->setText(Context::langManager->tr(line));
		m_hintLabel->setTextSize(15);
		//Center the label
		m_hintLabel->setPosition((bindWidth(gui) - bindWidth(m_hintLabel))/2.f, bindHeight(gui) * 0.38f);
		gui.add(m_hintLabel);
	}
}

LoadingState::~LoadingState()
{
	Context::gui->remove(m_background);
	Context::gui->remove(m_sentenceLabel);
	Context::gui->remove(m_hintLabel);
}

void LoadingState::draw()
{
}

bool LoadingState::update(sf::Time)
{
	return false;
}

bool LoadingState::handleEvent(const sf::Event&)
{
	return false;
}

void LoadingState::receive(const LoadingStateChange &loadingStateChange)
{
	if(m_sentenceLabel)
	{
		m_sentence = loadingStateChange.sentence;
		resetTexts();
	}
}

void LoadingState::resetTexts()
{
	m_sentenceLabel->setText(Context::langManager->tr(m_sentence) + L"...");
}


void LoadingState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}
