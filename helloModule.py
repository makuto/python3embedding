"""Module to test C++ embedding and extending"""

import testCAPI

def helloWorld():
    print("Hello C++, it's Python!")
    testCAPI.helloFromC()
    return
