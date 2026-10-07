#include "pch.h"
#include "song.hpp"
#include "shuffle.hpp"
#include "makeStation.hpp"

TEST(SongEquality, SameTitleAndArtistAreEqual) {
	EXPECT_TRUE(Song("Overdrive", "Neon Saints") == Song("Overdrive", "Neon Saints"));
}

 
TEST(SongEquality, IgnoresCase) {
	EXPECT_TRUE(Song("Overdrive", "Neon Saints") == Song("OVERDRIVE", "neon saints"));
}

//
//Some example tests for the Song class. You should add more tests to cover other cases.
//TEST(SongEquality, DifferentTitleNotEqual) {
//	EXPECT_FALSE(Song("Overdrive", "Neon Saints") == Song("Overdrive (Remix)", "Neon Saints"));
//}
//
//TEST(SongEquality, DifferentArtistNotEqual) {
//	EXPECT_FALSE(Song("Hallelujah", "Leonard Cohen") == Song("Hallelujah", "Jeff Buckley"));
//}
//
//TEST(Song, ParseNormalSplit) {
//	Song s("Neon Saints - Overdrive");
//
//	EXPECT_TRUE(s == Song("Neon Saints", "Overdrive"));
//}
//
//
//TEST(Shuffle, ReturnsRequestedNumberOfSongs) {
//	std::vector<Song> station = makeStation(20);
//	EXPECT_EQ(shuffle(station, 5).size(), 5);
//}