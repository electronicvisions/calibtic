#!/usr/bin/env python

import unittest
import pycalibtic as cal
import shutil
import tempfile


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


class TestIssue(unittest.TestCase):
    def setUp(self):
        self.backend_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.backend_dir)

    def test_BackendRestoreFactors(self):
        """Are factors written to backend and can be loaded again?"""
        backend = loadXMLBackend(self.backend_dir)

        speedup = 1234
        pll = 500
        startingcycle = 890

        nc0 = cal.NeuronCollection()
        nc0.setSpeedup(speedup)
        self.assertEqual(speedup, nc0.getSpeedup())
        nc0.setPLLFrequency(pll)
        nc0.setStartingCycle(startingcycle)
        backend.store("issue1166", cal.MetaData(), nc0)

        nc1 = cal.NeuronCollection()
        md = cal.MetaData()
        backend.load("issue1166", md, nc1)

        self.assertEqual(nc1.getSpeedup(), speedup)
        self.assertEqual(nc1.getPLLFrequency(), pll)
        self.assertEqual(nc1.getClock(), pll/4.)
        self.assertEqual(nc1.getStartingCycle(), startingcycle)


if __name__ == '__main__':
    unittest.main()
