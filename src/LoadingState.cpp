#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/LoadingState.h>

LoadingState::LoadingState(StateStack& stack, Context context) :
	State(stack, context),
	m_sentenceLabel{nullptr},
	m_percentLabel{nullptr}
{
	getContext().eventManager.subscribe<LoadingStateChange>(*this);
	
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui = getContext().gui;
	
	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.f), bindHeight(gui, 0.3f));
	m_background->setSize(bindWidth(gui), bindHeight(gui, 0.35f));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);
	
	m_sentenceLabel = tgui::Label::create();
	m_sentenceLabel->setTextSize(30);
	//Center the label
	m_sentenceLabel->setPosition((bindWidth(gui) - bindWidth(m_sentenceLabel))/2, bindHeight(gui, 0.5));
	m_sentenceLabel->setTextColor(sf::Color::Black);
	gui.add(m_sentenceLabel);
	
	m_percentLabel = tgui::Label::create();
	m_percentLabel->setTextSize(50);
	//Center the label
	m_percentLabel->setPosition((bindWidth(gui) - bindWidth(m_percentLabel))/2, bindHeight(gui, 0.38));
	m_percentLabel->setTextColor(sf::Color::Black);
	gui.add(m_percentLabel);
}

LoadingState::~LoadingState()
{
	getContext().gui.remove(m_background);
	getContext().gui.remove(m_sentenceLabel);
	getContext().gui.remove(m_percentLabel);
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
	m_sentence = loadingStateChange.sentence + L"...";
	m_sentenceLabel->setText(m_sentence);
	m_percentLabel->setText(std::to_string(loadingStateChange.percent) + "%");
}