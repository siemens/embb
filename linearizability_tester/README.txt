Linearizability tester on EMBB data structures
----------------------------------------------

This README.txt gives information on how to compile the linearizability test
and how to implement tests for other future data structures. 

The code in this folder is meant to check the linearizability of the
data structures implemented in the EMBB library. The original linearizability
checker was developed by Alex Horn (Oxford) and the original source code is 
freely downloadable from the public github repository:

--> https://github.com/ahorn/linearizability-checker

main.cc is the script that runs the test on the structures currently available.
The src folder contains a large part of the code by Alex Horn.

COMPILE INSTRUCTIONS

* On Linux system
Suppose that the embb library was build in ../build (relative path to the current
folder of the linearizability test).
From within the linearizability_tester folder, cd (or mkdir->cd) into the build directory,
then run "cmake .." and "make". The executable of the test should be placed in the
same folder with the name EMBB_linearizability_test.

* On Windows system
Suppose that the embb library was build in ../build (relative path to the current
folder of the linearizability test).
NOTE: the embb library should be build in Release mode, i.e. by running the command
"cmake --build . --config Release" from inside the correct directory.
From within the linearizability_tester folder, cd (or mkdir->cd) into the build directory,
then run "cmake .." and "cmake --build . --config Release". The Visual Studio prompt should
be used.
The executable of the test should be placed in the folder Release 
with the name EMBB_linearizability_test.exe.

HOW TO IMPLEMENT TESTS FOR OTHER DATASTRUCTURES

Data structures that support the following functions: 