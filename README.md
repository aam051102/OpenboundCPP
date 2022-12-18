# C++ Openbound engine
A C++ port of the [Openbound engine](https://github.com/WhatPumpkin/Sburb) by WhatPumpkin. No affiliation.
This project is fully usable for the Openbound games, as far as I'm aware (with the exception of saving and loading). Please report any issues you come across.

NOTE: There is not currently a way to define the initialization XML file, meaning that is must be in /levels/openbound/openbound.xml.

## Usage
1. Copy `/levels` and `/resources` directories to the same place as the executable (and any other necessary files).
2. Run the executable.
3. You will be presented with a file selector dialog. Find the initialization XML file for the game. If the game abides by good practice, it will be in `/levels` and will probably be named either `init.xml` or the name of the project. For Openbound part 1, for example, it is `/levels/openbound/openbound.xml`.
4. Profit!

## Setup
Clone repository.
Install Conan on the device.
Navigate via terminal to the cloned repository and execute `conan install . --build missing -s compiler.runtime=MTd` (downloads debug modules).
Then download Premake5, copy premake5.exe to the root directory and execute `premake5 vs2022`.

Example data can be found in the official Openbound engine repository or in the official Openbound game repository. To use the data, copy "levels" and "resources" and place them in the same directory as the built executable. Alternatively, copy the executable into the folder that has both "levels" and "resources".

## TODO

- Allow user to change init XML file.
- Add save/load support.
- Search for "CheckIsLoaded", uncomment everything and implement it.
- (Plausibly) Integrate Lightspark or Gnash or some other Flash emulator to support "movies" (i.e. Flash)?
- Bug fixes, optimizations, etc.

Contributions are welcome via pull requests or issues.