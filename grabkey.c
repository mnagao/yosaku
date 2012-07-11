
#include <Python.h>
#include <X11/Xlib.h>

static PyObject *my_callback = NULL;
static int keycode = 0;
static Display *disp = NULL;
static int flag_stop_grabkey = 0;

static PyObject *set_callback(PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)){
        if (!PyCallable_Check(temp)){
            PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
            return NULL;
        }
        Py_XINCREF(temp);
        Py_XDECREF(my_callback);
        my_callback = temp;
        Py_INCREF(Py_None);
        result = Py_None;
        printf("set_callback() ok.\n");
    }
    return result;
}

static PyObject *exec_callback(PyObject *self, PyObject *args)
{
    PyObject *args1;
    PyObject *result;

    args1 = Py_BuildValue("(i)", 999);
    result = PyObject_CallObject(my_callback, args1);
    if (result == NULL){
        printf("exec callback failed.\n");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *set_grabkey(PyObject *self, PyObject *args)
{
    PyObject *result;

    if (!PyArg_ParseTuple(args, "i", &keycode)){
         return NULL;
    }
    printf("set_grabkey: arg = %d\n", keycode);

    disp = XOpenDisplay(NULL);
    if (disp == NULL){
        printf("XOpenDisplay() Error.\n");
        return NULL;
    }

    XGrabKey(disp, keycode, ControlMask, DefaultRootWindow(disp), True, GrabModeAsync, GrabModeAsync);
    XGrabKey(disp, 54, ControlMask, DefaultRootWindow(disp), True, GrabModeAsync, GrabModeAsync);

    return Py_BuildValue("i", 1);
}

static PyObject *start_grabkey(PyObject *self, PyObject *args)
{
    XEvent e;
    PyObject *result;

    while(1){
        if (XPending(disp)){
            XNextEvent(disp, &e);
            switch (e.type){
                case KeyPress:
                    printf("Key pressed. keycode = %d\n", e.xkey.keycode);
                    if (e.xkey.keycode == 54){
                        flag_stop_grabkey = 1;
                    }
                    result = PyObject_CallObject(my_callback, Py_BuildValue("(i)", e.xkey.keycode));
                    if (result == NULL){
                        printf("exec callback failed.\n");
                        return NULL;
                    }
                    break;
            }
        }
        else {
            if (flag_stop_grabkey){
                XUngrabKey(disp, keycode, ControlMask, DefaultRootWindow(disp));
                XCloseDisplay(disp);
                break;
            }

        }
    }
 
    return Py_BuildValue("i", keycode);
}

static PyObject *end_grabkey(PyObject *self, PyObject *args)
{
    printf("in end_grabkey()\n");
    XUngrabKey(disp, keycode, ControlMask, DefaultRootWindow(disp));
    printf("end XUngrabKey()\n");
    XCloseDisplay(disp);
    printf("end XCloseDisplay()\n");

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef mymethods[] = {
    {"set_callback", set_callback, METH_VARARGS, "set my callback"},
    {"exec_callback", exec_callback, METH_VARARGS, "exec callback"},
    {"set_grabkey", set_grabkey, METH_VARARGS, "set grab key"},
    {"start_grabkey", start_grabkey, METH_VARARGS, "start grab key"},
    {"end_grabkey", end_grabkey, METH_VARARGS, "end grab key"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initgrabkey(void)
{
    (void) Py_InitModule("grabkey", mymethods);
}

