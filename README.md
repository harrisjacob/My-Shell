# My Shell

## Description
A very simple implementation of a Unix shell written in C.  Only a few Unix commands are implemented at this time. See **Supported** for further details.

## Purpose
The purpose of this project was to practice implementing some of the more popular Unix commands, work with a range of dynamically allocated types and structures, and utilize basic multiprocessing to handle user requests on the file system.  Implemented commands may not support the full range of flags available in other shells as each program is a work in progress. Commands are often implemented to accomplish their basic function and later revisited to restructure the operations, provide further support of flags, or both. 

## Use
**shell** can be run from the same directory that the executable appears in.  This will become the new "root" directory and during execution can be referenced using '/' as you would with the actual root.  Starting **shell** from a directory other than the one it currently is in will fail to properly set needed environement variables and will ultimately cause program failure.

## Supported