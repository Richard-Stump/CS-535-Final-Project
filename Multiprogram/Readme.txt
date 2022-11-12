This project stucture now supports multiple programs and putting them all onto
the disc. 

The structure is as follows:

Common/             Code that is used by all programs. The source files here are
                    compiled into libcommon.a, which can be linked to all of the 
                    other parts of this project. The headers should be added to
                    the global include, and can be accessed with 
                        #include <PS2Pad.hpp> 
                    instead of having to use relative pathing

Startup/            This folder contains the program that can load other
                    programs on the disc. Currently it loads the cubes demo

Programs/           This folder contains all the other programs that can be run.
                    Each program is put into a subfolder with it's name, and
                    provides it's own makefile that produces the .elf file(s)
                    to be included on the disc

    Cubes/          The milestone 1 program
    Lba3Richard/    Richard's attempt at porting lab3 

Resources/          These are resource files that are put onto the disc rather
                    than compiled into the program. I think that they should
                    be accessable with the standard c/c++ file operatations, but
                    I have yet to try this.

out/                Contains the .iso file with all the programs and reosuces in
                    it.

The makefile in this directory can be used to build the whole project. Here are
the commands for this makefile:

make:               Will build the entire project (make all)
make iso:           Creates the .iso file
make startup:       Builds the startup program
make common:        Builds libcommon.a
make programs:      Builds each of the program in the programs/ folder
make clean:         Cleans the entire project

Since each of the programs provides its own makefile, you can customize the
makefile to each specific program. They just need to produce one or more .elf
binaries to be included on the disc. 