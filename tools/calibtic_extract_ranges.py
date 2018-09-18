#!/usr/bin/env python
"""
script to extract the parameter ranges for the default calibration.
I.e. for each parameter, it extracts the range of biological parameters, that can be mapped to the voltage/current domain of the hardware.
"""

import pycalibtic as cal
import pyhalbe
import pprint
import numpy as np

def nice_limits(limits):
    return "({0:.3g}, {1:.3g})".format(*limits)

def pprint_ranges_dict(ranges):
    for name,limits in ranges.items():
        print name,":", nice_limits(limits)

nrn_param = pyhalbe.HICANN.neuron_parameter

bio_to_hw_name = {
    #'cm'
    'tau_refrac': nrn_param.I_pl,
    'v_spike'   : nrn_param.V_t,
    #'v_reset'   : pyhalbe.HICANN.shared_parameter.V_reset,
    'v_rest'    : nrn_param.E_l,
    'tau_m'     : nrn_param.I_gl,
    #'i_offset'
    'a'         : nrn_param.I_gladapt,
    'b'         : nrn_param.I_fire,
    'delta_T'   : nrn_param.I_rexp,
    'tau_w'     : nrn_param.I_radapt,
    'v_thresh'  : nrn_param.V_exp,
    'e_rev_E'   : nrn_param.E_synx,
    'tau_syn_E' : nrn_param.V_syntcx,
    'e_rev_I'   : nrn_param.E_syni,
    'tau_syn_I' : nrn_param.V_syntci,
    }


def extract_bio_ranges(nc, shiftV, alphaV, speedup, cm_bio, bigcap):
    """
    extract the biological ADEX parameter ranges from calibration data.
    From calibration data for each ADEX parameter, the domain ranges
    are extracted and scaled to the biological domain.

    params:
      nc      - calibration data for a neuron of type pycalibtic.NeuronCalibration
      shiftV  - voltage shift from bio to hw domain in Volt
      alphaV  - voltage scale from bio to hw domain (unitless) 
      speedup - speedup factor of hardware emulation vs biological realtime
      cm_bio  - biological membrane capacitance in nF
      bigcap  - choose big (small) capacitance in hardware neuron if True (False)

    returns:
      Biological parameter ranges for ADEX model, in detail:
      dictioniary containing pyNN.EIF_cond_exp_isfa_ista parameter names as keys
      (except 'cm' and 'i_offset'), and parameter ranges as tuples of size 2 with
      lower and upper bound in PyNN compatible units.
    """
    assert isinstance(nc, cal.NeuronCalibration)
    assert isinstance(bigcap, bool)
    cm_hw = nc.big_cap if bigcap else nc.small_cap
    bio_ranges = {}
    for bio, hw in bio_to_hw_name.items():
        if isinstance(nc.at(hw), cal.Polynomial):
            domain_range = nc.at(hw).getDomainBoundaries()
        elif isinstance(nc.at(hw), cal.Lookup):
            data = nc.at(hw).getData()
            domain_range = [min(data), max(data)]
        else:
            raise Exception("Unsupported Trafo type")
        assert len(domain_range) == 2
        domain_range = sorted(domain_range) # returns list
        bio_range = []
        if bio in ['tau_refrac','tau_m','tau_w','tau_syn_E','tau_syn_I']:
            for val in domain_range:
                bio_range.append(nc.reverseScaleTau(val, speedup))
        elif bio in ['v_spike','v_rest','e_rev_E','e_rev_I','v_thresh']:
            for val in domain_range:
                bio_range.append(nc.reverseScaleVoltage(val, shiftV, alphaV))
        elif bio == 'delta_T':
            for val in domain_range:
                bio_range.append(nc.reverseScaleVoltageDeltaT(val, alphaV))
        elif bio == 'a':
            for val in domain_range:
                bio_range.append(nc.reverseScaleConductance(val, speedup, cm_bio, cm_hw))
        elif bio == 'b':
            for val in domain_range:
                bio_range.append(nc.reverseScaleCurrent(val, speedup, cm_bio, alphaV, cm_hw))
        else:
            raise Exception("Unsupported AdEx parameter")
        bio_ranges[bio] = tuple(bio_range)
    return bio_ranges


def print_default_bio_ranges():
    nc = cal.NeuronCalibration()
    nc.setDefaults()
    nc_params = cal.NeuronCalibrationParameters()
    cm_bio = 0.281 # nS, default ADEX membrane capacitance
    #cm_bio = 10. # nS, default IF_cond_exp membrane capacitance
    nc_params.bigcap = True
    nc_params.alphaV=5.
    speedup = 10000.
    bio_ranges = extract_bio_ranges(nc, nc_params.shiftV, nc_params.alphaV, speedup, cm_bio, nc_params.bigcap)
    print "Bio ranges for parameters"
    print "speedup:", speedup
    print "shiftV:", nc_params.shiftV, "V"
    print "alphaV:", nc_params.alphaV
    print "bigcap:", nc_params.bigcap
    print "cm_bio:", cm_bio, "nF"
    #pprint.pprint(bio_ranges)
    pprint_ranges_dict(bio_ranges)


def extract_hw_ranges(nc):
    """
    extract the DAC ranges corresponding to the domain of parameter transformations
    Takes the available domain boundaries of the model parameters, translates them
    to dac values and returns the dac ranges.

    This is useful to e.g. test the ESS via sthal with values that lie in the domain
    when the reverse transformation is applied.
    """
    assert isinstance(nc, cal.NeuronCalibration)
    hw_params = bio_to_hw_name.values()
    hw_ranges = {}
    for param in hw_params:
        if isinstance(nc.at(param), cal.Polynomial):
            domain_range = nc.at(param).getDomainBoundaries()
        elif isinstance(nc.at(param), cal.Lookup):
            data = nc.at(param).getData()
            domain_range = [min(data), max(data)]
        else:
            raise Exception("Unsupported Trafo type")
        assert len(domain_range) == 2
        domain_range = list(domain_range)
        hw_range = []
        for val in domain_range:
            hw_range.append(nc.to_dac(val,param))
        hw_range.sort()
        hw_ranges[param] = tuple(hw_range)
    return hw_ranges


def print_default_hw_ranges():
    nc = cal.NeuronCalibration()
    nc.setDefaults()
    hw_ranges = extract_hw_ranges(nc)
    print "DAC ranges:"
    pprint.pprint(hw_ranges)

def extract_hw_weight_range(src):
    """
    returns the range of synaptic weights in the hardware in nanoSiemens
    params:
      src - calibdata for one synapse row of type
            pycalibtic.SynapseRowCalibration
    returns:
      tuple(min weight, max weight) for a single synapse in nanoSiemens
    """
    assert isinstance(src, cal.SynapseRowCalibration)
    global_min = np.inf
    global_max = -np.inf
    # test all gmax configs
    for sel_vgmax in range(4):
        for gmax_div in range(16):
            gc = cal.GmaxConfig(sel_vgmax, gmax_div)
            if src.exists(gc):
                sc = src.at(gc)
                min_ = sc.getMinAnalogWeight()
                max_ = sc.getMaxAnalogWeight()
                global_min = min(global_min,min_)
                global_max = max(global_max,max_)
    return (global_min,global_max)

def extract_bio_weight_range(src, speedup, cm_bio, bigcap, neuron_size):
    """
    extract the biological range of synaptic weights in the hardware.
    First, the hardware range of a single synapse is extracted from
    the calibration of a synapse row, then these ranges are scaled back
    to the biological domain considering the relevant settings (see below).

    params:
      src     - calibdata for one synapse row of type
                pycalibtic.SynapseRowCalibration
      speedup - speedup factor of hardware emulation vs biological realtime
      cm_bio  - biological membrane capacitance in nF
      bigcap  - choose big (small) capacitance in hardware neuron if True (False)
      neuron_size - number of hardware neurons forming one compound neuron

    returns:
      tuple(min weight, max weight) in microSiemens (PyNN unit for weights)
    """
    hw_range = extract_hw_weight_range(src)
    nc = cal.NeuronCalibration()
    cm_hw = nc.big_cap if bigcap else nc.small_cap
    cm_hw *= neuron_size
    bio_range = []
    nS_to_S = 1.e-9
    nS_to_uS = 1.e-3
    for val in hw_range:
        bio_range.append(nc.reverseScaleConductance(val*nS_to_S, speedup, cm_bio, cm_hw)*nS_to_uS)
    return bio_range

def print_default_hw_weight_range():
    src = cal.SynapseRowCalibration()
    src.setEssDefaults()
    hw_range = extract_hw_weight_range(src)
    print "HW weight range:"
    print nice_limits(hw_range), "nS"

def print_default_bio_weight_range(neuron_size):
    src = cal.SynapseRowCalibration()
    src.setEssDefaults()
    nc_params = cal.NeuronCalibrationParameters()
    cm_bio = 0.281 # nS, default ADEX membrane capacitance
    speedup = 10000.
    bio_weight_range = extract_bio_weight_range(src, speedup, cm_bio, nc_params.bigcap, neuron_size)
    print "Bio weight range for parameters"
    print "speedup:", speedup
    print "bigcap:", nc_params.bigcap
    print "cm_bio:", cm_bio, "nF"
    print "neuron_size:", neuron_size
    print "weight:", nice_limits(bio_weight_range), "uS"

if __name__ == "__main__":
    print_default_bio_ranges()
    print_default_hw_ranges()
    print_default_hw_weight_range()
    print_default_bio_weight_range(neuron_size=2)
