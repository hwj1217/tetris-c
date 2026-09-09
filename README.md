[繁體中文](README.zh-TW.md)

# Terminal Tetris in C

A playable Tetris clone for the terminal, written in C with ncurses. Built as a practice project after working through dynamic memory, structs, memory layout, and multi-file builds in C.

## Features

- **Piece movement & collision detection** — move left/right/down, blocked by walls and stacked pieces
- **Automatic gravity** — pieces fall on a timer, with manual soft drop
- **Rotation** — 4 rotation states per piece (SRS-based coordinates)
- **Line clearing** — full rows are cleared, with rows above shifted down
- **Random piece spawning** — a new piece of a random type spawns at the top
- **Scoring & display** — 100 points per cleared line, shown live next to the board
- **Color-coded pieces** — each piece type is rendered in its own color
- **Game over detection** — the game ends when a new piece cannot spawn

## Build & Run

### Dependencies

This project requires the ncurses development library.

On Debian/Ubuntu (including Kubuntu):

```bash
sudo apt install libncurses-dev
```

### Build

```bash
gcc -Wall -Wextra -o tetris main.c -lncurses
```

### Run

```bash
./tetris
```

### Controls

| Key | Action |
|-----|--------|
| `←` | Move left |
| `→` | Move right |
| `↓` | Soft drop (move down one row) |
| `↑` | Rotate clockwise |
| `q` | Quit the game |

## Architecture

### Single source of truth

All rendering and collision detection read from a single `board[20][10]` array. The falling piece is written into `board` on every move, so the array always reflects the true game state.

An earlier version drew pieces directly from the `all_pieces` lookup table while collision checks read from `board`. With two separate sources, any missed update leaves what the player sees out of sync with what the game logic decides — a class of bug that is hard to spot because the code compiles and mostly looks correct.

### Base point + relative offsets

Each piece is stored as a single base `Point` plus four relative offsets looked up from `all_pieces[type].rotations[rotIndex]`. Moving the piece one cell updates only `base.row` and `base.col` — 2 numbers instead of the 8 that four absolute coordinates would require.

Fewer numbers to maintain by hand means fewer chances to get one wrong. An early bug in `clear_piece` came from exactly this: a copy-pasted line used `base.row` where it should have used `base.col`.

## Debugging log

These are bugs that compiled cleanly but behaved incorrectly — the kind that only surface when you actually watch the program run.

### 1. New pieces invisible for one frame after spawn

**Symptom.** After a piece locked, the next piece did not appear until one tick later, and when it did appear it was already one row below its spawn position.

**Cause.** The spawn branch assigned a new `Fall` to `game.current` but never called `place_piece`, so the new piece existed in the game state without being written into `board`. Since all rendering reads from `board`, there was nothing to draw.

**Fix.** Call `place_piece` immediately after assigning the new piece.

**Takeaway.** Every branch that changes `current` must also update `board`. The three movement branches already followed a `clear_piece` → update → `place_piece` pattern; the spawn branch broke it by doing only the middle step.

### 2. Pieces stopped falling entirely after adding stack collision

**Symptom.** After adding the `board[row][col] != EMPTY` check to `is_valid_position`, pieces stopped falling completely — not stuck partway down, but unable to move even one row. Every tick was judged invalid, so a new piece spawned immediately while the old one sat frozen in place.

**Cause.** The gravity branch called `is_valid_position` *before* `clear_piece`, the reverse of the three movement branches. At the moment of the check, the piece's own four cells were still written in `board`, and the new occupancy check could not tell "this cell belongs to another piece" from "this cell is where I currently am." A T piece at `base = (5, 5)` occupies `(5, 6), (6, 5), (6, 6), (6, 7)`; moving down one row would occupy `(6, 6), (7, 5), (7, 6), (7, 7)`. The shared cell `(6, 6)` made the move invalid every time.

**Fix.** Reorder the gravity branch to `clear_piece` first, matching the other three. The invalid path then needs an extra step: call `place_piece` once to redraw the *old* piece where it was — this is what locking actually is — before switching `current` to the new piece and drawing that.

```c
clear_piece(&game);
if (is_valid_position(&game, game.current.base.row + 1, game.current.base.col, game.current.rotIndex)) {
    game.current.base.row += 1;
    place_piece(&game);
}
else {
    place_piece(&game);              // lock the old piece
    game.current = (Fall){ ... };    // then spawn
    place_piece(&game);
}
```

**Takeaway.** The same function can return different answers depending on when it is called. `is_valid_position` was never wrong; it was being asked the question while `board` still contained the piece doing the asking.

### 3. Only half of the consecutive full rows were cleared

**Symptom.** With four consecutive rows full, `clear_lines` removed only two of them. The other two stayed on the board.

**Cause.** The scan used `for (int row = 19; row >= 0; row--)`, which decrements `row` on every iteration. But clearing a row shifts everything above it down by one, so after handling row 19, `board[19]` holds what used to be row 18 — fresh data that has never been checked. The loop had already moved on to row 18 and would never look back.

**Fix.** Replace the `for` loop with a `while` loop that controls the decrement by hand. If the row is not full, move up; if it is full, clear it and stay on the same index so the newly shifted-in row gets checked too.

```c
int row = 19;
while (row >= 0) {
    // ... check whether this row is full ...
    if (isFull) {
        // ... shift everything above down by one ...
        lines_cleared += 1;
        // no row-- here: recheck the same index
    }
    else {
        row--;
    }
}
```

**Takeaway.** A `for` loop's automatic increment assumes that once an iteration finishes, that index is done. When the loop body modifies the data it is iterating over, that assumption breaks. The same trap shows up when removing elements from an array or invalidating iterators in C++.

## Not implemented

Known gaps compared to a standard Tetris implementation:

- **Wall kick** — rotation near a wall or against the stack simply fails instead of nudging the piece aside
- **Next piece preview** — the upcoming piece is not shown
- **Levels & variable drop speed** — gravity is fixed at 500 ms regardless of score
- **Hard drop** — only soft drop (`↓`) is available
- **Hold** — no way to stash the current piece for later
- **7-bag randomizer** — pieces are drawn with plain `rand() % 7`, so the same type can repeat several times in a row
