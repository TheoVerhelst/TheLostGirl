#include <random>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/states/LoadingState.h>

LoadingState::LoadingState(StateStack& stack, Context context):
	State(stack, context),
	m_sentenceLabel{nullptr},
	m_hintLabel{nullptr}
{
	getContext().eventManager.subscribe<LoadingStateChange>(*this);
	std::ifstream fileStream("resources/lang/hints");
	if(not fileStream.is_open())//If failed to open the file
		throw std::runtime_error("Unable to open hints file : resources/lang/hints");
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
			std::cerr << "Unable to convert content of line 0 to number in file \"resources/lang/hints\"" << std::endl;
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
		tgui::Gui& gui(getContext().gui);
		
		m_background = tgui::Panel::create();
		m_background->setPosition(bindWidth(gui, 0.f), bindHeight(gui, 0.3f));
		m_background->setSize(bindWidth(gui), bindHeight(gui, 0.35f));
		m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
		gui.add(m_background);
		
		m_sentence = L"...";
		m_sentenceLabel = tgui::Label::create();
		m_sentenceLabel->setText(m_sentence);
		m_sentenceLabel->setTextSize(20);
		//Center the label
		m_sentenceLabel->setPosition((bindWidth(gui) - bindWidth(m_sentenceLabel))/2, bindHeight(gui, 0.5));
		m_sentenceLabel->setTextColor(sf::Color::Black);
		gui.add(m_sentenceLabel);
		
		m_hintLabel = tgui::Label::create();
		m_hintLabel->setText(LangManager::tr(line));
		m_hintLabel->setTextSize(30);
		//Center the label
		m_hintLabel->setPosition((bindWidth(gui) - bindWidth(m_hintLabel))/2, bindHeight(gui, 0.38));
		m_hintLabel->setTextColor(sf::Color::Black);
		gui.add(m_hintLabel);
	}
}

LoadingState::~LoadingState()
{
	getContext().gui.remove(m_background);
	getContext().gui.remove(m_sentenceLabel);
	getContext().gui.remove(m_hintLabel);
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
		m_sentence = loadingStateChange.sentence + L"...";
		m_sentenceLabel->setText(m_sentence);
	}
}
