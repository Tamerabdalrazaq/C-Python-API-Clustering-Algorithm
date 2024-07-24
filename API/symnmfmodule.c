#include <Python.h>
#define PY_SSIZE_T_CLEAN
#include "symnmf.h"
#include <stdio.h>
#include <math.h>

PyObject *convert_array_to_python_list(double *arr, int N)
{
    PyObject *py_list = PyList_New(N);
    if (py_list == NULL)
    {
        printf("An Error Has Occurred!");
        exit(1);
    }

    for (int i = 0; i < N; i++)
    {
        PyObject *py_double = Py_BuildValue("d", arr[i]);
        if (py_double == NULL)
        {
            printf("An Error Has Occurred!!");
            exit(1);
            Py_DECREF(py_list);
            return NULL;
        }
        PyList_SetItem(py_list, i, py_double);
    }

    return py_list;
}


void *convert_list_to_double_array(PyObject *list)
{
    PyObject *item;
    int n = PyObject_Length(list);
    if (n < 0)
    {
        return NULL;
    }
    // printf("%d\n", PyObject_Length(list));
    double *arr = (double *)malloc(n * sizeof(double));
    if (arr == NULL)
    {
        printf("An Error Has Occurred!!!!");
        exit(1);
    }
    int i;
    for (i = 0; i < n; i++)
    {
        item = PyList_GetItem(list, i);
        arr[i] = PyFloat_AsDouble(item);
    }
    return arr;
}


static PyObject *module_sym(PyObject *self, PyObject *args)
{
    int N, d;
    PyObject *data_list_ptr;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "Oii", &data_list_ptr, &N, &d))
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    // *data = process_matrix_interface(data_list_ptr);
    double* data_array = convert_list_to_double_array(data_list_ptr);
    double* similarity_matrix = sym(data_array, N, d);
    return convert_array_to_python_list(similarity_matrix, N*N);
}


static PyObject *module_ddg(PyObject *self, PyObject *args)
{
    int N;
    PyObject *data_list_ptr;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "Oi", &data_list_ptr, &N))
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    // *data = process_matrix_interface(data_list_ptr);
    double* data_array = convert_list_to_double_array(data_list_ptr);
    double* ddg_matrix = ddg(data_array, N);
    return convert_array_to_python_list(ddg_matrix, N*N);
}

static PyObject *module_norm(PyObject *self, PyObject *args)
{
    int N;
    PyObject *sym_matrix_ptr;
    PyObject *ddg_matrix_ptr;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "OOi", &sym_matrix_ptr, &ddg_matrix_ptr, &N))
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    // *data = process_matrix_interface(data_list_ptr);
    double* sym_matrix = convert_list_to_double_array(sym_matrix_ptr);
    double* ddg_matrix = convert_list_to_double_array(ddg_matrix_ptr);
    double* norm_matrix = norm(sym_matrix, ddg_matrix, N);
    return convert_array_to_python_list(norm_matrix, N*N);
}

static PyObject *module_symnmf(PyObject *self, PyObject *args)
{
    int N, k;
    PyObject *norm_matrix_ptr;
    PyObject *H_matrix_ptr;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "OOii", &norm_matrix_ptr, &H_matrix_ptr, &N, &k))
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    // *data = process_matrix_interface(data_list_ptr);
    double* norm_matrix = convert_list_to_double_array(norm_matrix_ptr);
    double* H_matrix = convert_list_to_double_array(H_matrix_ptr);
    double* H = symnmf(norm_matrix, H_matrix, N, k);
    return convert_array_to_python_list(H, N*k);
}

static PyMethodDef geoMethods[] = {
    {"module_sym",
     (PyCFunction)module_sym, 
     METH_VARARGS, 
     PyDoc_STR("args: (X, N, k) where X is a py list (dim N*k), N, k integers")
     },
    {"module_ddg",
     (PyCFunction)module_ddg, 
     METH_VARARGS,
     PyDoc_STR(" args: (X, N) where X is a py list (dim N*N), N integer")
     },
    {"module_norm",
     (PyCFunction)module_norm, 
     METH_VARARGS,
     PyDoc_STR("args: (X, Y, N) where X and Y are  py lists (dim N*N), N integer")
     },
    {"module_symnmf",
     (PyCFunction)module_symnmf, 
     METH_VARARGS,
     PyDoc_STR("args: (X, Y, N) where X and Y are  py lists (X with dim N*N, Y with dim N*k), N, k integers")
     },
    /*  The docstring for the function */
    {NULL, NULL, 0, NULL}, 
};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmfmodule", /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    geoMethods    /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_symnmfmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m)
    {
        return NULL;
    }
    return m;
}