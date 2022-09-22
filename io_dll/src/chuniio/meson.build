chuniio_dll = shared_library(
    'chuniio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,    
    vs_module_defs : 'chuniio.def',
    c_pch : '../utils/precompiled.h',	
    dependencies : [
        hid_lib,
        setupapi_lib,
    ],
    sources : [
        'chuniio.c',
        'chuniio.h',
        '../utils/config.c',
        '../utils/config.h',
        '../utils/hid_impl.c',
        '../utils/hid_impl.h',
    ],
)

executable('chunitest', 'chunitest.c', link_with : chuniio_dll)