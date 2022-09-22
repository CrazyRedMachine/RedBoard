divaio_dll = shared_library(
    'divaio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,    
    vs_module_defs : 'divaio.def',
    c_pch : '../utils/precompiled.h',	
    dependencies : [
        hid_lib,
        setupapi_lib,
    ],
    sources : [
        'divaio.c',
        'divaio.h',
        '../utils/config.c',
        '../utils/config.h',
        '../utils/hid_impl.c',
        '../utils/hid_impl.h',
    ],
)

executable('divatest', 'divatest.c', link_with : divaio_dll)