# Saving Starman

A tile-based adventure game for an ARM mbed LPC1768 microcontroller. It runs
on a 128×128 uLCD with tilt controls from an accelerometer, three
pushbuttons, and PWM music, all in C++ with no engine. I built it for
Georgia Tech's ECE 2035. You're a SpaceX Dragon capsule,
Starman's Roadster is out of battery, and Mars is surrounded by asteroids.

<p align="center">
  <a href="https://youtu.be/H5QuszITFQo">
    <img src="https://i.ytimg.com/vi/H5QuszITFQo/hqdefault.jpg" alt="Saving Starman trailer on YouTube" width="480">
  </a>
</p>

<p align="center"><a href="https://youtu.be/H5QuszITFQo">Watch the trailer</a></p>

## Why

It was a project for my embedded systems class at Georgia Tech, ECE 2035.
On the last day of class it was featured as one of the best games.

This was Project 2 of the course. The mbed export is named `P2-2`, and part
2-1 was the hash table the game runs on. The course supplied a skeleton (see
Credits). The theme is SpaceX's Starman: he's stranded in space, his
Roadster needs a battery, and the only one is in the Tesla Gigafactory back
on Earth.

The code dates from spring 2019: the hash table file is dated March 2019 and
the mbed export is stamped April 2019. It was uploaded to GitHub in 2023 as a
single commit.

## How it plays

1. **Title screen.** A solar system drawn entirely from `filled_circle`
   calls: a striped sun centered just off the left edge, eight planets in a
   row, and a single `line` for Saturn's ring. Press Action to start.
2. **Space (50×50).** You fly a Dragon capsule around a starfield. Starman
   asks for a battery. Mars is walled in by a 5×5 asteroid field that the
   Dragon "can't maneuver through."
3. **Earth → Gigafactory (21×21).** Interacting with Earth swaps to a second
   map, a hand-built maze. Two floor buttons each open a door, and the
   battery sits in the far corner. Grabbing it drops you back in space where
   you left.
4. **Back to Starman.** Hand over the battery, and he takes you along in the
   Roadster. His tile turns into your empty Dragon capsule.
5. **The asteroids.** The Roadster "has a secret power": pressing Action next
   to an asteroid blows up that tile (explosion sprite for 250 ms, then
   erased). Blast a path in and land on Mars.

There's also an easter egg at coordinates (20, 35). The world generator keeps
the 3×3 area around it (`area2035`) clear, and pressing Action there gets you
an NPC asking you to "tell Schimmel I said hi" when you get back to Earth.

## How it works

### Hardware

| Part | Pins | Notes |
|---|---|---|
| mbed LPC1768 (Cortex-M3) | | 512 KB flash, 32 KB main SRAM + 32 KB AHB SRAM per the linker config |
| 4D Systems uLCD, 128×128 (Goldelox) | p9 / p10 / p11 | serial, driven at 3 Mbaud |
| MMA8452 accelerometer | p28 / p27 | I2C, tilt to move |
| 3 pushbuttons | p21 / p22 / p23 | Action / Menu / Omni, internal pull-ups, active-low |
| Speaker | p26 | PWM tone generation |
| USB serial | | 115200 baud debug console |

The course's hardware file also sets up an SD card and a DAC-based
`wave_player`. The game never uses them.

### Game loop

`main()` reads like the course skeleton intended:

```
read_inputs() → get_action() → update_game() → draw_game() → sleep to 100 ms
```

A `Timer` measures each iteration and `wait_ms` pads the rest. The game runs
at a fixed 100 ms tick, so no more than 10 frames per second and no more
than one tile of movement per frame. `update_game` returns a result code.
`FULL_DRAW` forces a complete redraw after dialogue. `EXP` runs the asteroid
explosion, `MAZE` toggles a button and opens its door, and `GAME_OVER` frees
both maps and shows the ending screen.

### Input

Buttons are polled every frame and win over movement: Action, then Menu, then
Omni. Movement comes from the accelerometer. Tilt past ±175 counts on an
axis and you move one tile that way, but only if the neighboring tile is
walkable (or Omni mode is on).

The Menu screen toggles two things:
- **Music on/off.**
- **"Inverted" accelerometer.** This swaps the axis mapping for holding the
  board rotated 90°, and a red warning screen tells you which way to turn it.

**Omni** (button 3) lets you walk through walls and shows `OMNI` in the
status bar. It's mostly for testing.

### The map is a hash table

The world isn't a 2D array. Each map is a chained hash table of `MapItem`s.
I wrote the hash table for an earlier part of the course (Project 2-1). Only
occupied cells are stored, so empty space is just a lookup that returns
`NULL`.

- **Key.** `XY_KEY` packs `x` into bits 8–15 and `y` into bits 0–7. That's
  unique for any map up to 256×256.
- **Hash.** `key % 127`, with 127 buckets (a prime).
- **Items.** Each `MapItem` holds a `type`, a `walkable` flag, a `void*` for
  extra data, and a **draw function pointer**. The renderer never switches
  on type, it just calls `item->draw(u, v)`. Changing how something looks
  means reassigning that pointer. That's how buttons light up
  (`draw_button_on`), asteroids explode (`draw_explosion`), and Starman's
  tile becomes an empty capsule (`draw_emptydragon`).
- **Two maps** (`mainmap` 50×50 and `gigamap` 21×21) sit side by side, and
  `set_active_map()` decides which one every accessor reads. Entering the
  Gigafactory saves your position. Leaving restores it.

**World generation.**
- **Stars** go on every 39th cell. They're "random" in quotes in the source,
  too.
- **Planets and players.** Mars (3×3 inside its 5×5 asteroid ring), Earth
  (3×3), Starman and the player are placed with `rand()`, re-rolling on
  overlap.
- **The seed** is `tmain.read_ms()`, a timer started at boot and read *after*
  you press Start. The galaxy layout comes from how long you sat on the title
  screen.

`print_map()` dumps the active map as ASCII over USB serial, which is handy
for seeing the whole world at once.

### Drawing

The screen shows an 11×9 window of 11×11-pixel tiles, with the player always
in the center. Status bars sit above and below: X/Y coordinates on top, the
mode on the bottom.

**Sprites are strings.** Each one is a 121-character literal, one character
per pixel in row-major order, so the art is visible (sort of) right in
`graphics.cpp`. `draw_img` translates the characters into colors through a
small palette:

- the course defaults (`R`, `Y`, `G`, `D`, `5`, `3`)
- my own additions: two SpaceX blues, Dragon blues, Mars rust, Earth blue
  and green, two star yellows, orange and white

`B` isn't in the palette, so it falls through the final `else` to black,
which is how "B for black" works. The colors fill a 121-`int` stack buffer
that goes to the LCD in one `BLIT`, followed by `wait_us(250)` (the source
comment says "Recovery time!").

**Only redraw what changed.** Every frame, `draw_game` compares the
`MapItem*` at each screen cell's current map position with the one at its
previous position, and only draws when the pointers differ. Flying through
empty space mostly repaints the stars as they slide past instead of all 99
tiles. Only dialogue, the menu and map changes force a full redraw.

**Dialogue** is two lines at a time in a box above the bottom status bar.
`long_speech` pages through longer messages in pairs while "Press Action"
blinks on a 250 ms cycle.

### Audio

`SongPlayer` drives the speaker straight from a `PwmOut`. For each note it
sets the period to `1/f` and the duty cycle to `volume/2`, then arms a
`Timeout`. When the timeout's interrupt fires, it moves to the next note.
A zero duration marks the end of the song, and at that point it calls
`PlaySong` again, so the theme loops forever from interrupt context while
the game loop goes about its business.

The tune is five notes: G4, F4, E4 at 0.96 s each, then F3, E3 at 0.48 s.
Turning music off just stops the next interrupt from re-arming.

### Memory

The LPC1768 has 32 KB of main RAM and a 4 KB boot stack, so a few choices
here are about staying small:

- **Sparse maps.** Only walls, stars, planets and items are allocated,
  a few hundred `malloc`'d `MapItem`s across both maps. Empty space costs
  nothing.
- **Sprites stay in flash.** They're `const char*` string literals. The
  only per-draw RAM is that 484-byte color buffer, which lives on the stack
  for a single `BLIT`.
- **Clean teardown.** `maps_deinit()` frees every item and both tables at
  game over, so a replay starts from a clean heap.
- **Size-optimized.** All build configurations compile with `-Os`.

### Known quirks

It's a 2019 class project, and the code shows it in a few places:

- **The empty-space walkability check reads a NULL pointer.** It does
  `north->walkable` even when `north` is `NULL`. On the LPC1768, address 0
  is readable flash (the vector table), so this reads a nonzero interrupt
  vector and treats empty space as walkable. It works, for reasons that
  should not be relied on.
- **Overlap re-rolls don't fully work.** The placement loops overwrite their
  `collision` flag on every comparison, so only the last check counts. Mars'
  footprint array is also shadowed inside its loop, so later checks compare
  against an uninitialized copy.
- **Replays recurse.** After the ending screen, the game calls `main()`
  again, so each playthrough nests one more frame on that 4 KB stack.
- **`deleteItem` always prints an error.** It logs
  `ERROR:Hash Table item DNE` to the console on every call, successful or
  not.

## Building it

This is an mbed 2 ("classic") project exported from the mbed online
compiler to **GNU ARM Eclipse**. The project is named `P2-2`. The repo
includes the prebuilt `libmbed.a` for `TARGET_LPC1768` / `TOOLCHAIN_GCC_ARM`
plus the library sources, so nothing needs to be fetched.

1. Install the GNU Arm Embedded toolchain (`arm-none-eabi-gcc`) and Eclipse
   with the GNU MCU/ARM Eclipse plugins.
2. Import the folder as an existing project. `.project`, `.cproject` and
   `makefile.targets` define Debug, Develop and Release configurations.
3. Build. The output is `P2-2.elf` plus a raw `.bin`.
4. Copy the `.bin` onto the mbed's USB drive and press reset.

The `.lib` / `.bld` files point at mbed.org URLs that may not resolve
anymore. That's fine, because the repo already vendors everything the build
needs.

## Lessons learned

1. **Draw less.** The LCD hangs off a serial link, so the cheapest pixel is
   the one you never send, and skipping tiles whose pointer didn't change is
   the main rendering optimization.
2. **Function pointers beat switch statements.** Giving every map item its
   own draw function made "change how this looks" a one-line pointer swap.
3. **A sparse world is a hash table.** Storing only occupied cells kept
   memory proportional to what's actually on the map.
4. **Undefined behavior can pass the demo.** Dereferencing `NULL` "worked"
   because address 0 happens to be readable on this chip, and that is not
   the same as correct.
5. **Seed with the human.** Timing how long the player waits on the title
   screen gave a different galaxy every game, without any entropy hardware.

## Repo layout

```
main.cpp            game loop, input → action mapping, quest logic, world generation
map.cpp / map.h     two maps on hash tables, MapItem types, add_* builders
hash_table.cpp/.h   chained hash table (ECE 2035 Project 2-1)
graphics.cpp/.h     string sprites, palette, status bars, border
speech.cpp/.h       dialogue boxes
songplayer.h        interrupt-driven PWM music
hardware.cpp/.h     pin assignments, input polling (course-provided)
globals.h           shared hardware objects, ASSERT_P
4DGL-uLCD-SE/       uLCD driver library
MMA8452/            accelerometer driver
SDFileSystem/       SD card + FatFs (initialized, unused)
wave_player/        DAC audio player (initialized, unused)
mbed/               mbed 2 SDK headers + prebuilt LPC1768 library
```

## Credits

**Course-provided** (the code's own comments say so):
- the project skeleton: `globals.h` (headed "Spring 2018 Gatech ECE2035")
  and `hardware.cpp`'s pin setup
- the outline of the game loop in `main()`, and (judging by its
  template-style comments) the tile-iteration / changed-tile structure of
  `draw_game`
- the public interfaces in `map.h`, `speech.h` and `hash_table.h`
- `createHashTable`, which is marked "provided for you as a starting point"

**Mine:**
- the rest of the hash table, plus the key packing and hash function
- all the sprites and the extended palette
- the story, quests and dialogue
- both maps and the world generation
- the menu (music toggle, accelerometer inversion) and Omni mode
- the title and ending screens
- the music player wiring
- the asteroid, button and door mechanics

**Libraries:**
- [mbed SDK](https://os.mbed.com/)
- `SongPlayer`, from Jim Hamblen's mbed cookbook speaker example
- 4DGL-uLCD-SE (4D Systems driver, modified for Goldelox by Jim Hamblen)
- MMA8452 (published under the `ece2035ta` mbed account)
- wave_player
- SDFileSystem with ChaN's FatFs
