#!/usr/bin/env python

import pycalibtic
from pyhalbe.HICANN import neuron_parameter

HW_params = pycalibtic.HWNeuronParameter()
HW_params.setParam(neuron_parameter.E_l,      511)
HW_params.setParam(neuron_parameter.I_gl,     511)
HW_params.setParam(neuron_parameter.I_gladapt,511)
HW_params.setParam(neuron_parameter.I_radapt, 511)
HW_params.setParam(neuron_parameter.I_rexp,   511)
HW_params.setParam(neuron_parameter.I_fire,   511)
HW_params.setParam(neuron_parameter.E_synx,   511)
HW_params.setParam(neuron_parameter.E_syni,   511)
HW_params.setParam(neuron_parameter.V_syntcx, 487)
HW_params.setParam(neuron_parameter.V_syntci, 487)
HW_params.setParam(neuron_parameter.V_exp,    511)
HW_params.setParam(neuron_parameter.I_pl,     511)
HW_params.setParam(neuron_parameter.V_t,      511)

neuron_calib = pycalibtic.NeuronCalibration()
neuron_calib.setDefaults()

speedup = 10000
cm_bio = 100. * pycalibtic.NeuronCalibrationParameters.big_cap
bio_params = neuron_calib.applyNeuronReverse(HW_params, speedup, cm_bio)

print(bio_params)
