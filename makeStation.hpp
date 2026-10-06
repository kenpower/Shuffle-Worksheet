// return a list of n Songs as a test helper function

#include <vector>
#include <string>
#include "song.hpp"

// Returns n different songs: "Title 1" by "Artist 1", "Title 2" by "Artist 2", ...
std::vector<Song> makeStation(int n) {
    std::vector<Song> station;
    for (int i = 1; i <= n; ++i) {
        station.push_back(Song("Title " + std::to_string(i),
            "Artist " + std::to_string(i)));
    }
    return station;
}