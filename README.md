# Kinoko

Kinoko is a reimplementation of Mario Kart Wii's physics engine in C++. This project is a spiritual continuation of [stblr/Hanachan](https://github.com/stblr/Hanachan). Like Hanachan, the goal of this reimplementation is to reach perfectly accurate ghost replay.

## Building

### Dependencies

- g++ 12 (C++23)
- ninja
- Python 3

### Process

Generate the ninja file:

```bash
./configure.py
```

Execute it:

```bash
ninja
```

## Interfacing

While a GUI is not planned for the project at this time, contributors are welcome to add a graphics frontend under three conditions. The license must not change, it does not interfere with the CLI, and most importantly, it does **not** distribute any in-game assets.

## Contributing

The codebase uses C++ for the engine and Python for any external scripts.

Pull requests resolving an issue or element of a tracking issue should reference the issue or item in the description.

Any commits should be formatted using the project's clang-format file.

## Resources

- [MKW-SP Discord](https://discord.gg/TPSKtyKgqD) - Request Ghidra server access and chat with contributors.
- [Matching Decompilation](https://github.com/riidefi/mkw) - A repository that compiles back into the game's original assembly.
- [Tockdom](http://wiki.tockdom.com/wiki/Main_Page) - A wiki maintained by multiple community members, most notably used for file formats.
