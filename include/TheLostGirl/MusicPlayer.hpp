#ifndef MUSICPLAYER_HPP
#define MUSICPLAYER_HPP

#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

/// Handle the list of musics in the game and play one of them at once.
/// This manager is useful for avoiding multiple musics played at the same time
/// and for have a centralized playlist management.
/// One instance of this class should be used at once in the game,
/// that's why instances cannot be copied.
class MusicPlayer : private sf::NonCopyable
{
	public:
		/// Constructor.
		MusicPlayer();

		/// Destructor.
		~MusicPlayer() = default;

		/// Open and play the music with the given \a fileName.
		/// If another music was open, it is stopped.
		/// \param fileName the file name of the music to play.
		void play(const std::string& fileName);

		/// Stop the currently played music.
		/// If no music is open, do nothing.
		void stop();

		/// Return the state of the current music, or return false if no music is open.
		/// \return True if the current music is played, false otherwise.
		bool getPaused() const;

		/// Pause or unpause the current music.
		/// If no music is open, do nothing.
		/// \param paused True for pause the current music, false for unpause it.
		void setPaused(bool paused);

		/// Return the volume at which music are played, in [0, 100].
		/// The volume is global, so it is not reseted when another music is open.
		/// By default, this is 100.f.
		/// \return The current volume.
		float getVolume() const;

		/// Set the current volume.
		/// \param newVolume The new volume.
		void setVolume(float newVolume);

	private:
		sf::Music m_music;///< The currently played music.
		float m_volume;   ///< The volume at which musics are played.
		bool m_paused;    ///< Pausing state of the music.
};

#endif//MUSICPLAYER_HPP
