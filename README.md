<div align="center">
  <h1>Kinoko</h1>
  <img src="https://github.com/vabold/Kinoko/assets/16770560/0e4ef63a-409a-40e4-8801-d30a27d7f7b5" width=100>

  A reimplementation of Mario Kart Wii's physics engine in C++.

[![Discord](https://img.shields.io/badge/Discord-grey?logo=discord)](https://discord.gg/qjt66ptQde)
[![Doxygen](https://img.shields.io/badge/📚%20Docs-grey)](https://vabold.github.io/Kinoko/docs/html/index.html)
</div>
<hr/><br/>


Kinoko is a reimplementation of Mario Kart Wii's physics engine in C++. This project is a spiritual continuation of [stblr/Hanachan](https://github.com/stblr/Hanachan). Like Hanachan, the goal of this reimplementation is to reach perfectly accurate ghost replay.

To see the current list of test cases and progress, visit [STATUS.md](STATUS.md).

## Building

### Dependencies

- g++ 13.1 (C++23)
- ninja 1.3
- Python 3.6
- Mario Kart Wii disc

### Extracting Files

Kinoko relies on files from the base game to run, with the same paths relative to the executable. Dolphin is the preferred method of extracting these files.

1. Make sure Mario Kart Wii is in the Dolphin game list. If it's not, click on Config, navigate to the Paths tab, and click "Add...". Find the folder Mario Kart Wii is in, and if needed, close out of the Config menu and click "Refresh".
2. Right click Mario Kart Wii in the game tab, and click "Properties".
3. Navigate to the "Filesystem" tab using the arrow keys in the top right of the properties window.
4. If "Data Partition (0)" is not already open, click the arrow next to the name to open it.
5. Right click on the "Race" folder, and click "Extract Files...".
6. Select the folder the Kinoko executable is placed in. For developers, this is most commonly in the "out" folder after building the project.

### Process

Generate the ninja file and test case binary:

```bash
./configure.py
```

Execute it:

```bash
ninja
```

Run Kinoko:

```bash
cd out
./kinoko -m test -s testCases.bin
```

## Creating New Test Cases

Currently, Kinoko runs by iterating over a set of test cases defined in `testCases.json`.

### Create KRKG

Test cases use a custom `.krkg` file format, which stores relevant in-game memory values to validate accuracy on every frame of a race. This file is generated using a custom version of MKW-SP.

1. Navigate to the [workflows](https://github.com/vabold/mkw-sp/actions?query=branch%3Akrkg) page for the KRKG branch of [vabold/mkw-sp](https://github.com/vabold/mkw-sp).
2. Download the `mkw-sp-test` artifact at the bottom of the page.
3. Extract `boot.dol` to a folder of your choosing.
4. In Dolphin, navigate to Config > Paths and set `Default ISO` to the path of your Mario Kart Wii PAL ISO.
5. In Dolphin, click Open and select the extracted `boot.dol`.
6. You should see some debug information printed on-screen and Mario Kart Wii should boot soon after.
7. Watch a ghost replay of your choosing. After the ghost finishes and the screen fades to black, navigate to `[DolphinDir]\User\Wii\title\00010004\524d4345\data`. You should see a new file called `test.krkg`.

### Add Test Case Params

Test cases are defined in `testCases.json` in the following format:

```json
"testCaseName": {
    "rkgPath": "pathTo.rkg",
    "krkgPath": "pathTo.krkg",
    "targetFrame": 0
}
```

`rkgPath` is the path to the game's native ghost file for the time trial you want to simulate. `krkgPath` points to the krkg file generated in the previous section for this time trial ghost. `targetFrame` is the number of frames to attempt sync for this ghost. Kinoko will throw an error if it is larger than the framecount stored in the krkg. If Kinoko reaches `targetFrame` while maintaing sync, then the test case will pass.

To update the test binary, be sure to re-run:

```bash
./configure.py
```

## Interfacing

While a GUI is not planned for the project at this time, contributors are welcome to add a graphics frontend under three conditions: the license must not change, it does not interfere with the CLI, and most importantly, it must **not** distribute any in-game assets.

## Integrate Kinoko to your project
You can link Kinoko to your own project using CMake by linking with the `libkinoko` target. For example:
```cmake
# add_subdirectory(kinoko)

add_executable(example main.cpp)
target_link_libraries(example libkinoko)
```
Note that any project using Kinoko's include directories will also require C++23.

## Contributing

The codebase uses C++ for the engine and Python for any external scripts.

Pull requests resolving an issue or element of a tracking issue should reference the issue or item in the description.

Any commits should be formatted using the project's clang-format file.

## Resources

- [Kinoko Discord](https://discord.gg/qjt66ptQde) - Discuss development, offer suggestions, and more!
- [MKW-SP Discord](https://discord.gg/TPSKtyKgqD) - Request Ghidra server access and chat with contributors.
- [Matching Decompilation](https://github.com/riidefi/mkw) - A repository that compiles back into the game's original assembly.
- [Tockdom](http://wiki.tockdom.com/wiki/Main_Page) - A wiki maintained by multiple community members, most notably used for file formats.
