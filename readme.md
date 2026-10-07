# Shuffle: a C++ requirements and TDD exercise

In this exercise you will build a small C++ library, test-first, using GoogleTest.
It is deliberately simple. Read the brief carefully, write tests before code, and
keep a record of any decision you have to make that the brief does not cover.

---

## Part 1: The brief

### Your first ticket[^1]

[^1]: A ticket (or work item) is a single, trackable piece of work. It records what needs doing, who's doing it, its current status, and the criteria that define when it's done.
  Tickets make the team's work visible and shared. Everyone can see what's planned, who's doing what and what's blocked, and each change can be traced back to the reason it was made.

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
> debug overlay shows the current track as `Title: …; Artist: …`, and QA paste
> that into bug reports, so we need to be able to read it back in too.

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

#### Constructor: flexible parsing

The text constructor must accept songs written in either of two formats.

**Format A, `Artist - Title`.** This is how the audio team's spreadsheet lists
tracks.

- Split on the **first** separator, so a title may itself contain one.
- The separator is a hyphen, en dash (`–`) or em dash (`—`) with whitespace on both sides.
  A hyphen with no spaces around it is part of a name (`Jay-Z`, `Blink-182`).
- Extra whitespace around the separator is allowed.
- Leading and trailing whitespace, including a Windows `\r`, is ignored.
- A missing or empty artist becomes `Unknown Artist`.
- A missing or empty title becomes `Unknown Title`.
- The constructor always produces a valid song. It never fails.

| Input                                    | Title                            | Artist           |
| ---------------------------------------- | -------------------------------- | ---------------- |
| `Neon Saints - Overdrive`                | `Overdrive`                      | `Neon Saints`    |
| `  Neon Saints   -   Overdrive  `        | `Overdrive`                      | `Neon Saints`    |
| `Jay-Z - 99 Problems`                    | `99 Problems`                    | `Jay-Z`          |
| `Queen - Bohemian Rhapsody - Remastered` | `Bohemian Rhapsody - Remastered` | `Queen`          |
| `Neon Saints – Overdrive` (en dash)      | `Overdrive`                      | `Neon Saints`    |
| `Title: Overdrive; Artist: Neon Saints`  | `Overdrive`                      | `Neon Saints`    |
| `artist: Neon Saints; title: Overdrive`  | `Overdrive`                      | `Neon Saints`    |
| `Overdrive`                              | `Overdrive`                      | `Unknown Artist` |
| ` - Overdrive`                           | `Overdrive`                      | `Unknown Artist` |
| `Neon Saints - `                         | `Unknown Title`                  | `Neon Saints`    |
| `Title: Overdrive; Artist:`              | `Overdrive`                      | `Unknown Artist` |
| `Title: Overdrive`                       | `Overdrive`                      | `Unknown Artist` |
| `Artist: Neon Saints`                    | `Unknown Title`                  | `Neon Saints`    |
| `-`                                      | `Unknown Title`                  | `Unknown Artist` |
| _(empty or whitespace only)_             | `Unknown Title`                  | `Unknown Artist` |


#### Equality

Two songs are equal when their titles match and their artists match, ignoring
case. The spreadsheet and the bug reports aren't consistent about capitals.
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

1. `Song(title, artist)`, the accessors and `operator==`
2. `print`
3. Parsing format A, starting with the simplest line and adding one rule at a time
4. Parsing format B
5. Missing and empty fields (`Unknown Title`, `Unknown Artist`)
6. The print-then-parse round trip
7. `shuffle`

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
