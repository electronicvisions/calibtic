#!/usr/bin/env python

import logging

logging.basicConfig(level=logging.INFO)

from pywrap.wrapper import Wrapper
import pywrap.namespaces as ns
from pygccxml.declarations import smart_pointer_traits, cpptypes
from pyplusplus.module_builder import call_policies
from pywrap import containers, classes, matchers
from pyplusplus.code_creators import smart_pointers

wrap = Wrapper()
mb = wrap.mb


containers.extend_std_containers(mb)

shared_ptr_class_factories = [
    # main data structures
    'Collection', 'Calibration',

    # tranformations
    'Constant', 'Polynomial', 'OneOverPolynomial',
    'NegativePowersPolynomial', 'SumOfTrafos', 'PowerOfTrafo', 'Lookup',

    # specific data structures
    'NeuronCalibration', 'SharedCalibration', 'ADCCalibration',
    'NeuronCollection', 'BlockCollection', 'HICANNCollection',
    'SynapseCalibration', 'SynapseRowCalibration',  'SynapseRowCollection',
    'STPUtilizationCalibration', 'QuadraticADCCalibration',
    'L1CrossbarCollection', 'L1CrossbarCalibration',
]

for c in ns.find(mb, 'classes', shared_ptr_class_factories):
    c.include()
    c.mem_funs('copy', allow_empty=True).exclude()

    # Note: don't use '::boost::shared_ptr', or Py++ will generate unneeded
    # converter
    c.held_type = smart_pointers.held_type_t('boost::shared_ptr')

    # add print() fct
    c.add_registration_code('def(bp::self_ns::str(bp::self_ns::self))')

    # For pickle support we need to add EXPORT macros, see bindings.h
    c.add_declaration_code(
        'BOOST_CLASS_EXPORT_IMPLEMENT({})'.format(c.decl_string))
    classes.add_pickle_suite(c)

    # Py++ doesn't wrap protected constructors correctly, when a held type is used
    f = matchers.access_type_matcher_t('protected')
    c.constructors(f, allow_empty=True).exclude()

cls = [
    'Base',
    'Transformation',
    'MetaData',
    'HWNeuronParameter',
    'HWSharedParameter',
    'GmaxConfig',
]

for c in ns.find(mb, 'classes', cls):
    c.include()

    # add print() fct
    c.add_registration_code('def(bp::self_ns::str(bp::self_ns::self))')

cls = [
    'NeuronCalibrationParameters',
    'VoltageMeasurement',
    'DataPoint',
]

ns.include(mb, 'classes', cls)

ns.include(mb, 'namespaces', ['::calibtic::backend'])

calibtic = mb.global_ns.namespace('::calibtic')

cls = mb.class_('Polynomial')
for fname in ('find_roots', 'find_real_roots'):
    cls.mem_fun(fname).call_policies = call_policies.custom_call_policies(
        "::pywrap::ReturnNumpyPolicy", "pywrap/return_numpy_policy.hpp")

def points_to_smart_ptr(td):
    return smart_pointer_traits.is_smart_pointer(td.target_decl)
calibtic.typedefs(points_to_smart_ptr).exclude()

free_functions = [
    'loadADCCalibration',
    'storeADCCalibration',
    ]

for fun in free_functions:
    mb.free_function(fun).include()

ns.exclude(mb, 'namespaces', [
           '::boost::serialization',
           '::boost::archive',
           '::boost::mpi', ],
           allow_missing=True)

# expose only public interfaces
ns.exclude_by_access_type(mb, ['classes'], 'private')


mb.decl("::std::pair< double, double>").include()

ns_hmf = mb.namespace("::HMF")
f = ns_hmf.free_function("to_string",
                         arg_types=["::HMF::NeuronCalibrationParameters::Calibrations::calib"])
f.include()

wrap.finish()
