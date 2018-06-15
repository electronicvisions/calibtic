#!/usr/bin/env python


def depends(ctx):
    # not actually a dependency, but build tests by default
    ctx.recurse('test')

    ctx('rant')
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
        cpp_standard = 'c++14',
        uselib_store='BOOST4CALIBTIC')


def build(bld):

    bld(
        target='calibtic_inc',
        export_includes=['.'],
        use=[
            'boost_serialization',
        ]
    )

    bld.shlib(
        target='calibtic',
        source=bld.path.ant_glob('calibtic/**/*.cpp'),
        use=[
            'BOOST4CALIBTIC',
            'DL4CALIBTIC',
            'LOG4CALIBTIC',
            'calibtic_inc',
            'rant',
            'pywrap',
        ],
        install_path='${PREFIX}/lib',
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
