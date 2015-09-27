Python 3 Embedding and Extending with C++
==========================================

Hacked together C++ code to show how to both embed and extend Python 3 in C++ (albeit, this code is basically C because I use very few C++ features).

Note that this is basically cut-and-paste from several sources, usually not verbatim but close. See References in main.cpp to get an idea of where I got this this stuff from.

I assume you are using Linux, g++, and Python 3.4. You may have to change some stuff in order to get this running.

To Do
------
-Arguments to / from C++ functions
-Returning stuff to Python from C++
-Returning stuff from Python to C++
-Calling Python class methods
-Calling C++ class methods (probably impossible - need to wrap and use global class singletons or something)
