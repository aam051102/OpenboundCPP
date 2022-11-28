# C++ Openbound engine
## WORK IN PROGRESS
A C++ port of the [Openbound engine](https://github.com/WhatPumpkin/Sburb) by WhatPumpkin. No affiliation.

## Setup
Clone repository.
Install Conan on the device.
Navigate via terminal to the cloned repository and execute `conan install . --build missing -s compiler.runtime=MTd`.
Then download Premake5, copy premake5.exe to the root directory and execute `premake5 vs2022`.

Example data can be found in the official Openbound engine repository or in the official Openbound game repository. To use the data, copy "levels" and "resources" and place them in the same directory as the built executable. Alternatively, copy the executable into the folder that has both "levels" and "resources".

## TODO

- Possibly integrate Lightspark or some other Flash emulator to support "movies".