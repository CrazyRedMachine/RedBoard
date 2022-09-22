# RedBoard IO DLLs

## Overview

These are the IO dlls (aimeio, chuniio, divaio) as well as test binaries. 

## How to build

1. Install MSVC (build tools), meson & ninja
2. run build_chuni.bat, build32.bat and build64.bat 
3. retrieve your files in bin folder

Note: one might have to update the path for vcvarsall.bat in both build32.bat and build64.bat, 
I'm using MSVC 2022 so it currently is `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\` for me

## How to use

### Test tools

Place `chuniio.dll` and `chunitest.exe` in a folder and run.

Place `aimeio.dll` and `aimetest.exe` in a folder and run.

### Chuni

Place `chuniio.dll` and `aimeio.dll` in game bin folder, and add the following to `segatools.ini`

Make sure `[aime]` `[io3]` and `[slider]` do **not** contain the line `enable=0`
(RedBoard acts as a usb controller, not as native hardware anymore)

```
[chuniio]
path=chuniio.dll
;tower_color_active=#fe00fe
;tower_color_inactive=#00fefe

[aimeio]
path=aimeio.dll
```

### Chusan

Place `chusanio_x86.dll` `chusanio_x64.dll` and `aimeio_x64.dll` in game bin folder, and add the following to `segatools.ini`

Make sure `[aime]` `[io4]` and `[slider]` do **not** contain the line `enable=0`
(RedBoard acts as a usb controller, not as native hardware anymore)

```
[chuniio]
path32=chusanio_x86.dll
path64=chusanio_x64.dll
;tower_color_active=#fe00fe
;tower_color_inactive=#fefe00

[aimeio]
path64=aimeio_x64.dll
```

### Diva

```
[divaio]
path=divaio_x64.dll
hid_offset=0xEDA460

[aimeio]
path64=aimeio_x64.dll
```