# My Shell

## Description
A very simple implementation of a Unix shell written in C.  Only a few Unix commands are implemented at this time. See **Supported** for further details.

## Purpose
The purpose of this project was to practice implementing some of the more popular Unix commands, work with a range of dynamically allocated types and structures, and utilize basic multiprocessing to handle user requests on the file system.  Implemented commands may not support the full range of flags available in other shells as each program is a work in progress. Commands are often implemented to accomplish their basic function and later revisited to restructure the operations, provide further support of flags, or both. 

## Use
**shell** can be run from the same directory that the executable appears in.  This will become the new "root" directory and during execution can be referenced using '/' as you would with the actual root.  Starting **shell** from a directory other than the one it currently is in will fail to properly set needed environement variables and will ultimately cause program failure.

## Supported
- All commands support absolute/relative pathnames (if applicable)

### exit / quit
- Exit the shell (return)

### cd - change directory
- Can reference current directory using "." and parent directory using ..
- No arguments ("cd ") will display the same result as "cd ."
- The path arguments can be a series of directories (e.g. foo/bar/)
- Gracefully denies accessing directories "above" the directory of initial execution

### ls - display directory contents
- "-a" shows entries that begin with "."
- "-l" use long listing (the total number provided is the number of 512B blocks allocated, which differs from the tradition 1024B block count)
- "-h" provides command line help

### pwd - print working directory
- prints the current working directory

### mv - move
- Supports renaming a file
- Supports moving a file to a differt directory

### rm - remove
- Supports removal of files

### touch - create a file
- Updates the access and modification times of a file to the current time
- Creates a new file if the specified name does exist
- "--help" provides command line help

### cp - copy
- Supports copying a file to a new name
- Supports copying a file to a new directory
- Copies original permissions to new file
- "-h" provides command line help


## Warning
Although no programs contain memory leaks due to failed dynamic memory deallocation, many initial implementations use buffers which can overflow given lengthy user input causing unpredictable behavior. Programs are revisted to prevent errors of this caliber, but the user should be aware that this project is a work in progress.
