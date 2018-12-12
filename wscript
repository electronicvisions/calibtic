#!/usr/bin/env python


def depends(ctx):
    # not actually a dependency, but build tests by default
    ctx.recurse('test')

    ctx('halbe')
    ctx('euter')
    ctx('euter', 'pycellparameters')
    ctx('rant')
    ctx('logger')
    ctx('lib-boost-patches')
    ctx('pythonic')
    ctx('pywrap')


def options(opt):
    opt.load('compiler_cxx')
    opt.load('boost')
    opt.load('documentation')


def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('boost')
    cfg.load('documentation')

    # runtime library loading
    cfg.check_cxx(
        lib='dl',
        uselib_store='DL4CALIBTIC',
        mandatory=True)

    cfg.check_cxx(
        lib='log4cxx',
        uselib_store='LOG4CALIBTIC',
        mandatory=True)

    cfg.check_boost(
        lib='filesystem serialization system',
        uselib_store='BOOST4CALIBTIC')

    cfg.check_boost(
            lib='filesystem serialization system',
            uselib_store='BOOST4CALIBTICBINARY')

    cfg.check_boost(
            lib='filesystem serialization system',
            uselib_store='BOOST4CALIBTICTEXT')

    cfg.check_boost(
            lib='filesystem serialization system',
            uselib_store='BOOST4CALIBTICXML')

    cfg.check_cxx(
            lib=['gsl', 'gslcblas'],
            uselib_store='GSL4CALIBTIC')


def build(bld):

    bld(
        target='calibtic_inc',
        export_includes=['include'],
        use=[
            'boost_serialization',
        ]
    )

    bld.shlib(
        features='cxx cxxshlib',
        target='calibtic',
        source=bld.path.ant_glob('src/calibtic/**/*.cpp'),
        use=[
            'BOOST4CALIBTIC',
            'DL4CALIBTIC',
            'LOG4CALIBTIC',
            'calibtic_inc',
            'rant',
            'pywrap',
        ],
        # gccxml requires non-variadic implementation of boost::variant for python wrappers
        defines='BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES',
        export_defines='BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES',
        install_path='${PREFIX}/lib',
    )

    bld(
            target          = 'hmf_calibration',
            features     = 'cxx cxxshlib',
            source          = bld.path.ant_glob('src/HMF/**/*.cpp'),
            use             = [
                'GSL4CALIBTIC',
                'calibtic',
                'halbe_container',
                'euter',
                'pythonic',
                'cellparameters_access',
                'logger_obj',
                ],
            install_path    = '${PREFIX}/lib',
    )

    for tool in bld.path.parent.ant_glob('tools/*.cpp'):
        bld(
            target       = os.path.splitext(tool.name)[0],
            features     = 'cxx cxxprogram',
            source       = tool,
            use          = ['hmf_calibration'],
            install_path = '${PREFIX}/bin/tools',
        )

    for tool in bld.path.parent.ant_glob('tools/*.py'):
        bld.install_as(os.path.join('${PREFIX}/bin',
                                    os.path.splitext(tool.name)[0]),
                       tool,
                       chmod=0755)

    bld.shlib(
            features='cxx cxxshlib',
            target          = 'calibtic_binary',
            source          = bld.path.ant_glob('src/backends/binary/*.cpp'),
            use             = [
                'BOOST4CALIBTICBINARY',
                'calibtic',
                'hmf_calibration',
                ],
            includes        = '.',
            install_path    = '${PREFIX}/lib',
    )

    bld.shlib(
            features='cxx cxxshlib',
            target          = 'calibtic_text',
            source          = bld.path.ant_glob('src/backends/text/*.cpp'),
            use             = [
                'BOOST4CALIBTICTEXT',
                'calibtic',
                'hmf_calibration',
                ],
            includes        = '.',
            install_path    = '${PREFIX}/lib',
    )

    flags = {
            "lib" : [ 'dl', ],
    }

    bld.shlib(
            features        = 'cxx cxxshlib',
            target          = 'calibtic_xml',
            source          = bld.path.ant_glob('src/backends/xml/*.cpp'),
            use             = [
                'BOOST4CALIBTICXML',
                'calibtic',
                'hmf_calibration',
                ],
            includes        = '.',
            install_path    = '${PREFIX}/lib',
            **flags
    )


def doc(dcx):
    '''build documentation (doxygen)'''

    dcx(
            features    = 'doxygen',
            doxyinput   = ['HMF','backends','calibtic'], # overrides doxy-par: INPUT (list of paths)
            doxyoutput  = "doc",                         # overrides doxy-par: OUTPUT_DIRECTORY, (a path)
            pdffile     = 'HMF_calibtic-manual.pdf',     # a pdf file to generate, relative to OUTPUT_DIRECTORY
            doxydebug   = True,                          # generate debug output (the final pars to OUTPUT_DIRECTORY/doxypars.debug)
    )
