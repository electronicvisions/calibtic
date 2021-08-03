"""
script to extract the domain values for the default calibration.
I.e. for each parameter, it extracts the range of biological parameters, that can be mapped to the voltage/current domain of the hardware.
"""
import pycalibtic as cal
import pyhalbe
import numpy as np

nrn_param = pyhalbe.HICANN.neuron_parameter

nc = cal.NeuronCalibration()
nc.setDefaults()
# remove domain!
for param in [nrn_param.E_l, nrn_param.E_synx, nrn_param.E_syni,nrn_param.V_exp, nrn_param.V_t,
    nrn_param.I_pl, nrn_param.I_radapt, nrn_param.V_syntcx, nrn_param.V_syntci, nrn_param.I_gl,
    nrn_param.I_gladapt, nrn_param.I_fire, nrn_param.I_rexp]:

    coeffs = nc.at(param).getData()
    nc.reset(param,cal.Polynomial(coeffs))

min_current = 0.
max_current = 2500.

min_voltage = 0.
max_voltage = 1800.

min_current_tau = max_current*1./1023
max_current_tau = max_current

def extract_domain_voltage(nc, param):
    assert(param in [nrn_param.E_l, nrn_param.E_synx, nrn_param.E_syni,nrn_param.V_exp, nrn_param.V_t])
    trafo = nc.at(param)
    trafo.getDomain().lower
    coeffs = trafo.getData()
    min = trafo.find_real_roots(min_voltage, False)
    max = trafo.find_real_roots(max_voltage, False)
    return (min, max)

def extract_domain_current(nc, param):
    trafo = nc.at(param)
    coeffs = trafo.getData()
    min = trafo.find_real_roots(min_current, False)
    max = trafo.find_real_roots(max_current, False)
    return (min, max)

def extract_domain_tau_refrac(nc, param):
    assert(param == pyhalbe.HICANN.neuron_parameter.I_pl)
    trafo = nc.at(param)
    coeffs = trafo.getData()
    min = trafo.find_real_roots(1./max_current_tau, False)
    max = trafo.find_real_roots(1./min_current_tau, False)
    return (min, max)

def extract_domain_tau_w(nc, param):
    assert(param == pyhalbe.HICANN.neuron_parameter.I_radapt)
    trafo = nc.at(param)
    coeffs = trafo.getData()
    min = trafo.find_real_roots(1./max_current_tau, False)
    max = trafo.find_real_roots(1./min_current_tau, False)
    if len(max)==0:
        # determine maximum of polynomial
        coeffs = np.array(coeffs)
        p1 = np.poly1d(coeffs[::-1])
        p1_derived = p1.deriv()
        max = p1_derived.r[0]
        # compute fg value and recompute max
        current_reverse = trafo.apply(max)
        current = 1./current_reverse
        dac = int(current/max_current*1023) + 1 # round up
        val = 1./(dac*1./1023*max_current)
        max = np.min(trafo.find_real_roots(val, False)) # use lower value
        assert( 1./max_current_tau <= trafo.apply(max) <= 1./min_current_tau)
        
    return (min, max)

def extract_domain_tau_syn(nc, param):
    trafo = nc.at(param)
    min = trafo.find_real_roots(min_voltage, False)
    max = trafo.find_real_roots(max_voltage, False)
    if len(max)==0:
        # determine maximum of polynomial
        coeffs = np.array(trafo.getData())
        p1 = np.poly1d(coeffs[::-1])
        p1_derived = p1.deriv()
        max = p1_derived.r[0]
        dac = int(trafo.apply(max)/max_voltage*1023) # round down
        val = dac*1./1023*max_voltage
        max = np.min(trafo.find_real_roots(val, False)) # use lower value
        assert( min_voltage <= trafo.apply(max) <= max_voltage)
    if np.min(min) < 0:
        dac = int(trafo.apply(0.)/max_voltage*1023) + 1 # lower bound must be positive
        val = dac*1./1023*max_voltage
        min = np.min(trafo.find_real_roots(val, False)) # use lower value
    return (min, max)


if __name__ == "__main__":

    # tau_refrac
    param = pyhalbe.HICANN.neuron_parameter.I_pl
    domain = extract_domain_tau_refrac(nc, param)
    print(param, domain)

    # tau_w
    param = pyhalbe.HICANN.neuron_parameter.I_radapt
    domain = extract_domain_tau_w(nc, param)
    print(param, domain)

    # tau_syn
    param = pyhalbe.HICANN.neuron_parameter.V_syntcx
    domain = extract_domain_tau_syn(nc,param)
    print(param, domain)

    param = pyhalbe.HICANN.neuron_parameter.V_syntci
    domain = extract_domain_tau_syn(nc,param)
    print(param, domain)

    # voltages
    params = [nrn_param.E_l, nrn_param.E_synx, nrn_param.E_syni,nrn_param.V_exp, nrn_param.V_t]
    for param in params:
        domain = extract_domain_voltage(nc,param)
        print(param, domain)

    # currents
    # I_gl
    param = nrn_param.I_gl
    domain = extract_domain_current(nc,param)
    print(param, domain)
    # find minimum
    current = nc.at(param).apply(0)
    dac = int(current/max_current*1023)+1
    print(dac)
    current = max_current*dac/1023
    min = nc.at(param).find_real_roots(current,False)
    print("min:", min)


    # I_gladapt
    param = nrn_param.I_gladapt
    domain = extract_domain_current(nc,param)
    print(param, domain)

    # I_fire (b)
    param = nrn_param.I_fire
    domain = extract_domain_current(nc,param)
    print(param, domain)
    # find minimum
    current = nc.at(param).apply(0)
    dac = int(current/max_current*1023)+1
    print(dac)
    current = max_current*dac/1023
    min = nc.at(param).find_real_roots(current,False)
    print("min:", min)

    # I_rexp delta_T
    param = nrn_param.I_rexp
    domain = extract_domain_current(nc,param)
    print(param, domain)
    print(nc.at(param))

