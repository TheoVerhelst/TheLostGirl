#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

namespace sf
{
	class Music;
	class NonCopyable;
}


class MusicPlayer : private sf::NonCopyable
{
	public:
		MusicPlayer();
		~MusicPlayer();
		void play(const std::string& fileName);
		void stop();
		bool getPaused() const;
		void setPaused(bool paused);
		float getVolume() const;
		void setVolume(float newVolume);

	private:
		sf::Music m_music;
		float m_volume;
		bool m_paused;
};

#endif//MUSICPLAYER_H
