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
> debug overlay shows the current track as `Artist: …; Title: …`.

That's the whole ticket. Your team lead is in meetings for the rest of the
day. It looks like about two hours' work.

You are building the core library: a `Song` class and a `shuffle` function.

### The `Song` class (skeleton in song.cpp)

A `Song` has an **artist** and a **title**.

```cpp
class Song {
public:
    explicit Song(const std::string& text);              // parse from text
    Song(std::string artist, std::string title);

    bool operator==(const Song& other) const;             // given: see Equality
    void print(std::ostream& out) const;                  // given: see print
};
```

#### Constructor: parsing text

The text constructor reads a song written as `Artist - Title`, the way the audio
team's spreadsheet lists tracks.[^strings]

- First, remove leading and trailing whitespace, including a Windows `\r`.
- The separator is a hyphen, en dash (`-`) or em dash (`—`) with a space on both
  sides. A hyphen with no spaces around it is part of a name (`Jay-Z`,
  `Blink-182`).
- A dash at the very start or very end of the trimmed text also counts as a
  separator, with nothing on that side (so ` - Overdrive` has an empty artist).
- Split on the **first** separator, so a title may itself contain one. If the
  text contains more than one kind of dash, the first is whichever comes
  earliest in the text.
- Extra spaces around the separator are allowed. Trim the artist and title after
  splitting.
- Text with no separator at all is treated as the artist.
- A missing or empty artist becomes `Unknown Artist`.
- A missing or empty title becomes `Unknown Title`.
- The constructor always produces a valid song. It never fails.

| Input                                    | Artist           | Title                            |
| ---------------------------------------- | ---------------- | -------------------------------- |
| `Neon Saints - Overdrive`                | `Neon Saints`    | `Overdrive`                      |
| `  Neon Saints   -   Overdrive  `        | `Neon Saints`    | `Overdrive`                      |
| `Jay-Z - 99 Problems`                    | `Jay-Z`          | `99 Problems`                    |
| `Queen - Bohemian Rhapsody - Remastered` | `Queen`          | `Bohemian Rhapsody - Remastered`                   |
| `Neon Saints - Overdrive - Live`         | `Neon Saints`    | `Overdrive - Live`               |
| `Neon Saints - Overdrive\r`              | `Neon Saints`    | `Overdrive`                      |
| `Neon Saints`                            | `Neon Saints`    | `Unknown Title`                  |
| ` - Overdrive`                           | `Unknown Artist` | `Overdrive`                      |
| `Neon Saints - `                         | `Neon Saints`    | `Unknown Title`                  |
| `-`                                      | `Unknown Artist` | `Unknown Title`                  |
| _(empty or whitespace only)_             | `Unknown Artist` | `Unknown Title`                  |

[^strings]: **Useful `std::string` functions for parsing.** All are in `<string>`
    unless noted.

    - `s.find(" - ")` gives the position of the first match, or
      `std::string::npos` if there isn't one. Use it to find a separator. You
      will need to search for each of the three dashes and take the earliest.
    - `s.substr(pos, len)` copies part of the string. Leave out `len` to go to
      the end. Use it to cut out the artist and title.
    - `s.find_first_not_of(" \t\r\n")` and `s.find_last_not_of(" \t\r\n")`
      give the positions of the first and last characters that aren't
      whitespace. Use them to trim. These treat their argument as a *set* of
      characters, not a sequence, so don't use them to find the separator.
    - `s.empty()` is true if the string has no characters. Use it to decide on
      `Unknown Artist` or `Unknown Title`.

    Two things to watch:

    - **Always check for `npos` before using a position.** `s.substr(npos + 3)`
      wraps round to a small number and gives nonsense, and `substr` with a
      start past the end of the string throws.
    - **The en dash and em dash are three bytes each in UTF-8**, not one
      character. `s.find(" - ")` still works, but the separator is 5 bytes
      long, not 3, so don't hard-code the length: use the length of the
      separator string you searched for.

#### Constructor: artist and title

```cpp
Song(std::string artist, std::string title);
```

The second constructor is for code that already has the artist and title as
separate strings, such as your tests. The arguments come in the same order as
the text format: **artist first, then title**. It does **no splitting**: it
never looks for a separator, so `Song("Neon Saints", "Overdrive - Live")` has
the title `Overdrive - Live`, separator and all.

It follows the same rules for missing values as the text constructor:

- Leading and trailing whitespace is removed.
- An empty artist becomes `Unknown Artist`, and an empty title becomes
  `Unknown Title`.

| Call                                      | Artist           | Title              |
| ----------------------------------------- | ---------------- | ------------------ |
| `Song("Neon Saints", "Overdrive")`        | `Neon Saints`    | `Overdrive`        |
| `Song(" Neon Saints", "  Overdrive ")`    | `Neon Saints`    | `Overdrive`        |
| `Song("Neon Saints", "Overdrive - Live")` | `Neon Saints`    | `Overdrive - Live` |
| `Song("", "Overdrive")`                   | `Unknown Artist` | `Overdrive`        |
| `Song("Neon Saints", "")`                 | `Neon Saints`    | `Unknown Title`    |
| `Song("", "")`                            | `Unknown Artist` | `Unknown Title`    |

This means two songs built in different ways can be compared: a song parsed from
text and a song built from its parts are equal when they hold the same artist and
title.

#### Equality

Two songs are equal when their artists match and their titles match, ignoring
case. The spreadsheet isn't consistent about capitals.
Ignoring case for the letters A-Z is enough; accented letters can be compared
exactly.

`operator==` is **given** in `song.cpp`; you don't write it or test it:

Equality compares only; it does not change the stored text. `print` still shows
the artist and title exactly as they were given.

#### `print`

`print` is **given** in `song.cpp`; you don't write it or test it. It writes the
song to a stream on a single line, with no trailing newline, in the debug
overlay's format:

#### Testing equality: do this first

`operator==` is given, but every other `Song` test compares songs with `==`, so
you need to know it works before anything else can be trusted. Your first tests
check it, using the artist-and-title constructor, which needs no parsing. They
will only pass once that constructor stores its arguments:

```cpp
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
```

Include both kinds of test. An `operator==` that always returns `true` passes
every "equal" test, and one that always returns `false` passes every "not
equal" test. Only the two together prove it works. The same goes for your
constructor: one that ignores its arguments and stores nothing makes every song
equal.

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
    Song expected("Neon Saints", "Overdrive");
    EXPECT_EQ(Song("Neon Saints - Overdrive"), expected);
}

TEST(SongParse, SplitsOnFirstSeparatorOnly) {
    Song expected("Queen", "Bohemian Rhapsody - Remastered");
    EXPECT_EQ(Song("Queen - Bohemian Rhapsody - Remastered"), expected);
}

TEST(SongParse, MissingArtistBecomesUnknownArtist) {
    Song expected("Unknown Artist", "Overdrive");
    EXPECT_EQ(Song(" - Overdrive"), expected);
}
```

This relies on your `operator==` being right, which is why equality is tested
first.


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

Here `makeStation(n)` is a helper in `makestation.cpp` that returns `n`
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
luck if the two calls happen to match. If each call returns all 20 songs in a
random order, that chance is 1 in 20!, about 1 in 2.4 × 10¹⁸, so it will never
happen in practice. With 2 songs it would fail half the time. Choose your test
data with that in mind.

### How to work

Use test-driven development: **red** (write a failing test), **green** (write the
least code that passes), **refactor** (tidy up with the tests still passing).

A suggested order:

1. `Song(artist, title)`, with tests for the given `operator==`
2. Parsing `Artist - Title`, starting with the simplest line and adding one rule at a time
3. Missing and empty fields (`Unknown Artist`, `Unknown Title`)
4. `shuffle`


### Decisions

If the ticket does not tell you what to do in some situation, **do not guess
silently**. Make a sensible decision, write a test that pins it down, and add a
line to `DECISIONS.md` saying what you decided and why.

### Submission

### Checklist

**Setup**
- [ ] Clone the starter repo and check the project builds and the (empty) test suite runs
- [ ] Create `DECISIONS.md`

**`Song(artist, title)` and equality**
- [ ] Run the four `SongEquality` tests (two equal, two not equal) and see them pass


**Parsing `Artist - Title`**
- [ ] One test per row of the parsing table, added one at a time (red → green → refactor)
- [ ] Simple `Artist - Title`
- [ ] Extra spaces and leading/trailing whitespace, including `\r`
- [ ] Hyphen inside a name (`Jay-Z`) is not a separator
- [ ] Splits on the first separator only
- [ ] No separator → artist only
- [ ] Missing artist, missing title, `-` alone, empty and whitespace-only text

**`shuffle`**
- [ ] Test: returns the requested number of songs
- [ ] Test: two calls give different lists (use enough songs)

**Decisions**
- [ ] For each situation the brief doesn't cover, write a test that pins down your choice
- [ ] Record each decision and the reason in `DECISIONS.md`

**Before you submit**
- [ ] All tests pass from a clean build
- [ ] Commits show the TDD rhythm (tests before code), not one big commit
- [ ] Push to GitHub
- [ ] Paste your new decisions into the Blackboard Submission box