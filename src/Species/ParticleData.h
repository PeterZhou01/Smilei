#ifndef PARTICLEDATA_H
#define PARTICLEDATA_H

#ifdef SMILEI_USE_NUMPY
#include "PyTools.h"

#include <numpy/arrayobject.h>

#include "Particles.h"

// class for exposing Particles to numpy
class ParticleData {
public:
    ParticleData(unsigned int nparticles) {
        // Use the empty python class "Particles" to store data
        PyObject* ParticleDataClass = PyObject_GetAttrString(PyImport_AddModule("__main__"),"ParticleData");
        particles = PyObject_CallObject(ParticleDataClass, NULL);
        Py_DECREF(ParticleDataClass);
        
        dims[0] = nparticles;
    };
    
    // Special constructor for initialization test only
    // Check a numpy function with fake data
    template <typename T>
    ParticleData( unsigned int nDim_particle, PyObject* function, std::string errorPrefix, T * dummy )
    : ParticleData(2)
    {
        // Check if function is callable
        unsigned int n_arg = PyTools::function_nargs(function);
        if( n_arg < 0 )
            ERROR(errorPrefix << " does not look like a normal python function");
        // Verify the number of arguments of the filter function
        if( n_arg != 1 )
            ERROR(errorPrefix << " has "<<n_arg<<" arguments while requiring 1");
        // Fill with fake data
        std::vector<double> test_value = {1.2, 1.4};
        std::vector<uint64_t> test_id = {3, 4};
        std::vector<short> test_charge = {3, 4};
        setVectorAttr( test_value, "x" );
        if( nDim_particle > 1 ) {
            setVectorAttr( test_value, "y" );
            if( nDim_particle > 2 )
                setVectorAttr( test_value, "z" );
        }
        setVectorAttr( test_value, "px" );
        setVectorAttr( test_value, "py" );
        setVectorAttr( test_value, "pz" );
        setVectorAttr( test_value, "weight" );
        setVectorAttr( test_charge, "charge" );
        setVectorAttr( test_id, "id" );
        // Verify the return value of the function
        PyObject *ret(nullptr);
        ret = PyObject_CallFunctionObjArgs(function, particles, NULL);
        if( !PyArray_Check(ret) )
            ERROR(errorPrefix << " must return a numpy array");
        checkType( ret, errorPrefix, dummy );
        unsigned int s = PyArray_SIZE((PyArrayObject *)ret);
        if( s != 2 )
            ERROR(errorPrefix << " must not change the arrays sizes");
        Py_DECREF(ret);
    };
    
    // Destructor
    ~ParticleData() {
        clear();
        Py_DECREF(particles);
    };
    
    inline void resize(unsigned int nparticles) {
        dims[0] = nparticles;
    };
    
    // Expose a vector to numpy
    inline PyArrayObject* vector2numpy( std::vector<double> &vec ) {
        return (PyArrayObject*) PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, (double*)(vec.data()));
    };
    inline PyArrayObject* vector2numpy( std::vector<uint64_t> &vec ) {
        return (PyArrayObject*) PyArray_SimpleNewFromData(1, dims, NPY_UINT64, (uint64_t*)(vec.data()));
    };
    inline PyArrayObject* vector2numpy( std::vector<short> &vec ) {
        return (PyArrayObject*) PyArray_SimpleNewFromData(1, dims, NPY_SHORT, (short*)(vec.data()));
    };
    
    // Add a C++ vector as an attribute, but exposed as a numpy array
    template <typename T>
    inline void setVectorAttr( std::vector<T> &vec, std::string name ) {
        PyArrayObject* numpy_vector = vector2numpy( vec );
        PyObject_SetAttrString(particles, name.c_str(), (PyObject*)numpy_vector);
        attrs.push_back( numpy_vector );
    };
    
    // Remove python references of all attributes
    inline void clear() {
        unsigned int nattr = attrs.size();
        for( unsigned int i=0; i<nattr; i++) Py_DECREF(attrs[i]);
        attrs.resize(0);
    };
    
    inline PyObject* get() {
        return particles;
    };
    
private:
    PyObject* particles;
    std::vector<PyArrayObject*> attrs;
    npy_intp dims[1];
    
    void checkType( PyObject *obj, std::string &errorPrefix, bool * dummy) {
        if( !PyArray_ISBOOL((PyArrayObject *)obj) )
            ERROR(errorPrefix << " must return an array of booleans");
    };
    void checkType( PyObject *obj, std::string &errorPrefix, double * dummy) {
        if( !PyArray_ISFLOAT((PyArrayObject *)obj) )
            ERROR(errorPrefix << " must return an array of floats");
    };
};

#endif

#endif
