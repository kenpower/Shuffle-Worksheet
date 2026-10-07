# Shuffle: a C++ requirements and TDD exercise

In this exercise you will build a small C++ library, test-first, using GoogleTest.
It is deliberately simple. Read the brief carefully, write tests before code, and
keep a record of any decision you have to make that the brief does not cover.

---

## Part 1: The brief

### Your first ticket

You've just joined the gameplay audio team at Halfmoon Interactive. The studio is
eight months from shipping _Midnight Circuit_, an open-world street racing game
with a big budget and a bigger marketing campaign.

When the player gets into a car, the radio comes on. The game has six radio
stations, each with around 40 licensed tracks, and a lot of money went into that
soundtrack. Every race needs a playlist.

On your first morning, your lead assigns you a ticket:

> **AUDIO-1142: Car radio shuffle**
>
> When a race starts, the radio needs a playlist. Write a function that takes
> the station's tracks and how many we need for the race, and returns a shuffled
> selection.
>
> Track names come from the audio team's spreadsheet as `Artist - Title`. The
> debug overlay shows the current track as `Title: …; Artist: …`.

That's the whole ticket. Your teamlead is in meetings for the rest of the
day. It looks like about two hours work.

You are building the core library: a `Song` class and a `shuffle` function.

### The `Song` class (skeleton in song.cpp)

A `Song` has a **title** and an **artist**.

```cpp
class Song {
public:
    explicit Song(const std::string& text);              // parse from text
    Song(std::string title, std::string artist);

    bool operator==(const Song& other) const;
    void print(std::ostream& out) const;                  // Title: Overdrive; Artist: Neon Saints
};
```

#### Constructor: parsing text

The text constructor reads a song written as `Artist - Title`, the way the audio
team's spreadsheet lists tracks.[^strings]

- Split on the **first** separator, so a title may itself contain one.
- The separator is a hyphen, en dash (`–`) or em dash (`—`) with whitespace on both sides.
  A hyphen with no spaces around it is part of a name (`Jay-Z`, `Blink-182`).
- Extra whitespace around the separator is allowed.
- Leading and trailing whitespace, including a Windows `\r`, is ignored.
- A missing or empty artist becomes `Unknown Artist`.
- A missing or empty title becomes `Unknown Title`.
- The constructor always produces a valid song. It never fails.

| Input                                    | Artist           | Title                            |
| ---------------------------------------- | ---------------- | -------------------------------- |
| `Neon Saints - Overdrive`                | `Neon Saints`    | `Overdrive`                      |
| `  Neon Saints   -   Overdrive  `        | `Neon Saints`    | `Overdrive`                      |
| `Jay-Z - 99 Problems`                    | `Jay-Z`          | `99 Problems`                    |
| `Queen - Bohemian Rhapsody - Remastered` | `Queen`          | `Bohemian Rhapsody - Remastered` |
| `Neon Saints – Overdrive` (en dash)      | `Neon Saints`    | `Overdrive`                      |
| `Neon Saints`                            | `Neon Saints`    | `Unknown Title`                  |
| ` - Overdrive`                           | `Unknown Artist` | `Overdrive`                      |
| `Neon Saints - `                         | `Neon Saints`    | `Unknown Title`                  |
| `-`                                      | `Unknown Artist` | `Unknown Title`                  |
| _(empty or whitespace only)_             | `Unknown Artist` | `Unknown Title`                  |


[^strings]: **Useful `std::string` functions for parsing.** All are in `<string>`
    unless noted.

    - `s.find(" - ")` gives the position of the first match, or
      `std::string::npos` if there isn't one. Use it to find the separator.
    - `s.substr(pos, len)` copies part of the string. Leave out `len` to go to
      the end. Use it to cut out the artist and title.
    - `s.find_first_not_of(" \t\r\n")` and `s.find_last_not_of(" \t\r\n")`
      give the positions of the first and last characters that aren't
      whitespace. Use them to trim. (DOES NOT FIND SUBSTRINGS)
    - `s.empty()` is true if the string has no characters. Use it to decide on
      `Unknown Title` or `Unknown Artist`.
    - `std::tolower(c)`, from `<cctype>`, gives the lower-case version of one
      character. Use it for case-insensitive equality.

    Three things to watch:

    - **Always check for `npos` before using a position.** `s.substr(npos + 3)`
      wraps round to a small number and gives nonsense, and `substr` with a
      start past the end of the string throws.
    - **The en dash and em dash are three bytes each in UTF-8**, not one
      character. `s.find(" – ")` still works, but the separator is 5 bytes
      long, not 3, so don't hard-code the length: use the length of the
      separator string you searched for.
    - **Pass `std::tolower` an `unsigned char`**:
      `std::tolower(static_cast<unsigned char>(c))`. A plain `char` can be
      negative, which is undefined behaviour.

#### Constructor: title and artist

```cpp
Song(std::string title, std::string artist);
```

The second constructor is for code that already has the title and artist as
separate strings, such as your tests. It does **no parsing**: it stores the two
strings as they are, so `Song("Overdrive - Live", "Neon Saints")` has the title
`Overdrive - Live`, separator and all.

Note the order: **title first, then artist**. That's the opposite of the text
format (`Artist - Title`), and an easy mistake to make in a test.

It follows the same rules for missing values as the text constructor:

- Leading and trailing whitespace is removed.
- An empty title becomes `Unknown Title`, and an empty artist becomes
  `Unknown Artist`.

| Call                                  | Title           | Artist           |
| ------------------------------------- | --------------- | ---------------- |
| `Song("Overdrive", "Neon Saints")`    | `Overdrive`     | `Neon Saints`    |
| `Song("  Overdrive ", " Neon Saints")`| `Overdrive`     | `Neon Saints`    |
| `Song("Overdrive - Live", "Neon Saints")` | `Overdrive - Live` | `Neon Saints` |
| `Song("Overdrive", "")`               | `Overdrive`     | `Unknown Artist` |
| `Song("", "Neon Saints")`             | `Unknown Title` | `Neon Saints`    |
| `Song("", "")`                        | `Unknown Title` | `Unknown Artist` |

This means two songs built in different ways can be compared: a song parsed from
text and a song built from its parts are equal when they hold the same title and
artist.

#### Equality

Two songs are equal when their titles match and their artists match, ignoring
case. The spreadsheet isn't consistent about capitals.
Ignoring case for the letters A–Z is enough; accented letters can be compared
exactly.

| Song A                             | Song B                              | Equal? |
| ---------------------------------- | ----------------------------------- | ------ |
| `Overdrive` / `Neon Saints`        | `Overdrive` / `Neon Saints`         | yes    |
| `Overdrive` / `Neon Saints`        | `OVERDRIVE` / `neon saints`         | yes    |
| `Overdrive` / `Neon Saints`        | `Overdrive (Remix)` / `Neon Saints` | no     |
| `Hallelujah` / `Leonard Cohen`     | `Hallelujah` / `Jeff Buckley`       | no     |
| `Unknown Title` / `Unknown Artist` | `Unknown Title` / `Unknown Artist`  | yes    |

Equality compares only; it does not change the stored text. `print` still shows
the title and artist exactly as they were given.

#### `print`

Writes the song to a stream on a single line, with no trailing newline:

```
Title: Overdrive; Artist: Neon Saints
```

#### Testing equality: do this first

Every other `Song` test compares songs with `==`, so `operator==` has to be
right before anything else can be trusted. Test it first, using the
title-and-artist constructor, which needs no parsing:

```cpp
TEST(SongEquality, SameTitleAndArtistAreEqual) {
    EXPECT_TRUE(Song("Overdrive", "Neon Saints") == Song("Overdrive", "Neon Saints"));
}

TEST(SongEquality, IgnoresCase) {
    EXPECT_TRUE(Song("Overdrive", "Neon Saints") == Song("OVERDRIVE", "neon saints"));
}

TEST(SongEquality, DifferentTitleNotEqual) {
    EXPECT_FALSE(Song("Overdrive", "Neon Saints") == Song("Overdrive (Remix)", "Neon Saints"));
}

TEST(SongEquality, DifferentArtistNotEqual) {
    EXPECT_FALSE(Song("Hallelujah", "Leonard Cohen") == Song("Hallelujah", "Jeff Buckley"));
}
```

Include both kinds of test. An `operator==` that always returns `true` passes
every "equal" test, and one that always returns `false` passes every "not
equal" test. Only the two together prove it works.

These tests use `EXPECT_TRUE(a == b)` rather than `EXPECT_EQ(a, b)` so that
they read as a test of `==` itself. Once equality is tested, use `EXPECT_EQ`
everywhere else: it gives better failure messages.

#### Testing the constructor

`Song` has no getters, so a test can't ask a song for its title. There are two
ways to check what the constructor did.

**1. Compare with a song you build directly.** Use the two-argument
constructor to build the song you expect, then compare the two with `==`:

```cpp
TEST(SongParse, ArtistDashTitle) {
    Song expected("Overdrive", "Neon Saints");
    EXPECT_EQ(Song("Neon Saints - Overdrive"), expected);
}

TEST(SongParse, SplitsOnFirstSeparatorOnly) {
    Song expected("Bohemian Rhapsody - Remastered", "Queen");
    EXPECT_EQ(Song("Queen - Bohemian Rhapsody - Remastered"), expected);
}

TEST(SongParse, MissingArtistBecomesUnknownArtist) {
    Song expected("Overdrive", "Unknown Artist");
    EXPECT_EQ(Song(" - Overdrive"), expected);
}
```

This relies on your `operator==` being right, which is why equality is tested
first.

**2. Check the printed text.** Because equality ignores case, method 1 can't
tell `Neon Saints` from `neon saints`. When the exact text matters, print the
song into a string and check that instead:

```cpp
TEST(SongParse, KeepsOriginalCapitals) {
    Song song("Neon Saints - Overdrive");
    std::ostringstream out;
    song.print(out);
    EXPECT_EQ(out.str(), "Title: Overdrive; Artist: Neon Saints");
}

TEST(SongParse, TrimsSpacesAroundNames) {
    Song song("  Neon Saints   -   Overdrive  ");
    std::ostringstream out;
    song.print(out);
    EXPECT_EQ(out.str(), "Title: Overdrive; Artist: Neon Saints");
}
```

Method 2 also catches stray spaces that equality would miss: if your parser
left a space on the end of `Overdrive `, the two songs would not be equal, but
the printed text makes the problem easy to see.

Each row of the constructor table above is one test. Start with the first row,
make it pass, then add the next.

### The `shuffle` function (skeleton in shuffle.cpp)

```cpp
std::vector<Song> shuffle(const std::vector<Song>& songs, std::size_t count);
```

Given a station's songs and a number, `shuffle` returns a shuffled selection of
`count` songs from the station.

#### Testing `shuffle`

You can't predict what a random function returns, so test properties that are
always true instead of exact results. At a minimum:

- **The result has the right size.** `shuffle(songs, 5)` returns 5 songs.
- **Calling `shuffle` twice gives different lists.** Two calls with the same
  songs and count should not return the same order.

```cpp
TEST(Shuffle, ReturnsRequestedNumberOfSongs) {
    std::vector<Song> station = makeStation(20);
    EXPECT_EQ(shuffle(station, 5).size(), 5u);
}

TEST(Shuffle, TwoCallsGiveDifferentLists) {
    std::vector<Song> station = makeStation(20);
    EXPECT_NE(shuffle(station, 20), shuffle(station, 20));
}
```

Here `makeStation(n)` is a helper in `makestation.cpp` that retuns `n`
different songs.

> **Comparing vectors.** `EXPECT_EQ` and `EXPECT_NE` work directly on two
> `std::vector`s. Two vectors are equal when they have the same size and every
> pair of elements in the same position is equal, compared with your
> `Song::operator==`. This has three consequences:
>
> - **Order matters.** `{A, B, C}` and `{C, B, A}` are different vectors, which
>   is exactly what the "different lists" test relies on.
> - **Your `operator==` is used.** Because song equality ignores case, two lists
>   that differ only in capitals count as equal.
> - **To check two vectors hold the same songs in any order**, use
>   `std::is_permutation` from `<algorithm>` instead:
>   `EXPECT_TRUE(std::is_permutation(a.begin(), a.end(), b.begin()));`
>
> If a vector comparison fails, GoogleTest prints both vectors. Define `PrintTo`
> for `Song` (see the tips below) so you can read them.

The "different lists" test is checking something random, so it can fail by bad
luck if the two calls happen to match. With 20 songs that chance is about 1 in
2.4 × 10¹⁸, so it will never happen in practice. With 2 songs it would fail
half the time. Choose your test data with that in mind.

### How to work

Use test-driven development: **red** (write a failing test), **green** (write the
least code that passes), **refactor** (tidy up with the tests still passing).

A suggested order:

1. `Song(title, artist)` and `operator==`
2. `print`
3. Parsing `Artist - Title`, starting with the simplest line and adding one rule at a time
4. Missing and empty fields (`Unknown Title`, `Unknown Artist`)
5. `shuffle`

Some tips:

- Test against `std::ostringstream` rather than `std::cout`.
- To print `Song`s readably when a test fails, define
  `void PrintTo(const Song& s, std::ostream* os)` in your test code.

### Decisions

If the ticket does not tell you what to do in some situation, **do not guess
silently**. Make a sensible decision, write a test that pins it down, and add a
line to `DECISIONS.md` saying what you decided and why.

### Submission

Push to github **and** paste your **new decisions** into the Submission box in the blackboard assignment
