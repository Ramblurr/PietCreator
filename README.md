Piet Creator
-------------------------------------------------------------------------------

[![Build Status](https://travis-ci.org/Ramblurr/PietCreator.svg?branch=master)](https://travis-ci.org/Ramblurr/PietCreator)

Piet Creator is a cross platform GUI IDE for the Piet esoteric programming
language. It includes a graphical editor, embedded interpreter, and integrated
debugger.

"Piet is a programming language in which programs look like abstract
paintings. The language is named after Piet Mondrian, who pioneered the
field of geometric abstract art. " - David Morgan-Mar. dmm@dangermouse.net (Piet Author)

Piet Creator's primary author is Casey Link <unnamedrambler@gmail.com>.

The interpreter backend is provided by npiet, a piet interpreter written in C by
Erik Schoenfelder.

piet, the language: http://www.dangermouse.net/esoteric/piet.html
npiet: http://www.bertnase.de/npiet/

Piet Creator is licensed under the GPL v3, and is written in C++ with Qt.

Piet the esoteric programming language was created by David Morgan-Mar <dmm@dangermouse.net>,
and is copyright by him.

Compiling / Running
----------------
Prerequisites for Linux/Mac OS X/Windows:

* Qt >= 4.6
* libPNG  - http://www.libpng.org/pub/png/libpng.html
* GIFLIB
* GD Graphics Library - http://www.boutell.com/gd/
* CMake - http://www.cmake.org

Piet Creator uses the cmake build system, which is supported on all major
operating systems.

On Linux/Mac OS X

From the source directory:
$ mkdir build
$ cd build
$ cmake ../   # This will attempt and find all the dependencies
$ make
$ ./pietcreator


On Windows (With VS 2008)

Create the build directory as shown above
Run the same cmake command "cmake ../" or "cmake path_to_sources"
Open the resulting .sln in Visual Studio
Execute the pietcreator.exe binary in Debug/

On Windows (With mingw)

Unknown. Anyone care to contribute?
You will need the Qt SDK for windows that includes mingw from:
http://qt.nokia.com/downloads/sdk-windows-cpp
