import pycalibtic as cal
import numpy as np
import pylab as pl

"""
Script to identify a good default set of Synapse Driver Calibration on the ESS
Idea:
Have at least 4 different gmax settings, which are in the range, that we have told the users:
https://brainscales-r.kip.uni-heidelberg.de/projects/bss-neuron-parameters/repository/revisions/master/entry/cells.py

Approach:
We assume that the weights scale proportional to Vgmax/gmax_div, so that we can copy the dependence
on the digital weight from the default trafo from RTL sims.

Then we choose 3 other gmax settings ( vgmax and gmax_div ), so that
1) a big range of weights is supported
2) the maximum weight approximately corresponds to the max bio weight of 300 nS promised to the modelers
UPDATE by BV: I decided to set the max bio weight to 100 nS, after I had put the wrong speedup before.

The found settings are used in "SynapseRowCalibration.setEssDefaults()" to store the corresponding trafos.
"""

promised_maxed_bio_weight=100. # nS

big_hw_cap = 2.16456e-3 # nF (NeuronCalibration.cpp)
cm_bio =  0.2 # nF  (as defined in bss-neuron-parameters)
speedup=10000.
# values from MOS' RTL simulation. for which the default trafo was extracted
vgmax=500. # nA 
gmax_div=11


def transform_conductance_to_hardware(value, hw_cap, bio_cap, speedup):
    return value*speedup*hw_cap/bio_cap

def transform_conductance_to_bio(value, hw_cap, bio_cap, speedup):
    return value/speedup*bio_cap/hw_cap

def compute_scalar(vgmax_new,gmax_div_new, vgmax_old=vgmax,gmax_div_old=gmax_div):
    """assumes linear dependency of weights: w ~ vgmax/gmax_div"""
    return (1./gmax_div_new)/(1./gmax_div_old)*vgmax_new/vgmax_old

required_max_hw_weight = transform_conductance_to_hardware( promised_maxed_bio_weight, big_hw_cap, cm_bio, speedup)

sc = cal.SynapseCalibration()
sc.setDefaults()
print sc
dws = range(16)
aws = []

for dw in dws:
    aws.append( sc.getAnalogWeight(dw) )
aws = np.array(aws)

settings = [
            dict(vgmax=550.,gmax_div=1),
            dict(vgmax=650.,gmax_div=4),
            dict(vgmax=500.,gmax_div=11), # default
            dict(vgmax=200.,gmax_div=15),
            ]

all_weights = []
for s in settings:
    scalar = compute_scalar(s['vgmax'],s['gmax_div'])
    print s, scalar
    pl.plot(dws,aws*scalar,"+",label=str(s))
    all_weights += (aws*scalar).tolist()

max_weight =  np.max(all_weights)
min_weight = np.min(all_weights)
print "HW: Max =",max_weight,"Min =",min_weight,"Ratio =",max_weight/min_weight

max_weight_bio = transform_conductance_to_bio(max_weight, big_hw_cap, cm_bio,speedup)
min_weight_bio = transform_conductance_to_bio(min_weight, big_hw_cap, cm_bio,speedup)
print "Bio: Max =",max_weight_bio,"Min =",min_weight_bio,"Ratio =",max_weight_bio/min_weight_bio

pl.axhline(y=required_max_hw_weight)
pl.semilogy()
pl.xlim(-1,16)
pl.xlabel("digital weight")
pl.ylabel("analog hardware weight [nS]")

pl.legend(loc="upper left")
pl.show()
