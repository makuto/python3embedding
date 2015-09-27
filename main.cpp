#include <iostream>
#include <Python.h> // Note that I use -I in compile args to specify version, which isn't great
#include <string>

/* Hacked together C++ code to show how to both embed and extend
 * Python 3 in C++ (albeit, this code is basically C because I use very
 * few C++ features).
 *
 * Note that this is basically cut-and-paste from several
 * sources, usually not verbatim but close. See References to get an idea of
 * where I got this this stuff from
 *
 * --------------
 * =    TODO    =
 * --------------
 * 
 *      - Arguments to / from C++ functions
 *      - Returning stuff to Python from C++
 *      - Returning stuff from Python to C++
 *      - Calling Python class methods
 *      - Calling C++ class methods (probably impossible - need to wrap and use global class singletons or something)
 * */
 
///////////////////////////////////////////////////////////
// Compiling + linking (assuming g++)                    //
///////////////////////////////////////////////////////////
/*

  compile:
    g++ -c -Wall -Wno-unused-result -Werror=declaration-after-statement -g -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security  -DNDEBUG -fwrapv --std=c++11 -pedantic -I/usr/include/python3.4dm "%f"

  link:
    g++ -o "%e" "%e.o" -L/usr/lib -lpthread -ldl  -lutil -lm  -lpython3.4dm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

  To get official python build/link args, use:
    python3.4-config --cflags
  and
    python3.4-config --ldflags

*/

///////////////////////////////////////////////////////////
// References   (look here when you see [n])             //
///////////////////////////////////////////////////////////
/*
 
  https: docs.python.org/3/extending/embedding.html
       (great overview for Python 3) [1]
  https: docs.python.org/2/extending/embedding.html
       (overview for 2.7) [2] 
  http: rhodesmill.org/brandon/2008/porting-a-c-extension-module-to-python-30/
       (string to pystring) [3]
  https: docs.python.org/3/c-api/intro.html#embedding-python
       (details about programname stuff) [4]
  http: www.cplusplus.com/forum/beginner/49000/
       (wchar_t conversion) [5]
  http: markmail.org/message/qwex6rhhqd43jg2o
       (Setting python import path (important when having private dir for python modules)) [6]

*/
///////////////////////////////////////////////////////////
// C++ API module (Python extension)                     //
///////////////////////////////////////////////////////////

void helloWorld(void)
{
    std::cout << "Hello Python, it's C++!\n";
}
static PyObject* emb_helloWorld(PyObject* self, PyObject *args)
{
    helloWorld();
    return Py_BuildValue("");
}
static PyMethodDef EmbMethods[] = {
    {"helloFromC", emb_helloWorld, METH_VARARGS,
        "have c++ say hello"},

    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef testCAPI = {
    PyModuleDef_HEAD_INIT,
    "testCAPI",
    NULL,
    -1,
    EmbMethods
};

static PyObject* PyInit_testCAPI(void)
{
    return PyModule_Create(&testCAPI);
}

///////////////////////////////////////////////////////////
// Main                                                  //
///////////////////////////////////////////////////////////

// Test python embedding and extending
// Follow [1] to get what's going on
int main()
{
    PyObject *pModule;
    PyObject *pModuleName;
    PyObject *pModuleFunc;

    // See [5]
    const wchar_t c_s[] = L"/home/macoy/projects/pythonTesting/main";
    wchar_t *s = new wchar_t[sizeof(c_s) / sizeof(c_s[0])];
    wcscpy(s, c_s);

    const wchar_t p_s[] = L":.";
    wchar_t *ps = new wchar_t[sizeof(p_s) / sizeof(p_s[0])];
    wcscpy(ps, p_s);

    Py_SetProgramName(s); // see [4]

    // Add C++ API to modules (supposed to call before Py_Initialize!). See [1]
    PyImport_AppendInittab("testCAPI", &PyInit_testCAPI);
    
    Py_Initialize();

    delete[] s;

    // See [6]; The below code makes it so we can import from the current directory
    wchar_t *path, *newpath;
    path=Py_GetPath();
    newpath=new wchar_t[wcslen(path)+4];
    wcscpy(newpath, path);
    wcscat(newpath, ps);  // ":." for unix, or ";." for windows
    PySys_SetPath(newpath);
    delete[] newpath;

    // See [3]
    pModuleName = PyUnicode_FromString("helloModule");

    pModule = PyImport_Import(pModuleName);
    Py_DECREF(pModuleName);

    // Follow [1] to get what's going on
    if (pModule != NULL)
    {
        pModuleFunc = PyObject_GetAttrString(pModule, "helloWorld");

        if (pModuleFunc && PyCallable_Check(pModuleFunc))
        {
            PyObject* pArgs = NULL;
            PyObject *pReturnVal = PyObject_CallObject(pModuleFunc, pArgs);
        }
        else
            std::cout << "error: no func\n";
        Py_XDECREF(pModuleFunc);
        Py_DECREF(pModule);
    }
    else
        std::cout << "error: no module\n";

    // cleanup (probably missing some Py_DECREFs but oh well)
    Py_Finalize();
    return 0;
}

