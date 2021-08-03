"""
Converts ADCCalibration stored via ADCCalibration.store to
calibrations stored via storeADCCalibration
"""
import argparse
import glob
import pycalibtic
import re
from pyhalco_hicann_v2 import ADC

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('backend_path', type=str)
    args = parser.parse_args()

    lib = pycalibtic.loadLibrary('libcalibtic_xml.so')
    backend = pycalibtic.loadBackend(lib)
    backend.config('path', args.backend_path)
    backend.init()

    for calib in glob.glob('adc-*.xml'):
        print("*" * 80)
        adc = ADC(re.match(r'adc-([\w\d]+)\.xml', calib).group(1))
        print(adc)
        calib = pycalibtic.ADCCalibration()
        meta = calib.load(backend, adc)
        qcalib = pycalibtic.ADCCalibration.convertToQuadraticADCCalibration(calib)
        print(meta)
        print(qcalib)
        pycalibtic.storeADCCalibration(backend, qcalib, meta, adc)
        meta = pycalibtic.MetaData()
        pycalibtic.loadADCCalibration(backend, meta, adc)
        print(meta)
        print(qcalib)

if __name__ == '__main__':
    main()
