#include <ostream>
#include <string>
#pragma once

class Song {
public:
    Song(const std::string& text) {
		// parse the text to extract title and artist
    
    }            

    Song(std::string title, std::string artist) {
		this->title = title;
		this->artist = artist;

	} // construct from title and artist


	bool operator==(const Song& other) const {
		return other.title == this->title && other.artist == this->artist;
	}
    
	void print(std::ostream& out) const {
		out << "Title: " << title << "; Artist: " << artist;
	}   // Title: Overdrive; Artist: Neon Saints

private:
	std::string title;
	std::string artist;
};