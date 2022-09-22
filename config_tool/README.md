# RedBoard Config Tool

## Overview

This is the config tool for the redboard. 

It uses PySimpleGUI and hid libraries to display a cool GUI and communicate with the RedBoard via hid feature reports.

The code is probably very aweful to read for a python dev (which I'm not :D), but hey, "it works™️".

## How to run

Install required packages found in requirements.txt then run the .py script. You need `hidapi.dll` in the same folder.

Alternatively, the release contains a standalone .exe version, which doesn't require python or dependencies to be installed in your system.

Note that the RedBoard has to be plugged and recognized for this config tool to work.