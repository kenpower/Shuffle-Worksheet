#include "pch.h"
#include "song.hpp"
#include "shuffle.hpp"
#include "makeStation.hpp"


TEST(Song, ParseNormalSplit) {
	Song s("Neon Saints - Overdrive");

	EXPECT_TRUE(s == Song("Neon Saints", "Overdrive"));
}



TEST(Shuffle, ReturnsRequestedNumberOfSongs) {
	std::vector<Song> station = makeStation(20);
	EXPECT_EQ(shuffle(station, 5).size(), 5);
}