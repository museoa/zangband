%typemap(python, varout) u32b[ANY], s32b[ANY], u16b[ANY], s16b[ANY], byte[ANY]
{
	PyObject *o;
	int i;

	$target = PyList_New($dim0);

	for (i = 0; i < $dim0; i++)
	{
		o = PyInt_FromLong($source[i]);
		PyList_SetItem($target, i, o);
	}
}

%typemap(python, varout) term* [ANY]
{
	PyObject *o;
	int i;

	$target = PyList_New($dim0);

	for (i = 0; i < $dim0; i++)
	{
		o = SWIG_NewPointerObj((void *)angband_term[i], SWIGTYPE_term_p);
		PyList_SetItem($target, i, o);
	}
}

%typemap(python, varout) char [ANY][ANY]
{
	PyObject *o;
	int i;

	$target = PyList_New($dim0);

	for (i = 0; i < $dim0; i++)
	{
		o = PyString_FromString($source[i]);
		PyList_SetItem($target, i, o);
	}
}


%typemap(python, varout) byte [ANY][ANY]
{
	PyObject *tuple;
	int i, j;

	$target = PyList_New($dim0);

	for (i = 0; i < $dim0; i++)
	{
		tuple = PyTuple_New($dim1);

		for (j = 0; j < $dim1; j++)
			PyTuple_SetItem(tuple, j, PyInt_FromLong($source[i][j]));

		PyList_SetItem($target, i, tuple);
	}
}

%typemap(python, argout) int *
{
	PyObject *o;

	o = PyInt_FromLong(*$source);

	if ((!$target) || ($target == Py_None))
	{
		$target = o;
	}
	else
	{
		if (!PyList_Check($target))
		{
			PyObject *o2 = $target;
			$target = PyList_New(0);
			PyList_Append($target, o2);
			Py_XDECREF(o2);
		}

		PyList_Append($target, o);
		Py_XDECREF(o);
	}
}

%typemap(python, ignore) int * (int temp)
{
	$target = &temp; /* Assign the pointer to a local variable */
}

