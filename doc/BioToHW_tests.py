#!/usr/bin/env python

import pycalibtic
import pycellparameters
from pyhalbe.HICANN import neuron_parameter, shared_parameter

neuron_calib = pycalibtic.NeuronCalibration()
neuron_calib.setDefaults()

neuron_calib_params = pycalibtic.NeuronCalibrationParameters()
neuron_calib_params.alphaV = 10
neuron_calib_params.shiftV = 1.2

bio_params = pycellparameters.EIF_cond_exp_isfa_ista()

bio_params.tau_refrac = 1.00
bio_params.a = 2.5
bio_params.tau_m = 10.0
bio_params.e_rev_E = 0.0
bio_params.cm = 0.24
bio_params.delta_T = 1.2
bio_params.e_rev_I = -80.0
bio_params.v_thresh = -40.0
bio_params.b = 0.05
bio_params.tau_syn_E = 2
bio_params.v_spike = 0.0
bio_params.tau_syn_I = 2
bio_params.tau_w = 30.0
bio_params.v_rest = -60.0

speedup = 10000

HW_params = neuron_calib.applyNeuronCalibration(bio_params, speedup)

print(HW_params)
