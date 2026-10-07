#include "pch.h"
#include "song.hpp"
#include "shuffle.hpp"
#include "makeStation.hpp"

TEST(SongEquality, SameArtistAndTitleAreEqual) {
	EXPECT_TRUE(Song("Neon Saints", "Overdrive") == Song("Neon Saints", "Overdrive"));
}


TEST(SongEquality, IgnoresCase) {
	EXPECT_TRUE(Song("Neon Saints", "Overdrive") == Song("neon saints", "OVERDRIVE"));
}


TEST(SongEquality, DifferentTitleNotEqual) {
	EXPECT_FALSE(Song("Neon Saints", "Overdrive") == Song("Neon Saints", "Overdrive (Remix)"));
}

TEST(SongEquality, DifferentArtistNotEqual) {
	EXPECT_FALSE(Song("Leonard Cohen", "Hallelujah") == Song("Jeff Buckley", "Hallelujah"));
}


//Some example tests for the Song class. You should add more tests to cover other cases.
TEST(Song, ParseNormalSplit) {
	Song s("Neon Saints - Overdrive");

	EXPECT_TRUE(s == Song("Neon Saints", "Overdrive"));
}

//
//TEST(Shuffle, ReturnsRequestedNumberOfSongs) {
//	std::vector<Song> station = makeStation(20);
//	EXPECT_EQ(shuffle(station, 5).size(), 5);
//}