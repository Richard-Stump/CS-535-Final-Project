This folder is the reorganized project to allow multiple programs for the PS2

The structure is as follows:

Common/             Files that are used in all programs.
Startup/            The main program. This is the program that is loaded from
                    the .iso file and can load the other .elf files on the disc
Programs/           This folder contains folders for the other programs
    Cubes/          The cubes program for checkpoint 1
    Lab3/           PS2 implementation of Lab3
    Slideshow/      The presentation for the project

Resources/          Files that are to be coppied verbatum into the iso


The startup folder and each of the program subfolders are responsible for
their own makefiles. They should output .elf files, and the elf files are then
put into the .iso produced.