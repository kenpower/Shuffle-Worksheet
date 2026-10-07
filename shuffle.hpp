#include <vector>
#include "song.hpp"

std::vector<Song> shuffle(const std::vector<Song>& songs, std::size_t count) {
	std::vector<Song> playlist;                  //empty list of songs to return

//todo: Implement the shuffle function

// ---------------------------------------------------------------------------
// Using std::vector<Song>                        (needs #include <vector>)
// ---------------------------------------------------------------------------
//
// Create
//   std::vector<Song> playlist;                  // empty, size 0

//   std::vector<Song> two = { Song("Neon Saints", "Overdrive"),
//                             Song("Jay-Z", "99 Problems") };
//
// Add
//   playlist.push_back(song);                    // adds a copy of song to the end
//   playlist.push_back(Song("Queen", "Bohemian Rhapsody"));
//
// Size
//   playlist.size()                              // number of songs (a std::size_t)
//   playlist.empty()                             // true if there are no songs
//
// Remove
//   playlist.pop_back();                         // removes the last song
//   playlist.clear();                            // removes all songs
//
// Loop over every song
//   for (const Song& song : playlist) {          // const& = no copy, can't change it
//       song.PrintTo(std::cout);
//   }
//
// Copy a song from one vector to another
//   playlist.push_back(songs[i]);                // copies song i from songs onto the
//                                                // end of playlist; songs is unchanged
//
// Compare (uses Song::operator==)
//   a == b                                       // if a and b are both std::vector<Song>, true if same size, same songs, same order
//
// ---------------------------------------------------------------------------
	
	return playlist;
}