# Decisions log: AUDIO-1142 (car radio shuffle)

Tickets never cover everything. When you hit a situation the ticket doesn't
describe, you still have to make the code do _something_. This file records what
you chose, so that your lead, QA and whoever maintains this code after you can
see that it was a deliberate decision and not an accident.

For every decision:

1. **Write it down here**, using the template below.
2. **Write a test that pins it down**, so that nobody changes the behaviour by
   accident later.
3. **Note who should really make the call.** You're making a sensible choice so
   that you can keep working, but some of these questions belong to someone
   else. Say who you would ask, and what you would ask them.

Keep entries short. One or two sentences per field is plenty.

---

## Template

### D? – Short name for the decision

- **Situation:** What happened that the ticket doesn't cover?
- **What the ticket says:** Quote it, or write "Nothing".
- **Options considered:** The choices you could have made.
- **Decision:** What the code does.
- **Why:** Your reasoning.
- **Test:** The name of the test that checks it.
- **Who should decide:** Who you'd ask, and the question you'd put to them.

---

## Decisions

### D1 (Example) – Extra spaces inside a name

- **Situation:** The spreadsheet has a line with **two** spaces between
  "Neon" and "Saints":
  >
  ```
  Neon  Saints - Overdrive
  ```
  > With each space shown as `␣`, that's `Neon␣␣Saints␣-␣Overdrive`.
- **What the ticket says:** Nothing. The spec says whitespace _around_ the
  artist and title is ignored, but doesn't mention spaces inside them.
- **Options considered:**
  - Keep the name exactly as written: `Neon␣␣Saints`
  - Squash repeated spaces down to one: `Neon␣Saints`
- **Decision:** Names are kept exactly as written. Only leading and trailing
  spaces are removed.
- **Why:** It's the simplest rule, and changing a name could break a real one
  that's meant to look unusual.
- **Test:** `SongParse.KeepsSpacesInsideNames`
- **Who should decide:** The audio team. If `Neon␣␣Saints` and `Neon␣Saints`
  both appear in the spreadsheet, are they the same artist? With this
  decision, those two songs won't be equal.
