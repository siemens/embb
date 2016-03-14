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
We suppose that the embb library was build in ../build (relative path to the current
folder of the linearizability test).
From within the linearizability_tester folder, cd (or mkdir->cd) into the build directory,
then run "cmake .." and "make". The executable of the test should be placed in the
same folder with the name EMBB_linearizability_test.

* On Windows system
We suppose that the embb library was build in ../build (relative path to the current
folder of the linearizability test).
NOTE: the embb library should be build in Release mode, i.e. by running the command
"cmake --build . --config Release" from inside the correct directory.
From within the linearizability_tester folder, cd (or mkdir->cd) into the build directory,
then run "cmake .." and "cmake --build . --config Release". The Visual Studio prompt should
be used.
The executable of the test should be placed in the folder Release 
with the name EMBB_linearizability_test.exe.

HOW TO IMPLEMENT TESTS FOR OTHER DATA STRUCTURES

Data structures that support the following functions: 
- check if structure is empty
- check if structure contains a specific element
- insert an element in the structure
- erase an element from the structure
should be easily checkable using the Set specification contained in the 
src/sequential_datastructures.h file.
In general, the checker needs a sequential implementation test against the
concurrent one (the EMBB one). The implementation should support efficient equality checks
and efficient memory management. Stick to the implementation of the already integrated
structures (stacks, queues..) for future integration. 
After adding the implementation of the datastructure, it is necessary to add a worker and
experiment for the newly created datastructure in main.cc. Stick again to the already
implemented functions (it should be sufficient to change the name of the methods that are called
on the EMBB structure).