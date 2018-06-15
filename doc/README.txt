Calibtic offers methods for transforming biological neuron parameters to their matching HW-equivalents.
For this purpose the class "NeuronCalibration" was designed. By using the method "applyNeuronCalibration" of this class,
a set of biological Parameters is transformed to hardware parameters.
This is done by scaling the parameters, then using calibration-routines and finally converting the technical parameters to floating gate values.
The first two operations are based on the thesis "Reproducing Biologically Realistic Regimes on a Highly-Accelerated Neuromorphic Hardware System" 
by M. O. Schwartz and were implemented with slight differences to the methods introduced in this work.
Additionally by calling "inverseNeuronCalibration" a transformation from hardware to biological parameters is at hand.

Expected input of "applyNeuronCalibration": (PyNN-standard??)
BioParameter with vector mParams with components:
    tau_refrac  -> refractory period in [ms]
    a           -> adaptation constant in [nS]
    tau_m       -> membrane time constant in [ms]
    e_rev_E     -> reversal potential of the excitatory synaptic input in [mV]
    i_offset    -> not used
    cm          -> membrane capacitance in [nF]
    delta_T     -> voltage governing the exponential rise in [mV]
    e_rev_I     -> reversal potential of the inhibitory synaptic input in [mV]
    v_thresh    -> soft-threshold in exp. term in [mV]
    b           -> adaptation current is increased by this value every reset in [nA] ->is this PyNN standard??
    tau_syn_E   -> synaptic time constant (excitatory) in [ms]
    v_reset     -> reset voltage in [ms]
    v_spike     -> hard threshold: when membrane potential crosses this value the neuron is reset in [mV]
    v           -> not used
    tau_syn_I   -> synaptic time constant (inhibitory) in [ms]
    tau_w       -> time constant of the adaptation term in [ms]
    w           -> not used
    v_rest      -> resting potential of the neuron in [mV]

"inverseNeuronCalibration" is called with DAC values. Only the following parametwers are transformed.
If not further remarked, their input ranges from 0 to 1023,
    E_l         -> resting potential
    I_gl        -> current controling the leakage conductance. Range: 1 to 1023
    V_t         -> threshold for spike generation
    I_pl        -> current controling the refractory period. Range: 1 to 1023 (very high values for tau_refrac for DAC < 10)
    I_gladapt   -> current controling the leakage conductance of the adaptation term (a)
    I_radapt    -> current controling the time constant of the adaptation term Range: 76 to 1023
    I_fire      -> current controling the reset of the adaptation current
    I_rexp      -> current controling the exponential rise
    V_exp       -> exponential threshold
    E_syni      -> reversal potential of the inhibiory input
    E_synx      -> reversal potential of the excitatory input
    V_syntci    -> time constant of the inhibitory input. Range: 786 to 834
    V_syntcx    -> time constant of the excitatory input. Range: 786 to 834

For detailed information of the transformations/inverse transformations see Mathematica Notebooks in this Folder.

UPDATE: calibration.nb is also available as rendered pdf and as IPython notebook with further explanations
