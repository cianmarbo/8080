<p align="center">
  <img src="logo.png" alt="8080 logo" width="480">
</p>

# 8080

An emulator for the **Intel 8080**, the 8-bit microprocessor Intel
released in 1974, built specifically to run the original 1978 *Space Invaders*
arcade game.

Space Invaders ran on 8080 hardware, so emulating the CPU — along with the
cabinet's video interrupts and memory-mapped display — is enough to bring
the game back to life. This project implements the 8080 instruction set in
C99 and drives an SDL2 window that renders the game's video memory.

## Status

**Working:**
- Most of the 8080 instruction set, decoded through the `execute()` loop
- An SDL2-backed frame loop timed to the ~2 MHz CPU clock
- The two per-frame video interrupts (RST 1 mid-screen, RST 2 VBlank)
- Rendering the 1-bit-per-pixel framebuffer from VRAM (`0x2400`) to the window

**Not yet done:**
- Hardware I/O ports (`IN`/`OUT`), including the bit-shift register
- Sound and player input

## Requirements

- A C99 compiler (`gcc` or `clang`)
- SDL2 — `brew install sdl2` on macOS, or `libsdl2-dev` on Debian/Ubuntu

## Build & Run

The repo holds two independent programs, each with its own Makefile.

### Emulator

```sh
cd emulator
make build        # produces ./8080
./8080 <rom1> <rom2> <rom3> <rom4>
```

Space Invaders originally shipped across four ROM chips
(`invaders.h`, `invaders.g`, `invaders.f`, `invaders.e`); the emulator
loads four files consecutively into memory, so pass all four in order. Run
it from inside `emulator/`, since it reads `intro.txt` from the working
directory.

Flags:
- `--dumpregisters` — print register / PC / SP state after the window closes
- `--about` — print version and build date

### Disassembler

A standalone tool that prints a full disassembly listing of a ROM.

```sh
cd disassembler
make build             # produces ./disasm
./disasm <romfile>     # disassembly listing with addresses
./disasm <romfile> -v  # also print the ROM file size
```

## Project layout

```
emulator/src/
  cpu.{c,h}      CPU state, instruction set, execute loop, interrupts
  display.{c,h}  SDL window, framebuffer, video rendering
  rom.{c,h}      ROM file loading
  main.c         entry point, frame loop, CLI args
  disasm.{c,h}   opcode-to-mnemonic table (trimmed copy of the standalone tool)
disassembler/    the standalone disassembler
```

## Resources

These references were invaluable:

- [Intel 8080 opcode reference table](https://www.pastraiser.com/cpu/i8080/i8080_opcodes.html).
- [Space Invaders hardware notes](https://www.computerarcheology.com/Arcade/SpaceInvaders/Hardware.html).
- [Intel 8080 Programmer's Manual (PDF)](https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf).

## License

Released under the [MIT License](LICENSE). Built by cianmarbo.
