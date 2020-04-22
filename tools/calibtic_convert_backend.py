#!/usr/bin/env python

import argparse
import os

import pycalibtic
import pylogging
pylogging.default_config(date_format='absolute')
pylogging.set_loglevel(pylogging.get("Default"), pylogging.LogLevel.INFO)
pylogging.set_loglevel(pylogging.get("Calibtic"), pylogging.LogLevel.DEBUG)

from pysthal.command_line_util import add_default_coordinate_options

def get_calibtic_name(wafer, hicann):
    wafer_id = wafer.value()
    hicann_id = hicann.toEnum().value()
    name = "w{}-h{}".format(int(wafer_id), int(hicann_id))
    return name

def load_backend(path, ext):
    """
    path: path to calibration files
    ext: extension of files aka backend type
    """

    if ext == "xml":
        backend_type = "xml"
    elif ext == "dat":
        backend_type = "binary"
    elif ext == "txt":
        backend_type = "text"
    else:
        raise RuntimeError("no backend for extensions: {}".format(ext))

    lib = pycalibtic.loadLibrary('libcalibtic_{}.so'.format(backend_type))
    backend = pycalibtic.loadBackend(lib)
    backend.config('path', path)
    backend.init()
    return backend

def main(inpath, inext, outpath, outext, wafer, hicann):
    """
    converts inname to outname based on filename extensions
    """

    in_backend = load_backend(inpath, inext)
    out_backend = load_backend(outpath, outext)

    in_hc = pycalibtic.HICANNCollection()
    in_md = pycalibtic.MetaData()

    in_backend.load(get_calibtic_name(wafer, hicann), in_md, in_hc)
    out_backend.store(get_calibtic_name(wafer, hicann), in_md, in_hc)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('inpath')
    parser.add_argument('inext')
    parser.add_argument('outpath')
    parser.add_argument('outext')
    add_default_coordinate_options(parser)

    args = parser.parse_args()

    if args.hicann == None or args.wafer == None:
        print "error: wafer and hicann have to be specified, e.g. --wafer 0 --hicann 276"
        exit(1)

    main(args.inpath, args.inext, args.outpath, args.outext, args.wafer, args.hicann)
