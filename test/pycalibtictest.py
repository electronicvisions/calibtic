#!/usr/bin/env python

import unittest
import pycalibtic as cal
import pyhalbe
import pycellparameters as cell
import numpy as np
import pywrapstdvector

# http://stackoverflow.com/a/22726782/1350789
from contextlib import contextmanager
import tempfile
import shutil
import itertools

import pylogging
pylogging.default_config()

@contextmanager
def TemporaryDirectory():
    name = tempfile.mkdtemp()
    try:
        yield name
    finally:
        shutil.rmtree(name)

def initBackend(fname):
    lib = cal.loadLibrary(fname)
    backend = cal.loadBackend(lib)

    if not backend:
        raise Exception('unable to load %s' % fname)

    return backend


def loadXMLBackend(path='build'):
    backend = initBackend('libcalibtic_xml.so')
    backend.config('path', path)
    backend.init()
    return backend

class TestPyCalibtic(unittest.TestCase):

    def test_RegressionUpcast(self):

        with TemporaryDirectory() as tmp_dir:

            backend = loadXMLBackend(tmp_dir)

            nc0 = cal.NeuronCollection()
            tmp = cal.NeuronCalibration()
            tmp.setDefaults()
            nc0.insert(0, tmp)

            backend.store("fisch", cal.MetaData(), nc0)

            nc1 = cal.NeuronCollection()
            md = cal.MetaData()
            backend.load("fisch", md, nc1)

            self.assertEqual(nc0.at(0).size(), nc1.at(0).size())

    def test_Printer(self):
        def match(className):
            cl = getattr(cal, className)
            self.assertRegexpMatches(cl().__str__(), '^%s:.*' % className)

        for cl in ['Collection', 'Calibration', 'NeuronCalibration',
                   'NeuronCollection', 'ADCCalibration', 'Constant', 'Polynomial',
                   'MetaData']:
            match(cl)

    def test_NeuronCalibration(self):

        with TemporaryDirectory() as tmp_dir:

            backend = loadXMLBackend(tmp_dir)

            nc0 = cal.NeuronCollection()
            nc0.setSpeedup(int(1e4))
            nc0.setStartingCycle(42)
            tmp = cal.NeuronCalibration()
            tmp.setDefaults()
            nc0.insert(0, tmp)

            self.assertLess(0, tmp.at(pyhalbe.HICANN.neuron_parameter.E_l).apply(0.5))

            backend.store("fisch", cal.MetaData(), nc0)

            nc1 = cal.NeuronCollection()
            md = cal.MetaData()
            backend.load("fisch", md, nc1)
            self.assertEqual(int(1e4), nc1.getSpeedup())
            self.assertEqual(42, nc0.getStartingCycle())

            bio = cell.EIF_cond_exp_isfa_ista()

            hwparam0 = nc0.applyNeuronCalibration(
                bio, 0, cal.NeuronCalibrationParameters())
            hwparam1 = nc1.applyNeuronCalibration(
                bio, 0, cal.NeuronCalibrationParameters())

            for idx in range(cal.HWNeuronParameter.size()):
                self.assertEqual(hwparam0.getParam(idx), hwparam1.getParam(idx),
                                 "HWNeuronParameter[{}]: {} != {}".format(
                                 idx, hwparam0.getParam(idx), hwparam1.getParam(idx)))

    def test_ADCDatatypes(self):
        dp = cal.DataPoint(1, 2, 3)
        self.assertEqual(dp.ref, 1.0)
        self.assertEqual(dp.mean, 2.0)
        self.assertEqual(dp.std, 3.0)

        vm = cal.VoltageMeasurement()
        self.assertEqual(vm.size(), 0)
        vm.push_back(dp)
        self.assertEqual(vm.size(), 1)
        vm.push_back(dp)
        self.assertEqual(vm.size(), 2)

    def test_ADCCalibration(self):
        """Tests ADCCalibration functionality."""

        # generate fake calibration measurement
        vm = cal.VoltageMeasurement()
        for i in range(1, 9):
            dp = cal.DataPoint(0.2 + 0.01*i, i, 0.0001)
            vm.push_back(dp)

        # generate fake raw data to be converted to voltage
        data = pywrapstdvector.Vector_UInt16(np.array([1, 5, 3], dtype='uint16'))

        # create fresh calibration object
        adc = cal.ADCCalibration()

        # applying calibration on empty object should fail
        channel = pyhalbe.Coordinate.ChannelOnADC(0)
        self.assertRaises(RuntimeError, adc.apply, channel, data)

        # generate polynomials for each channel
        c_gnd = pyhalbe.Coordinate.ChannelOnADC(-1)
        for cc in pyhalbe.Coordinate.iter_all(pyhalbe.Coordinate.ChannelOnADC):
            if cc == c_gnd:
                # skip GND "channel"
                continue
            adc.makePolynomialTrafo(cc.value(), vm)

        # convert raw data to voltages
        res = adc.apply(channel, data)

        # check result
        self.assertAlmostEqual(res[0], 0.21, places=2)
        self.assertGreater(res[1], res[2])

        with TemporaryDirectory() as tmp_dir:

            backend = loadXMLBackend(tmp_dir)
            md = cal.MetaData()
            id = pyhalbe.Coordinate.ADC("0")
            adc.store(backend, md, id)

            adc2 = cal.ADCCalibration()
            md2 = adc2.load(backend, id)
            del md2

            res = adc2.apply(channel, data)
            self.assertAlmostEqual(res[0], 0.21, places=2)
            self.assertGreater(res[1], res[2])

            data = np.array([1, 5, 3], dtype=np.ushort)
            res = adc.apply(channel, data)
            self.assertAlmostEqual(res[0], 0.21, places=2)
            self.assertGreater(res[1], res[2])

    def test_HICANNCollection(self):
        """Write HICANNCollection to backend and read it back, compare"""

        with TemporaryDirectory() as tmp_dir:

            backend = loadXMLBackend(tmp_dir)

            # create HICANNCollection, fill in defaults
            hc0 = cal.HICANNCollection()
            hc0.setDefaults()
            nc0 = hc0.atNeuronCollection()
            bc0 = hc0.atBlockCollection()
            sc0 = hc0.atSynapseRowCollection()

            # store collection
            backend.store("frosch", cal.MetaData(), hc0)

            # create objects filled by backend
            hc1 = cal.HICANNCollection()
            md = cal.MetaData()

            # load back collection
            backend.load("frosch", md, hc1)
            nc1 = hc1.atNeuronCollection()
            bc1 = hc1.atBlockCollection()
            sc1 = hc1.atSynapseRowCollection()

            # check that all collections exist
            self.assertIsInstance(nc0.at(0).size(), int)
            self.assertIsInstance(nc1.at(0).size(), int)
            self.assertIsInstance(bc0.at(0).size(), int)
            self.assertIsInstance(bc1.at(0).size(), int)
            self.assertIsInstance(sc0.at(0).size(), int)
            self.assertIsInstance(sc1.at(0).size(), int)

            # check that collections are of equal size after loading from backend
            self.assertEqual(nc0.at(0).size(), nc1.at(0).size(), "NeuronCollection was not restored from backend")
            self.assertEqual(bc0.at(0).size(), bc1.at(0).size(), "BlockCollection was not restored from backend")
            self.assertEqual(sc0.at(0).size(), sc1.at(0).size(), "SynapseRowCollection was not restored from backend")

            # TODO more assertions

    def test_SynapseCalibration(self):
        sc = cal.SynapseCalibration()
        sc.setDefaults()
        self.assertEqual(sc.size(), 1)
        coeffs = pywrapstdvector.Vector_Double([0,1])
        sc.reset(0,cal.Polynomial(coeffs))
        self.assertEqual(sc.getMinAnalogWeight(), 0)
        self.assertEqual(sc.getMaxAnalogWeight(), 15)

    def test_GmaxConfig(self):
        gc = cal.GmaxConfig(2,3)
        self.assertEqual(gc.get_sel_Vgmax(), 2)
        self.assertEqual(gc.get_gmax_div(), 3)

        gc2 = cal.GmaxConfig.Default()
        self.assertEqual(gc2.get_sel_Vgmax(), 0)
        self.assertEqual(gc2.get_gmax_div(), 11)

    def test_SynapseRowCalibration(self):
        src = cal.SynapseRowCalibration()

        self.assertEqual(src.size(), 0)
        src.setDefaults()
        self.assertEqual(src.size(), 1)

        gc_default = cal.GmaxConfig.Default()
        self.assertEqual(src.findBestGmaxConfig(1.), gc_default)

    def test_SynapseRowCollection(self):
        src = cal.SynapseRowCollection()
        src.setDefaults()
        self.assertEqual(src.size(), 448)

    def test_EssDefaultTrafo(self):
        """
        tests the default trafo for the ESS:
        checks whether it provides a plausible range as promised to the modelers
        """
        src = cal.SynapseRowCalibration()
        src.setEssDefaults()
        self.assertEqual(src.size(), 4)
        # should contain the same trafo as normal default
        src2 = cal.SynapseRowCalibration()
        src2.setDefaults()
        gc_default = cal.GmaxConfig.Default()
        self.assertTrue(src.exists(gc_default))
        self.assertTrue(src2.exists(gc_default))
        self.assertEqual(src.at(gc_default), src2.at(gc_default) )

        # check that default calib covers a big weight range
        min_hw_weight = src.at(gc_default).getMinAnalogWeight()
        max_hw_weight = src.at(gc_default).getMaxAnalogWeight()
        for selgmax in range(4):
            for gmax_div in range(1,16):
                gc = cal.GmaxConfig(selgmax,gmax_div)
                if (src.exists(gc)):
                    sc = src.at(gc)
                    if sc.getMinAnalogWeight() < min_hw_weight:
                        min_hw_weight = sc.getMinAnalogWeight()
                    if sc.getMaxAnalogWeight() > max_hw_weight:
                        max_hw_weight = sc.getMaxAnalogWeight()
        # test if weight range goes over more than 2 magnitudes
        self.assertTrue(max_hw_weight/min_hw_weight > 100.)

        # check that maximum hw weight corresponds to the 300 nS promised to modelers in:
        # https://brainscales-r.kip.uni-heidelberg.de/projects/bss-neuron-parameters
        # which assumes a bio cap of 0.2 nF
        promised_max_bio_weight = 100. # nS // BV: changed from 300 to 100 nS
        big_hw_cap = 2.16456e-3 # nF (NeuronCalibration.cpp)
        cm_bio =  0.2 # nF  (as defined in bss-neuron-parameters)
        speedup=10000.
        def transform_conductance_to_bio(value, hw_cap, bio_cap, speedup):
            return value/speedup*bio_cap/hw_cap
        max_bio_weight = transform_conductance_to_bio(max_hw_weight, big_hw_cap, cm_bio,speedup)
        self.assertAlmostEqual(promised_max_bio_weight, max_bio_weight, delta=0.1*promised_max_bio_weight) # +-10%


    def test_StoreSynapseCalibration(self):
        """
        typical storage stuff for one HICANN quadrant (top left)
        Here: 4 different gmax values, but always the same divisor (11)
        """
        hc = cal.HICANNCollection()
        sr_coll = hc.atSynapseRowCollection()
        # vmax values
        vmax0 = 100
        vmax1 = 150
        vmax2 = 200
        vmax3 = 250
        # configure  floating gates, hardware etc.
        divisor=11
        # measure for many different settings
        for sel_gmax in range(4):
            for driver in range(56):
                for line in range(2):
                    row_address = 2+ driver*2 + line
                    # measure and create synapse calibration function
                    coeffs = pywrapstdvector.Vector_Double([1,2,3])
                    sc = cal.SynapseCalibration()
                    sc.reset(0,cal.Polynomial(coeffs))
                    # get SynapseRowCalibration
                    if sr_coll.exists(row_address):
                        sr_calib = sr_coll.at(row_address)
                    else:
                        sr_calib = cal.SynapseRowCalibration()
                        sr_coll.insert(row_address,sr_calib)
                    # store with used gmax config
                    gmax_config = cal.GmaxConfig(sel_gmax,divisor)
                    if not sr_calib.exists(gmax_config):
                        sr_calib.insert(gmax_config, sc)
                    else:
                        print "Warning, calibration for synapse row ", row_address, "gmax_config", gmax_config, "already exists."
                        print "Existing calib is not overwritten"
        self.assertEqual(sr_coll.size(), 112) # one quadrant = 112 synapse rows
        self.assertEqual(sr_coll.at(2).size(),4) # 4 sel_gmax settings

    def test_ApplySynapseCalibration(self):
        """
        typical usage of synapse calibration:
        here shown for one synapse row [42].
        STP is not yet considered yet
        """
        hc0 = cal.HICANNCollection()
        sc0 = hc0.atSynapseRowCollection()
        sc0.setDefaults()
        for synapse_row in [42]:
            # 100 random synapses of a row are used and have weights uniformly distributed in [0.001 and 0.01]
            analog_weights = np.zeros(256) # analog weights on hw
            n_used_syns = 100
            w = np.random.uniform(0.001,0.01,n_used_syns) # generate random weights in nano Siemens
            random_indices = np.array(range(256))
            np.random.shuffle(random_indices)
            analog_weights[random_indices[:n_used_syns]] = w

            # now happens the trafo
            sr_calib = sc0.at(synapse_row)
            # search for gmax config with best match (requires max analog weight per row)
            gmax_cfg = sr_calib.findBestGmaxConfig(analog_weights.max())
            s_calib = sr_calib.at(gmax_cfg)
            digital_weights = np.zeros(256, dtype=int)
            for n,aw in enumerate(analog_weights):
                if aw > 0:
                    digital_weights[n] = s_calib.getDigitalWeight(aw)
            # set row_config and digital_weights on hw
            selgmax = gmax_cfg.get_sel_Vgmax()
            gmax_div = gmax_cfg.get_gmax_div()
            digital_weights

    def test_SumOfTrafos(self):
        """
        construct from a list of other transformations
        """
        s = cal.SumOfTrafos([cal.Constant(3), cal.Constant(4)])
        self.assertEqual(s.apply(1), 7)

    def test_PolynomialOutsideDomainBehavior(self):
        """
        test the different behaviors THROW, CLIP and IGNORE when the input
        value is outside the domain of a Polynomial.
        """
        poly1 = cal.Polynomial([0,0,1],0.,3.)
        poly2 = cal.Polynomial([0,0,1])

        # clipping is set per default
        self.assertEqual(poly1.apply(4.), poly1.apply(3.))
        self.assertEqual(poly1.apply(-4.), poly1.apply(0.))

        # throw exception
        # OutsideDomainException is not wrapped, checking for msg
        self.assertRaisesRegexp(RuntimeError, "outside of domain.", poly1.apply, 4., cal.Transformation.THROW)

        # ignore domain
        self.assertEqual(poly1.apply(4., cal.Transformation.IGNORE), poly2.apply(4., cal.Transformation.IGNORE))

    def test_PowerOfTrafo(self):
        """
        construct from a one other transformation and take the square root
        """
        p = cal.PowerOfTrafo(0.5, cal.Constant(2))
        self.assertAlmostEqual(p.apply(2), 1.4142135, places=6)

    def test_ideal_calibration(self):

        nc = cal.NeuronCalibration()
        nc.setDefaults()

        for dac in xrange(1024):
            ideal_volt = float(dac)/nc.max_fg_value*nc.max_techn_volt
            self.assertEqual(nc.ideal_volt_to_dac(ideal_volt), dac)

        params = cal.NeuronCalibrationParameters()
        params.alphaV = 10
        params.shiftV = 1.2

        pcp = cell.IF_cond_exp()
        pcp.v_thresh = -50
        pcp.v_rest = -60
        pcp.e_rev_E = -70
        pcp.e_rev_I = -80
        pcp.tau_refrac = 10
        pcp.tau_syn_E = 147
        pcp.tau_syn_I = 17.47

        hw_params = nc.applyNeuronCalibration(pcp, 10000, params)

        # compare with scaling + ideal volt-to-dac or best-guess transformation
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_t"]), 398)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["E_l"]), 341)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["E_synx"]), 284)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["E_syni"]), 227)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_pl"]), 16)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_convoffi"]), 1023)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_convoffx"]), 1023)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_convi"]), 1023)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_convx"]), 1023)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_intbbi"]), 511)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_intbbx"]), 511)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_syni"]), 511)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_synx"]), 511)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["I_spikeamp"]), 1023)

        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_syntci"]), 408)
        self.assertEqual(hw_params.getParam(pyhalbe.HICANN.neuron_parameter.names["V_syntcx"]), 169)

    def test_reverse_apply_reverse_domains(self):

        nc = cal.NeuronCalibration()
        nc.setDefaults()

        for c in ["V_t", "E_l", "E_synx", "E_syni", "I_pl",
                  "I_gl_slow0_fast0_bigcap0", "V_syntcx", "V_syntcx"]:

            calib = nc.at(cal.NeuronCalibrationParameters.Calibrations.calib.names[c])
            reverse_domain = calib.getReverseDomainBoundaries()

            at_lower_boundary = calib.reverseApply(reverse_domain.first)
            at_upper_boundary = calib.reverseApply(reverse_domain.second)

            below_boundary = calib.reverseApply(reverse_domain.first-1, cal.Transformation.CLIP)
            above_boundary = calib.reverseApply(reverse_domain.second+1, cal.Transformation.CLIP)

            self.assertEqual(at_lower_boundary, below_boundary)
            self.assertEqual(at_upper_boundary, above_boundary)

    def test_NeuronCalibrationParameters(self):

        params = cal.NeuronCalibrationParameters()

        for bc, s, f in itertools.product([True,False],
                                          [True,False],
                                          [True,False]):

            params.I_gl_slow = s
            params.I_gl_fast = f
            params.I_gladapt_slow = s
            params.I_gladapt_fast = f
            params.I_radapt_slow = s
            params.I_radapt_fast = f
            params.bigcap = bc

            # check that the correct enum is returned for the
            # current bigcap / speedup combination

            self.assertEqual(getattr(cal.NeuronCalibrationParameters.Calibrations,
                                     "I_gl_slow{}_fast{}_bigcap{}".format(int(s),
                                                                          int(f),
                                                                          int(bc))),
                             params.I_gl())

            self.assertEqual(getattr(cal.NeuronCalibrationParameters.Calibrations,
                                     "I_gladapt_slow{}_fast{}_bigcap{}".format(int(s),
                                                                               int(f),
                                                                               int(bc))),
                             params.I_gladapt())

            self.assertEqual(getattr(cal.NeuronCalibrationParameters.Calibrations,
                                     "I_radapt_slow{}_fast{}_bigcap{}".format(int(s),
                                                                              int(f),
                                                                              int(bc))),
                             params.I_radapt())

            # check that the hardware parameter is give the same int value for all
            # current bigcap / speedup combinations

            self.assertEqual(cal.NeuronCalibrationParameters().Calibrations_to_HW[int(getattr(cal.NeuronCalibrationParameters.Calibrations, "I_gl_slow{}_fast{}_bigcap{}".format(int(s), int(f), int(bc))))],
                             pyhalbe.HICANN.neuron_parameter.I_gl)

            self.assertEqual(cal.NeuronCalibrationParameters().Calibrations_to_HW[int(getattr(cal.NeuronCalibrationParameters.Calibrations, "I_gladapt_slow{}_fast{}_bigcap{}".format(int(s), int(f), int(bc))))],
                             pyhalbe.HICANN.neuron_parameter.I_gladapt)

            self.assertEqual(cal.NeuronCalibrationParameters().Calibrations_to_HW[int(getattr(cal.NeuronCalibrationParameters.Calibrations, "I_radapt_slow{}_fast{}_bigcap{}".format(int(s), int(f), int(bc))))],
                             pyhalbe.HICANN.neuron_parameter.I_radapt)


if __name__ == '__main__':
    unittest.main()
