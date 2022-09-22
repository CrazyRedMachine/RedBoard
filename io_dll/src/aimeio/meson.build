aimeio_dll = shared_library(
    'aimeio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,    
    vs_module_defs : 'aimeio.def',
    c_pch : '../utils/precompiled.h',	
    dependencies : [
        hid_lib,
        setupapi_lib,
    ],
    sources : [
        'aimeio.c',
        'aimeio.h',
        '../utils/config.c',
        '../utils/config.h',
        '../utils/hid_impl.c',
        '../utils/hid_impl.h',
    ],
)

executable('aimetest', 'aimetest.c', link_with : aimeio_dll)