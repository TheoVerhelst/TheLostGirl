#include <stdexcept>
#include <SFML/Audio/Music.hpp>

#include <TheLostGirl/MusicPlayer.h>

MusicPlayer::MusicPlayer():
	m_volume{100.f},
	m_paused{true}
{

}

MusicPlayer::~MusicPlayer()
{

}

void MusicPlayer::play(const std::string& fileName)
{
	if(not m_music.openFromFile(fileName))
		throw std::runtime_error("unable to open music file \"" + fileName + "\".");
	m_music.setVolume(m_volume);
	m_music.setLoop(true);
	m_paused = false;
	m_music.play();
}

void MusicPlayer::stop()
{
	m_music.stop();
	m_paused = true;
}

bool MusicPlayer::getPaused() const
{
	return m_paused;
}

void MusicPlayer::setPaused(bool paused)
{
	if(paused)
		m_music.pause();
	else
		m_music.play();
	m_paused = paused;
}

float MusicPlayer::getVolume() const
{
	return m_volume;
}

void MusicPlayer::setVolume(float newVolume)
{
	m_volume = newVolume;
}
