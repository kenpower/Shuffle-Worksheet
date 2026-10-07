#include <ostream>
#include <string>
#pragma once

std::string toLower(const std::string& input_text) {
	std::string lower_text = input_text;
	for (char& c : lower_text) {
		c = std::tolower(static_cast<unsigned char>(c));
	}
	return lower_text;
}

class Song {
public:
    Song(const std::string& text) {
		// parse the text to extract title and artist
    
    }            

    Song(std::string _artist, std::string _title) {
		artist = _artist;
		title = _title;

	} 


	//this function is complete, no need to modify it
	bool operator==(const Song& other) const {
		bool artistSame = toLower(other.artist) == toLower(artist);
		bool titleSame = toLower(other.title) == toLower(title);
		return titleSame && artistSame;
	}
    

	//this function is complete, no need to modify it
	void PrintTo(const Song& s, std::ostream* os) {
		*os << "Artist: " << artist << "; Title: " << title;
	}

private:
	std::string artist;
	std::string title;
};