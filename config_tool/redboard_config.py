import PySimpleGUI as sg
import os
import ctypes
import sys
import platform
if platform.system() == 'Windows':
	ctypes.CDLL('.\\hidapi.dll')
import hid
import colorsys

ICON = b'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'
COLORWHEEL = b'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'
CROSSHAIR = b'iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAABJ0AAASdAHeZh94AAAAB3RJTUUH5gMWFiEkliwOXAAAAB1pVFh0Q29tbWVudAAAAAAAQ3JlYXRlZCB3aXRoIEdJTVBkLmUHAAAAa0lEQVQoz2OUe2jIAAEiDMjgkfzu/zC23MMIRnR5BgYRBiYGCsAQ1cyCSwISSPgBIwMDw39Mja6MDAzCaKG/4j/tnc3yh4kocxmV7ppCmUJoUsJofCGMRIIzwO4pL4P7UeluFOMwSiQUaQYAj68Se/JzWmMAAAAASUVORK5CYII='

def color_rgb(rainbow, fade, red, green, blue):
	return [rainbow, fade, blue, red, green]

########## Global HID data 

p_hid_mode = [0]
p_rainbow_delay = [1,0x14]
p_rainbow_speed = [0x19]
p_fade_speed = [0x20]
r_hid_mode = [0]
r_rainbow_delay = [1,0x14]
r_rainbow_speed = [0x19]
r_fade_speed = [0x20]

colors = [[0,0,0,0,0],[0,1,0x7f,0,0x23],[0,0,0,0,0xff],color_rgb(0,0,0x23,0,0x7f),
          [0,0,0,0,0],[0,1,0x7f,0,0x23],[0,0,0,0,0xff],color_rgb(0,0,0x23,0,0x7f)]

o_configsize = [50]
o_virtual_card = { "clone_timeout": [0], "type":[0], "uid":[0,0,0,0,0,0,0,0] }
o_aime_scan = { "blink_duration":[0,0], "iterations":[0] }
o_aime_clone = { "blink_duration":[0,0], "iterations":[0] }
o_icca = { "send_once": [1], "keypad_blank_eject": [0], "auto_eject_timer":[0,0,0,0] }
o_serial = { "open_timeout":[0,0,0,0], "nb_retries":[0], "aime_alt_baudrate":[0] }
o_input = { "home_hotkey_delay":[0,0,0,0], "touch_threshold":[0]}
o_touch = { "force_touch":[0], "touch_height":[90], "touch_portrait":[0], "touch_reverse":[0]}	
o_diva = { "base_lamp_state": [0] }
o_hid = { "cardio_cooldown": [0], "reactive_timeout": [0,0,0,0] }
o_hw = { "last_reader":[0], "last_panel":[0], "last_aime_slow":[0], "no_reader":[0], "autodetect":[0]}
o_cardio = { "always_felica":[0]}

def dict_to_list(dictlist):
	return [item for sublist in [dictlist[key] for key in dictlist] for item in sublist]

def listoflist_to_bytes(listoflist):
	flat_list = [item for sublist in listoflist for item in sublist]
	print('[{}]'.format(', '.join(hex(x) for x in flat_list))) #print command in advanced tab
	return bytes(bytearray(flat_list))

reader_type_arr = ['UNK','ICCA','ICCB','ICCC','AIME']
panel_type_arr = ['None','DIVA','Chunithm','Nostalgia']

p_hid_mode_arr = ['-PRADIOHIDIGNORE-', '-PRADIOHIDSWAP-', '-PRADIOHIDCOMBINE-']
r_hid_mode_arr = ['-RRADIOHIDIGNORE-', '-RRADIOHIDSWAP-', '-RRADIOHIDCOMBINE-']

def update_gui():
	window[p_hid_mode_arr[p_hid_mode[0]]].update(value=True)
	prainbow=(p_rainbow_delay[1]*256+p_rainbow_delay[0])/100
	window['-PRAINBOWDELAY-'].update(value=prainbow)
	window["-PRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(prainbow))+"s")
	prainbow=int.from_bytes(p_rainbow_speed[0:1], byteorder='little', signed=True)
	window['-PRAINBOWSPEED-'].update(value=prainbow)
	window["-PRAINBOWSPEEDTXT-"].update("Rainbow speed: "+str(int(prainbow)))
	window['-PFADEDURATION-'].update(value=p_fade_speed[0]/10)
	window["-PFADEDURATIONTXT-"].update("Fade duration: "+str((p_fade_speed[0]/10))+"s")
	
	window[r_hid_mode_arr[r_hid_mode[0]]].update(value=True)
	rrainbow=(r_rainbow_delay[1]*256+r_rainbow_delay[0])/100
	window['-RRAINBOWDELAY-'].update(value=rrainbow)
	window["-RRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(rrainbow))+"s")
	rrainbow=int.from_bytes(r_rainbow_speed[0:1], byteorder='little', signed=True)
	window['-RRAINBOWSPEED-'].update(value=rrainbow)
	window["-RRAINBOWSPEEDTXT-"].update("Rainbow speed: "+str(int(rrainbow)))
	window['-RFADEDURATION-'].update(value=r_fade_speed[0]/10)
	window["-RFADEDURATIONTXT-"].update("Fade duration: "+str((r_fade_speed[0]/10))+"s")
	
	for i in range(0,8):
		set_color_rgbext(i, colors[i])
	
	window['-ICCASENDONCE-'].update(value=o_icca["send_once"]==[1])
	window['-ICCABLANKEJECT-'].update(value=o_icca["keypad_blank_eject"]==[1])
	window['-ICCAAUTOEJECTTIMER-'].update(value=(int.from_bytes(o_icca["auto_eject_timer"][0:5], byteorder='little')/500)/2)
	window["-ICCAAUTOEJECTTIMERTXT-"].update("Auto eject timer: "+str(((int.from_bytes(o_icca["auto_eject_timer"][0:5], byteorder='little')/500)/2))+"s")
	
	window['-CLONETIMEOUT-'].update(value=o_virtual_card['clone_timeout'])
	window["-CLONETIMEOUTTXT-"].update("Clone mode timeout: "+str((o_virtual_card['clone_timeout'][0]))+"s")

	window['-FELICA-'].update(value=o_virtual_card['type'] == [2])
	window['-CARDUID-'].update(value=''.join('%02x'% i for i in o_virtual_card['uid']))

	window['-SALTBPS-'].update(value=o_serial["aime_alt_baudrate"]==[1])
	window['-SNBRETRIES-'].update(value=o_serial["nb_retries"])
	window['-STIMEOUT-'].update(value=(int.from_bytes(o_serial["open_timeout"][0:5], byteorder='little')/500)/2)
	window["-STIMEOUTTXT-"].update("Open timeout: "+str(((int.from_bytes(o_serial["open_timeout"][0:5], byteorder='little')/500)/2))+"s")
	
	window['-HIDCARDIOCOOLDOWN-'].update(value=(int(o_hid["cardio_cooldown"][0])))
	window["-HIDCARDIOCOOLDOWNTXT-"].update("Cardio cooldown: "+str(o_hid["cardio_cooldown"][0])+"s")
	window['-HIDREACTIVEFALLBACK-'].update(value=(int.from_bytes(o_hid["reactive_timeout"][0:5], byteorder='little')/500)/2)
	window["-HIDREACTIVEFALLBACKTXT-"].update("Reactive fallback: "+str(((int.from_bytes(o_hid["reactive_timeout"][0:5], byteorder='little')/500)/2))+"s")
	
	window['-DIVABUTTONSON-'].update(value=o_diva['base_lamp_state']==[1])
	
	window['-DISABLEREADER-'].update(value=o_hw['no_reader']==[1])
	
	window['-AUTODETECT-'].update(value=o_hw['autodetect']==[1])
	if o_hw['last_aime_slow'] == [1] :
		window['-READERBPSRADIO1-'].update(value=True)
	else:
		window['-READERBPSRADIO2-'].update(value=True)
	window['-READERTYPESPIN-'].update(value=reader_type_arr[o_hw['last_reader'][0]] if o_hw['last_reader'][0]<len(reader_type_arr) else None)
	window['-PANELTYPESPIN-'].update(value=panel_type_arr[o_hw['last_panel'][0]] if o_hw['last_panel'][0]<len(panel_type_arr) else None)
	
	blinkclone = (o_aime_clone['blink_duration'][1]*256+o_aime_clone['blink_duration'][0])/1000
	window['-BLINKCLONE-'].update(value=blinkclone)
	window['-BLINKCLONETXT-'].update(value="Blink on clone: "+str(blinkclone)+"s")
	window['-BLINKCLONEITER-'].update(value=o_aime_clone['iterations'][0])
	
	blinkscan = (o_aime_scan['blink_duration'][1]*256+o_aime_scan['blink_duration'][0])/1000
	window['-BLINKSCAN-'].update(value=blinkscan)
	window['-BLINKSCANTXT-'].update(value="Blink on scan: "+str(blinkscan)+"s")
	window['-BLINKSCANITER-'].update(value=o_aime_scan['iterations'][0])
	
	window['-HOTKEYDURATION-'].update(value=(int.from_bytes(o_input["home_hotkey_delay"][0:5], byteorder='little')/500)/2)
	window["-HOTKEYDURATIONTXT-"].update("Longpress: "+str(((int.from_bytes(o_input["home_hotkey_delay"][0:5], byteorder='little')/500)/2))+"s")
	
	window['-TOUCHTHRESHOLD-'].update(value=(int(o_input["touch_threshold"][0])))
	window["-TOUCHTHRESHOLDTXT-"].update("Threshold: "+str((int(o_input["touch_threshold"][0]))))

	window['-TOUCHFORCE-'].update(value=o_touch['force_touch']==[1])
	window['-TOUCHREVERSE-'].update(value=o_touch['touch_reverse']==[1])
	window['-TOUCHPORTRAIT-'].update(value=o_touch["touch_portrait"]==[1])
	window['-TOUCHHEIGHT-'].update(value=o_touch['touch_height'])
	
	window['-ALWAYSFELICA-'].update(value=o_cardio['always_felica']==[1])
	
def read_from_board():
	global p_hid_mode, p_rainbow_delay, p_rainbow_speed, p_fade_speed, colors, r_hid_mode, r_rainbow_delay, r_rainbow_speed, r_fade_speed, o_aime_clone, o_aime_scan, o_touch, o_diva, o_hid, o_hw, o_icca, o_input, o_serial, o_virtual_card, o_cardio, o_configsize
	#panel
	data = lights.get_feature_report(7, 26)
	p_hid_mode = [data[1]]
	p_rainbow_delay = [data[2], data[3]]
	p_rainbow_speed = [data[4]] 
	p_fade_speed = [data[5]]
	colors[0] = [data[6],data[7],data[8],data[9],data[10]]
	colors[1] = [data[11],data[12],data[13],data[14],data[15]]
	colors[2] = [data[16],data[17],data[18],data[19],data[20]]
	colors[3] = [data[21],data[22],data[23],data[24],data[25]]
	#reader
	data = lights.get_feature_report(8, 26)
	r_hid_mode = [data[1]]
	r_rainbow_delay = [data[2], data[3]]
	r_rainbow_speed = [data[4]]
	r_fade_speed = [data[5]]
	colors[4] = [data[6],data[7],data[8],data[9],data[10]]
	colors[5] = [data[11],data[12],data[13],data[14],data[15]]
	colors[6] = [data[16],data[17],data[18],data[19],data[20]]
	colors[7] = [data[21],data[22],data[23],data[24],data[25]] 
	#options
	data = lights.get_feature_report(9, 51)
	o_configsize = [data[1]]
	o_virtual_card = { "clone_timeout": [data[2]], "type": [data[3]], "uid":[data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]] }
	o_aime_scan = { "blink_duration": [data[12],data[13]], "iterations":[data[14]] }
	o_aime_clone = { "blink_duration": [data[15],data[16]], "iterations":[data[17]] }
	o_icca = { "send_once": [data[18]], "keypad_blank_eject": [data[19]], "auto_eject_timer":[data[20],data[21],data[22],data[23]] }
	o_serial = { "open_timeout":[data[24],data[25],data[26],data[27]], "nb_retries":[data[28]], "aime_alt_baudrate":[data[29]] } 
	o_input = { "home_hotkey_delay":[data[30],data[31],data[32],data[33]], "touch_threshold":[data[34]] }	
	o_touch = { "force_touch":[data[35]], "touch_height":[data[36]], "touch_portrait":[data[37]], "touch_reverse":[data[38]]}
	o_diva = { "base_lamp_state": [data[39]] }
	o_hid = { "cardio_cooldown": [data[40]], "reactive_timeout": [data[41],data[42],data[43],data[44]] }
	o_hw = { "last_reader":[data[45]], "last_panel":[data[46]], "last_aime_slow":[data[47]], "no_reader":[data[48]], "autodetect":[data[49]]}
	o_cardio = { "always_felica":[data[50]] }
	update_gui()
	

def send_to_board():
	window['-ADVANCEDTXT-'].update(value="") #clear advanced tab text as this function will repopulate it
	# panel
	data = listoflist_to_bytes([[7], p_hid_mode, p_rainbow_delay, p_rainbow_speed, p_fade_speed, [item for sublist in colors[0:4] for item in sublist]])
	# print(data)
	if slider != None:
		lights.send_feature_report(data)
	#reader
	data = listoflist_to_bytes([[8], r_hid_mode, r_rainbow_delay, r_rainbow_speed, r_fade_speed, [item for sublist in colors[4:8] for item in sublist]])
	# print(data)
	if slider != None:
		lights.send_feature_report(data)
	#options
	data = listoflist_to_bytes([[9], o_configsize, dict_to_list(o_virtual_card), dict_to_list(o_aime_scan), dict_to_list(o_aime_clone), dict_to_list(o_icca), dict_to_list(o_serial), dict_to_list(o_input), dict_to_list(o_touch), dict_to_list(o_diva), dict_to_list(o_hid), dict_to_list(o_hw), dict_to_list(o_cardio)])
	# print(data)
	if slider != None:
		lights.send_feature_report(data)
	
############

def hsv2rgb(h,s,v):
	rgb_color = tuple(round(i * 255) for i in colorsys.hsv_to_rgb(h,s,v))
	color= '#' + ''.join('%02x'% i for i in rgb_color[:3])
	return color

def open_device(vid):
	itf_list = hid.enumerate(vid, 0x0000) # Note: using pid 0x0000 will match any
	itf_list.sort(key=lambda lst:lst['path']) # Sometimes the interfaces are not installed in correct order
	if len(itf_list) > 2:
		slider = hid.Device(None,None,None,itf_list[0]['path'])
		lights = hid.Device(None,None,None,itf_list[1]['path'])
		sliderName = slider.product
		readerName = hid.Device(None, None, None, itf_list[2]['path']).product
	else:
		slider = None
		lights = None
		sliderName = '     Device not found'
		readerName = 'Reader not found'

	return [sliderName, readerName, slider, lights]

sliderName, readerName, slider, lights = open_device(0x0f0d)

def save_to_flash():
	if slider == None:
		sg.popup_error('No device found')
		return
	data = listoflist_to_bytes([[0x07, 0x73, 0x61, 0x76, 0x65, 0x21, 0x00, 0x00]])
	print(data)
	lights.send_feature_report(data)

override = False #debug feature to simulate device found
showall = False
#utility function to determine whether an element is visible
def isVisible(input):
	if showall:
		return True
	#depends
	if input == 'AIME':
		return readerName == 'Aime Reader'
	if input == 'ICCA':
		return readerName == 'ICCA Reader'
	if input == 'ICCB':
		return readerName == 'ICCB Reader'
	if input == 'ICCC':
		return readerName == 'ICCC Reader'
	if input == 'RNONE':
		return readerName == 'Card Button'
	if input == 'DIVA':
		return sliderName == 'Diva Slider'
	if input == 'CHUN':
		return sliderName == 'Chunithm Slider'
	if input == 'NOST':
		return sliderName == 'Nostalgia Keyboard'
	if input == 'PNONE':
		return sliderName == 'No panel detected'
	if input == 'ADVANCED':
		return False #will only be displayed if showall is true

#is visible if ANY condition from the list works (e.g. )
def isVisibleAny(input):
	for item in input:
		if isVisible(item):
			return True
	return False

#is visible if ALL conditions from the list are visible (e.g. )
def isVisibleAll(input):
	for item in input:
		if not isVisible(item):
			return False
	return True	

def noReaderText():
	return "Enable readers to view extra tabs." if readerDisabled() else "No reader detected."
	
def noDevice():
	return sliderName == '     Device not found'

def readerDisabled():
	return o_hw['no_reader']==[1]
	
#utility function to generate a realtime color picker graph with rainbow and fade
def ext_color(frametitle, framekey, graphkey, inputkey, sliderkey, rainbowkey, fadekey):
	return sg.Frame(frametitle, 
					[ 
						[ 
							sg.Graph((150,150), (0,0),(150,150),background_color="lightblue", key=graphkey, enable_events=True,drag_submits=True), 
							sg.Slider((0,0xff), s=(7,15), key=sliderkey, disable_number_display = True, enable_events=True)
						],
						[ 
							sg.Input('#7f2300', size=(7,1), key=inputkey, enable_events=True), 
							sg.ColorChooserButton('pick')
						],
						[
							sg.Checkbox("Rainbow", key=rainbowkey, enable_events=True),	
							sg.Checkbox("Fade", key=fadekey, enable_events=True)
						] 
					],
					element_justification='c',
					key=framekey
					)

layout_hardware_detection = [
								sg.Frame('Hardware type', 
										[
											[ 
												sg.Checkbox('Autodetect', key='-AUTODETECT-', tooltip="scan for all kind of hardware (last detected will still be scanned first)", enable_events=True) 
											] ,
											[ 
												sg.Text("Panel type:"), 
												sg.Spin(["Chunithm","DIVA","Nostalgia","None"], size=8, key='-PANELTYPESPIN-', enable_events=True)
											],
											[ 
												sg.Text("Reader type:", key='-READERTYPETEXT-', text_color='grey' if readerDisabled() else 'white'), 
												sg.Spin(["ICCC","ICCB","ICCA","AIME"], key='-READERTYPESPIN-', disabled=readerDisabled(), enable_events=True)
											],
											[ 
												sg.Radio("38400bps", 'aime_bps', key='-READERBPSRADIO1-', disabled=readerDisabled(), enable_events=True),
												sg.Radio("115200bps", 'aime_bps', key='-READERBPSRADIO2-', disabled=readerDisabled(), enable_events=True) 
											], 
											[ 
												sg.Checkbox('Disable reader', key='-DISABLEREADER-', enable_events=True) 
											] 
										], expand_x=True
										)
							]

layout_virtual_card = [ 
							sg.Frame('Virtual Card', 
									[
										[
											sg.Input('Card UID',s=(16,1), key='-CARDUID-', enable_events=True),
											sg.Checkbox('FeliCa', key='-FELICA-', enable_events=True)
										],
										[ 
											sg.Slider((0,20), orientation='h', s=(15,15), disable_number_display=True, key='-CLONETIMEOUT-', enable_events=True)
											
										],
										[ sg.Text("Clone mode timeout: 0s", key='-CLONETIMEOUTTXT-') ]
									], expand_x=True
									)
					  ]

layout_cardio = [ 
							sg.Frame('CardIO', 
									[
										[
											sg.Checkbox('Always spoof FeliCa', key='-ALWAYSFELICA-', enable_events=True) 
										]
									], expand_x=True
									)
					  ]
layout_serial = [ 
					sg.Frame('Serial settings', 
							[ 
								[ 
									sg.Slider((0,20), resolution=0.5, orientation='h', s=(15,15), key='-STIMEOUT-', disable_number_display=True, enable_events=True)
								],
								[
									sg.Text("Open timeout: 0.0s", key='-STIMEOUTTXT-')
								],
								[ 
									sg.Text("Retries:"), 
									sg.Spin(["0","1","2","3","4","5","6","7","8","9"], key='-SNBRETRIES-', enable_events=True)
								], 
								[
									sg.Checkbox('Try AIME alt. baudrate', tooltip="Alternate between 38400 and 115200bps when trying to open an AIME reader", key='-SALTBPS-', enable_events=True)
								]
							], expand_x=True
							)
				]

layout_hid = [ 
				sg.Frame('HID global settings', 
						[ 
							[ 
								sg.Slider((0,20), resolution=0.5, orientation='h', s=(15,15), key='-HIDCARDIOCOOLDOWN-', disable_number_display=True, enable_events=True)
							],
							[
								sg.Text("Cardio cooldown: 0.0s", key='-HIDCARDIOCOOLDOWNTXT-')
							],							[ 
								sg.Slider((0,20), resolution=0.5, orientation='h', s=(15,15), key='-HIDREACTIVEFALLBACK-', disable_number_display=True, enable_events=True)
							],
							[
								sg.Text("Reactive fallback: 0.0s", key='-HIDREACTIVEFALLBACKTXT-')
							],							
						], expand_x=True
						)
			 ]
				
layout_options = [ 
					[ 
						sg.Checkbox("Show all options", tooltip='Display options for hardware not matching what\'s currently detected', key='-SHOWALL-', enable_events=True) 
					], 
					layout_virtual_card,
					layout_cardio,
					layout_serial, 
					layout_hid,
					layout_hardware_detection, 
				 ]

layout_panel_lights = [ 
							[
								sg.Text('HID mode :'), 
								sg.Radio("Swap", 'panel_hid_mode', key='-PRADIOHIDSWAP-', default=True, enable_events=True),
								sg.Radio("Combine", 'panel_hid_mode', key='-PRADIOHIDCOMBINE-', enable_events=True),
								sg.Radio("Ignore", 'panel_hid_mode', key='-PRADIOHIDIGNORE-', enable_events=True)
							],
							[
								sg.Slider((0,300), disable_number_display = True,default_value=120, orientation='h', s=(21,15), key='-PRAINBOWDELAY-', enable_events=True),
								sg.Text('Rainbow delay: 10s', key='-PRAINBOWDELAYTXT-')
							], 
							[
								sg.Slider((-127,128),  disable_number_display = True,default_value=15, orientation='h', s=(21,15), key='-PRAINBOWSPEED-', enable_events=True),
								sg.Text('Rainbow speed: 0', key='-PRAINBOWSPEEDTXT-')
							], 
							[
								sg.Slider((0,25.5),  disable_number_display = True, default_value=3, resolution=0.5, orientation='h', s=(21,15), key='-PFADEDURATION-', enable_events=True),
								sg.Text('Fade duration: 0s', key='-PFADEDURATIONTXT-')
							], 
							[
								sg.Frame("", [[
								sg.Slider((0,7),  disable_number_display = True, resolution=0.5, orientation='h', s=(5,15), key='-HOTKEYDURATION-', enable_events=True),
								sg.Text('Longpress: 0s', tooltip="Hold 'select'/'service' for this duration to trigger a 'home'/'test' keypress\nHold card scan button for this duration to enter clone mode\n0 to disable", size=13, key='-HOTKEYDURATIONTXT-')]], border_width=0),
								sg.Frame("", [[
								sg.Slider((0,255),  disable_number_display = True, orientation='h', s=(5,15), key='-TOUCHTHRESHOLD-', enable_events=True),
								sg.Text('Threshold: 20', size=12, key='-TOUCHTHRESHOLDTXT-')]], key="-TOUCHTHRESHOLDFRAME-", border_width=0, visible=not isVisible('NOST'))
							],
							[
								sg.Checkbox("Diva button lights ON by default", tooltip="(reactive mode) buttons will turn OFF on press", key="-DIVABUTTONSON-", enable_events=True)
							],
							[ 
								sg.Frame('',
									[[
										ext_color('Inactive zone', '-PCOLOR1-', '-PGRAPH1-', '-PCOLORINPUT1-','-PSLIDER1-','-PRAINBOW1-','-PFADE1-'),
										ext_color('Active zone', '-PCOLOR2-', '-PGRAPH2-', '-PCOLORINPUT2-','-PSLIDER2-','-PRAINBOW2-','-PFADE2-')
									]], border_width=0, key='-PCOLORALL-')
							], 
							[ 
								sg.Frame('',
									[[
										ext_color('Inactive Towers & Sep', '-PCOLOR4-', '-PGRAPH4-', '-PCOLORINPUT4-','-PSLIDER4-','-PRAINBOW4-','-PFADE4-'),
										ext_color('Active Towers', '-PCOLOR3-', '-PGRAPH3-', '-PCOLORINPUT3-','-PSLIDER3-','-PRAINBOW3-','-PFADE3-')
									]], border_width=0, key='-PCOLORCHUN-')
							]			
					  ]

layout_reader_lights = [ 
							[
								sg.Text('HID mode :'), sg.Radio("Swap", 'reader_hid_mode', key='-RRADIOHIDSWAP-', default=True, enable_events=True),
								sg.Radio("Combine", 'reader_hid_mode', key='-RRADIOHIDCOMBINE-', enable_events=True),
								sg.Radio("Ignore", 'reader_hid_mode', key='-RRADIOHIDIGNORE-', enable_events=True)
							],
							[
								sg.Slider((0,300), disable_number_display = True,default_value=120, orientation='h', s=(21,15), key='-RRAINBOWDELAY-', enable_events=True),
								sg.Text('Rainbow delay: 10s', key='-RRAINBOWDELAYTXT-')
							], 
							[
								sg.Slider((-127,128),  disable_number_display = True,default_value=15, orientation='h', s=(21,15), key='-RRAINBOWSPEED-', enable_events=True),
								sg.Text('Rainbow speed: 0', key='-RRAINBOWSPEEDTXT-')
							], 
							[
								sg.Slider((0,25.5),  disable_number_display = True, default_value=3, resolution=0.5, orientation='h', s=(21,15), key='-RFADEDURATION-', enable_events=True),
								sg.Text('Fade duration: 0s', key='-RFADEDURATIONTXT-')
							], 
							[
								sg.Text('Blink on scan:  0.0s', s=17, tooltip="Hold 'select'/'service' for this duration to trigger a HOME keypress\n0 to disable", key='-BLINKSCANTXT-'),
								sg.Slider((0,2),  disable_number_display = True, resolution=0.01, orientation='h', s=(6,15), key='-BLINKSCAN-', enable_events=True),
								sg.Text("Iterations:"), 
								sg.Spin([str(i) for i in range(1,256)], key='-BLINKSCANITER-', size=3, enable_events=True)			
							],
							[
								sg.Text('Blink on clone: 0.0s', s=17, tooltip="Hold 'select'/'service' for this duration to trigger a HOME keypress\n0 to disable", key='-BLINKCLONETXT-'),
								sg.Slider((0,2),  disable_number_display = True, resolution=0.01, orientation='h', s=(6,15), key='-BLINKCLONE-', enable_events=True),
								sg.Text("Iterations:"), 
								sg.Spin([str(i) for i in range(1,256)], key='-BLINKCLONEITER-', size=3, enable_events=True)			
							],
							[ 
								sg.Frame('',
									[[
										ext_color('No card', '-RCOLOR1-', '-RGRAPH1-', '-RCOLORINPUT1-', '-RSLIDER1-','-RRAINBOW1-','-RFADE1-'),
										ext_color('Card in range', '-RCOLOR2-', '-RGRAPH2-', '-RCOLORINPUT2-', '-RSLIDER2-','-RRAINBOW2-','-RFADE2-')
									]], border_width=0, key='-RCOLORROW1-')
							], 
							[ 
								sg.Frame('',
									[[
										ext_color('Clone mode', '-RCOLOR4-', '-RGRAPH4-', '-RCOLORINPUT4-', '-RSLIDER4-','-RRAINBOW4-','-RFADE4-'),
										ext_color('Blink to', '-RCOLOR3-', '-RGRAPH3-', '-RCOLORINPUT3-', '-RSLIDER3-','-RRAINBOW3-','-RFADE3-')
									]], border_width=0, key='-RCOLORROW2-')
							]
					   ]
			 
layout_reader_icca = [ 
						[
							sg.Checkbox('Send once', key='-ICCASENDONCE-', enable_events=True)
						],
						[
							sg.Checkbox('Blank key to eject', key='-ICCABLANKEJECT-', enable_events=True)
						],
						[
							sg.Slider((0,65.5), resolution=0.5, disable_number_display = True,default_value=15, orientation='h', s=(20,15), key='-ICCAAUTOEJECTTIMER-', enable_events=True),
							sg.Text('Auto eject timer: 0ms', key='-ICCAAUTOEJECTTIMERTXT-')
						],  	
					 ]

layout_touch = [ 
						[
							sg.Checkbox('Multitouch ON by default', tooltip='Hold SELECT while plugging the controller to use the non-default mode.', key='-TOUCHFORCE-', enable_events=True)
						],
						[
							sg.Checkbox('Portrait mode (Project sekai compatibility)', key='-TOUCHPORTRAIT-', enable_events=True)
						],
						[
							sg.Checkbox('Reverse direction', key='-TOUCHREVERSE-', enable_events=True)
						],
						[
							sg.Slider((0,100), disable_number_display = True,default_value=90, orientation='h', s=(20,15), key='-TOUCHHEIGHT-', enable_events=True),
							sg.Text('Y axis: 90%', key='-TOUCHHEIGHTTXT-')
						],  	
					 ]

layout_advanced = [ 
						[
							sg.Text("HID Commands:")
						],
						[
							sg.Multiline("", s=(20,11), expand_x=True, key='-ADVANCEDTXT-', no_scrollbar=True,reroute_stdout=True)
						],
						[
							sg.Button("Send", key='-ADVANCEDSEND-')
						]
				  ]

layout_tabs = [
				[
					sg.TabGroup([
									[
										sg.Tab("Panel", layout_panel_lights, key="-PANELTAB-", visible=isVisibleAny(['NOST','CHUN','DIVA'])),
										sg.Tab("Panel", 
												[ 
													[
														sg.VPush(), 
														sg.Text("No panel detected."), 
														sg.VPush() 
													]
												], element_justification='c', key="-NOPANELTAB-"
											  ),
										sg.Tab("Multitouch", layout_touch, key="-TOUCHTAB-"),
										sg.Tab("AIME Reader", layout_reader_lights, key="-AIMETAB-", visible=not readerDisabled() and isVisible('AIME')),
										sg.Tab("No Reader", 
												[ 
													[
														sg.VPush(), 
														sg.Text(noReaderText(), key='-NOREADERTXT-'), 
														sg.VPush() 
													] 
												], element_justification='c', key="-NOAIMETAB-", visible=readerDisabled()
											  ),
										sg.Tab("ICCA", layout_reader_icca, key="-ICCATAB-", visible=not readerDisabled() and isVisible('ICCA')),
										sg.Tab("Advanced", layout_advanced, key="-ADVANCEDTAB-", visible=isVisible('ADVANCED'))
									]
								]
								) 
				]
			  ]
				 
general_layout = [ 
					[
						sg.Text(sliderName, key='-PRODUCTNAME-', font=(sg.DEFAULT_FONT, 20)), 
						sg.Button('Re-scan', key='-SCANBUTTON-', visible=noDevice())
					],
					[
						sg.Text(", "+readerName, key='-READERNAME-', font=(sg.DEFAULT_FONT, 18))
					],
					[
						sg.Column(layout_options),
						sg.Column(layout_tabs)
					],
					[
						sg.Button('Save to board', key="-COMMITBUTTON-", disabled=noDevice())
					]
				 ]
 
# Create the window
window = sg.Window('RedBoard Configuration Tool', general_layout, icon=ICON, element_justification='c', finalize=True)
	
#utility function to refresh display for all elements which have dynamic visibility and texts
def refresh_dynamic_elements():
		window['-COMMITBUTTON-'].update(disabled=noDevice())
		window['-SCANBUTTON-'].update(visible=noDevice())
		window['-PRODUCTNAME-'].update(sliderName, text_color='red' if noDevice() else 'white')
		window['-READERNAME-'].update(readerName, text_color='red' if noDevice() else 'white')
		window['-READERTYPETEXT-'].update(text_color='darkgrey' if readerDisabled() else 'white')
		window['-READERTYPESPIN-'].update(disabled=readerDisabled())
		window['-READERBPSRADIO1-'].update(disabled=readerDisabled() or window['-READERTYPESPIN-'].Get() != 'AIME')
		window['-READERBPSRADIO2-'].update(disabled=readerDisabled() or window['-READERTYPESPIN-'].Get() != 'AIME')
		window['-SALTBPS-'].update(disabled=readerDisabled())
		window['-AIMETAB-'].update(visible=isVisible('ADVANCED') or (not readerDisabled() and isVisible('AIME')))
		window['-ICCATAB-'].update(visible=isVisible('ADVANCED') or (not readerDisabled() and isVisible('ICCA')))
		window['-NOAIMETAB-'].update(visible=not isVisible('ADVANCED') and (readerDisabled() or not isVisibleAny(['AIME','ICCA','ICCB','ICCC'])))
		window['-PCOLORCHUN-'].update(visible=isVisible('CHUN'))
		window['-TOUCHTHRESHOLDFRAME-'].update(visible=isVisible('ADVANCED') or not isVisible('NOST'))
		window['-DIVABUTTONSON-'].update(visible=isVisible('DIVA'))
		window['-NOREADERTXT-'].update(value=noReaderText())
		window['-NOPANELTAB-'].update(visible=not isVisibleAny(['NOST','DIVA','CHUN']))
		window['-PANELTAB-'].update(visible=isVisibleAny(['NOST','DIVA','CHUN']))
		window['-TOUCHTAB-'].update(visible=isVisibleAny(['NOST','DIVA','CHUN']))
		window['-ADVANCEDTAB-'].update(visible=isVisible('ADVANCED'))
		
window["-PGRAPH1-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-PGRAPH2-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-PGRAPH3-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-PGRAPH4-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-RGRAPH1-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-RGRAPH2-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-RGRAPH3-"].draw_image(data=COLORWHEEL, location=(0,150))
window["-RGRAPH4-"].draw_image(data=COLORWHEEL, location=(0,150))

dict_pcolor1 = {"graph":"-PGRAPH1-","input":"-PCOLORINPUT1-","slider":"-PSLIDER1-","rainbow":"-PRAINBOW1-","fade":"-PFADE1-","curr_cross_coord":(0,0),"last_cross":None}
dict_pcolor2 = {"graph":"-PGRAPH2-","input":"-PCOLORINPUT2-","slider":"-PSLIDER2-","rainbow":"-PRAINBOW2-","fade":"-PFADE2-","curr_cross_coord":(0,0),"last_cross":None}
dict_pcolor3 = {"graph":"-PGRAPH3-","input":"-PCOLORINPUT3-","slider":"-PSLIDER3-","rainbow":"-PRAINBOW3-","fade":"-PFADE3-","curr_cross_coord":(0,0),"last_cross":None}
dict_pcolor4 = {"graph":"-PGRAPH4-","input":"-PCOLORINPUT4-","slider":"-PSLIDER4-","rainbow":"-PRAINBOW4-","fade":"-PFADE4-","curr_cross_coord":(0,0),"last_cross":None}
dict_rcolor1 = {"graph":"-RGRAPH1-","input":"-RCOLORINPUT1-","slider":"-RSLIDER1-","rainbow":"-RRAINBOW1-","fade":"-RFADE1-","curr_cross_coord":(0,0),"last_cross":None}
dict_rcolor2 = {"graph":"-RGRAPH2-","input":"-RCOLORINPUT2-","slider":"-RSLIDER2-","rainbow":"-RRAINBOW2-","fade":"-RFADE2-","curr_cross_coord":(0,0),"last_cross":None}
dict_rcolor3 = {"graph":"-RGRAPH3-","input":"-RCOLORINPUT3-","slider":"-RSLIDER3-","rainbow":"-RRAINBOW3-","fade":"-RFADE3-","curr_cross_coord":(0,0),"last_cross":None}
dict_rcolor4 = {"graph":"-RGRAPH4-","input":"-RCOLORINPUT4-","slider":"-RSLIDER4-","rainbow":"-RRAINBOW4-","fade":"-RFADE4-","curr_cross_coord":(0,0),"last_cross":None}

dict_allcolors = [dict_pcolor1,dict_pcolor2,dict_pcolor3,dict_pcolor4,dict_rcolor1,dict_rcolor2,dict_rcolor3,dict_rcolor4]

#utility function to update global colors array and gui when reading a color value from the board
def set_color_rgbext(coloridx, color):
	graphkey = dict_allcolors[coloridx]["graph"]
	colorkey = dict_allcolors[coloridx]["input"]
	sliderkey = dict_allcolors[coloridx]["slider"]
	rainbowkey = dict_allcolors[coloridx]["rainbow"]
	fadekey = dict_allcolors[coloridx]["fade"]

	window[rainbowkey].update(value=(color[0] > 0))
	window[fadekey].update(value=(color[1] > 0))
	r = color[3]
	g = color[4]
	b = color[2]
	h,s,v = colorsys.rgb_to_hsv(r/255,g/255,b/255)
	x = h*150
	y = s*150
	z = v*255
	dict_allcolors[coloridx]["curr_cross_coord"] = (int(x),int(y))
	window[sliderkey].update(value=z)

	if dict_allcolors[coloridx]["last_cross"]:
		window[graphkey].delete_figure(dict_allcolors[coloridx]["last_cross"])
	dict_allcolors[coloridx]["last_cross"] = window[graphkey].draw_image(data=CROSSHAIR, location=(x-7,y+7))
	rgb_color = hsv2rgb(x/150,y/150,z/255)
	window[colorkey].update(rgb_color, background_color=rgb_color, text_color='white')
	colors[coloridx] = color

#utility function to update global colors array, and gui (graph crosshair, rgb text input)
def set_color(coloridx, reverseMode, graphUpdate):
	graphkey = dict_allcolors[coloridx]["graph"]
	colorkey = dict_allcolors[coloridx]["input"]
	sliderkey = dict_allcolors[coloridx]["slider"]
	rainbowkey = dict_allcolors[coloridx]["rainbow"]
	fadekey = dict_allcolors[coloridx]["fade"]
	if reverseMode: #color picker or textbox edit was used, use new value to update graph crosshair/slider position
		if len(values[colorkey]) != 7:
			return
		r = int(values[colorkey][1:3],16)
		g = int(values[colorkey][3:5],16)
		b = int(values[colorkey][5:7],16)
		h,s,v = colorsys.rgb_to_hsv(r/255,g/255,b/255)
		x = h*150
		y = s*150
		z = v*255
		dict_allcolors[coloridx]["curr_cross_coord"] = (x,y)
		window[sliderkey].update(value=z)
	else:
		if not graphUpdate: #different behavior to handle updating either the slider or only the rainbow/fade tickboxes
			x, y = dict_allcolors[coloridx]["curr_cross_coord"]
		else:
			x, y = values[graphkey]
		z = values[sliderkey]
		
	if x == None:
		r = int(values[colorkey][1:3],16)
		g = int(values[colorkey][3:5],16)
		b = int(values[colorkey][5:7],16)
		colors[coloridx] = color_rgb(values[rainbowkey],values[fadekey],r,g,b)
		return
	if x<0:
		x = 0
	if x>150:
		x = 150
	if y<0:
		y=0
	if y>150:
		y=150
		
	dict_allcolors[coloridx]["curr_cross_coord"] = (x,y)
	if dict_allcolors[coloridx]["last_cross"]:
		window[graphkey].delete_figure(dict_allcolors[coloridx]["last_cross"])
	dict_allcolors[coloridx]["last_cross"] = window[graphkey].draw_image(data=CROSSHAIR, location=(x-7,y+7))

	rgb_color = hsv2rgb(x/150,y/150,z/255)
	red = int(rgb_color[1:3],16)
	green = int(rgb_color[3:5],16)
	blue = int(rgb_color[5:7],16)
	colors[coloridx] = color_rgb(values[rainbowkey],values[fadekey],red,green,blue)
	window[colorkey].update(rgb_color, background_color=rgb_color, text_color='white')

if lights != None:
	read_from_board()
	
while True:
	refresh_dynamic_elements()
	event, values = window.read()
	# print(event)
	if event == sg.TIMEOUT_KEY:
		window.refresh()
	if event == sg.WIN_CLOSED or event == 'Exit':
		break
	if event == 'RainbowSpeed':
		if values['RainbowSpeed'] > 0:
			rainbow_speed = [int(values['RainbowSpeed'])]
	if event == '-COMMITBUTTON-':
		if sg.Window('Save to board', [[ sg.T("") ],[sg.T("This will commit the current settings to the RedBoard persistent memory.")],[sg.T("DO NOT unplug the board while it's flashing, wait for the buttons/slider to become responsive again.")], [ sg.T("") ], [sg.OK(s=10), sg.Cancel(s=10)]], icon=ICON, element_justification='c').read(close=True)[0] == 'OK':
			save_to_flash()
	if event == '-SCANBUTTON-':
		sliderName, readerName, slider, lights = open_device(0x0f0d)
		if lights != None:
			read_from_board()
	if event == '-HOTKEYDURATION-':
		y1, y2, y3, y4 = ( int(values['-HOTKEYDURATION-']*1000) & 0xFFFFFFFF).to_bytes(4, 'little')
		o_input["home_hotkey_delay"] = [y1,y2,y3,y4]
		window["-HOTKEYDURATIONTXT-"].update("Longpress: "+str((values['-HOTKEYDURATION-']))+"s")
	if event == '-TOUCHTHRESHOLD-':
		o_input["touch_threshold"] = [int(values['-TOUCHTHRESHOLD-'])]
		window["-TOUCHTHRESHOLDTXT-"].update("Threshold: "+str(int(values['-TOUCHTHRESHOLD-'])))
	if event == '-DIVABUTTONSON-':
		o_diva['base_lamp_state'] = [1] if values['-DIVABUTTONSON-'] else [0]	
	if event == '-DISABLEREADER-':
		o_hw['no_reader'] = [1] if values['-DISABLEREADER-'] else [0]
	if event == '-AUTODETECT-':
		o_hw['autodetect'] = [1] if values['-AUTODETECT-'] else [0]
	if event.startswith('-READERBPSRADIO'):
		if values['-READERBPSRADIO1-']:
			o_hw['last_aime_slow'] = [1]
		else:
			o_hw['last_aime_slow'] = [0]
	if event == '-READERTYPESPIN-':
		for i in range(6):
			if reader_type_arr[i] == values['-READERTYPESPIN-']:
				o_hw['last_reader'] = [i]
				break
	if event == '-PANELTYPESPIN-':
		for i in range(5):
			if panel_type_arr[i] == values['-PANELTYPESPIN-']:
				o_hw['last_panel'] = [i]
				break
	if event.startswith('-PRADIOHID'):
		if values['-PRADIOHIDIGNORE-']:
			p_hid_mode = [0]
		elif values['-PRADIOHIDSWAP-']:
			p_hid_mode = [1]
		elif values['-PRADIOHIDCOMBINE-']:
			p_hid_mode = [2]	
	if event.startswith('-RRADIOHID'):
		if values['-RRADIOHIDIGNORE-']:
			r_hid_mode = [0]
		elif values['-RRADIOHIDSWAP-']:
			r_hid_mode = [1]
		elif values['-RRADIOHIDCOMBINE-']:
			r_hid_mode = [2]
	if event == '-PRAINBOWDELAY-':
		window["-PRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(values['-PRAINBOWDELAY-']))+"s")
		window["-RRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(values['-PRAINBOWDELAY-']))+"s")
		window["-RRAINBOWDELAY-"].update(values['-PRAINBOWDELAY-'])
		c, f = divmod(int(values['-PRAINBOWDELAY-']*100), 256)
		p_rainbow_delay = [ f, c ] #little endian
		r_rainbow_delay = [ f, c ] #little endian
	if event == '-RRAINBOWDELAY-':
		window["-RRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(values['-RRAINBOWDELAY-']))+"s")
		window["-PRAINBOWDELAYTXT-"].update("Rainbow delay: "+str(int(values['-RRAINBOWDELAY-']))+"s")
		window["-PRAINBOWDELAY-"].update(values['-RRAINBOWDELAY-'])
		c, f = divmod(int(values['-RRAINBOWDELAY-']*100), 256)
		r_rainbow_delay = [ f, c ] #little endian
		p_rainbow_delay = [ f, c ] #little endian
	if event == '-PRAINBOWSPEED-':
		window["-PRAINBOWSPEEDTXT-"].update("Rainbow speed: "+str(int(values['-PRAINBOWSPEED-'])))
		p_rainbow_speed = [ int('{:04X}'.format(int(values['-PRAINBOWSPEED-']) & ((1 << 8)-1)), base=16) ]
	if event == '-RRAINBOWSPEED-':
		window["-RRAINBOWSPEEDTXT-"].update("Rainbow speed: "+str(int(values['-RRAINBOWSPEED-'])))
		r_rainbow_speed = [ int('{:04X}'.format(int(values['-RRAINBOWSPEED-']) & ((1 << 8)-1)), base=16) ]
	if event == '-PFADEDURATION-':
		window["-PFADEDURATIONTXT-"].update("Fade duration: "+str((values['-PFADEDURATION-']))+"s")
		p_fade_speed = [int(values['-PFADEDURATION-']*10)]
	if event == '-RFADEDURATION-':
		window["-RFADEDURATIONTXT-"].update("Fade duration: "+str((values['-RFADEDURATION-']))+"s")
		r_fade_speed = [int(values['-RFADEDURATION-']*10)]
	if event == '-BLINKCLONE-':
		window["-BLINKCLONETXT-"].update("Blink on clone: "+str(float(values['-BLINKCLONE-']))+"s")
		c, f = divmod(int(values['-BLINKCLONE-']*1000), 256)
		o_aime_clone['blink_duration'] = [ f, c ] #little endian
	if event == '-BLINKSCAN-':
		window["-BLINKSCANTXT-"].update("Blink on scan: "+str(float(values['-BLINKSCAN-']))+"s")
		c, f = divmod(int(values['-BLINKSCAN-']*1000), 256)
		o_aime_scan['blink_duration'] = [ f, c ] #little endian
	if event == '-BLINKSCANITER-':
		o_aime_scan["iterations"] = [ int(values['-BLINKSCANITER-']) ]
	if event == '-BLINKCLONEITER-':
		o_aime_clone["iterations"] = [ int(values['-BLINKCLONEITER-']) ]
	if event == '-PGRAPH1-' or event == '-PSLIDER1-' or event == '-PRAINBOW1-' or event == '-PFADE1-' or event == '-PCOLORINPUT1-':	
		set_color(0, event == '-PCOLORINPUT1-', event == '-PGRAPH1-')
	if event == '-PGRAPH2-' or event == '-PSLIDER2-' or event == '-PRAINBOW2-' or event == '-PFADE2-' or event == '-PCOLORINPUT2-':
		set_color(1, event == '-PCOLORINPUT2-', event == '-PGRAPH2-')
	if event == '-PGRAPH3-' or event == '-PSLIDER3-' or event == '-PRAINBOW3-' or event == '-PFADE3-' or event == '-PCOLORINPUT3-':
		set_color(2, event == '-PCOLORINPUT3-', event == '-PGRAPH3-')
	if event == '-PGRAPH4-' or event == '-PSLIDER4-' or event == '-PRAINBOW4-' or event == '-PFADE4-' or event == '-PCOLORINPUT4-':
		set_color(3, event == '-PCOLORINPUT4-', event == '-PGRAPH4-') 
	if event == '-RGRAPH1-' or event == '-RSLIDER1-' or event == '-RRAINBOW1-' or event == '-RFADE1-' or event == '-RCOLORINPUT1-':
		set_color(4, event == '-RCOLORINPUT1-', event == '-RGRAPH1-')
	if event == '-RGRAPH2-' or event == '-RSLIDER2-' or event == '-RRAINBOW2-' or event == '-RFADE2-' or event == '-RCOLORINPUT2-':
		set_color(5, event == '-RCOLORINPUT2-', event == '-RGRAPH2-')
	if event == '-RGRAPH3-' or event == '-RSLIDER3-' or event == '-RRAINBOW3-' or event == '-RFADE3-' or event == '-RCOLORINPUT3-':
		set_color(6, event == '-RCOLORINPUT3-', event == '-RGRAPH3-')
	if event == '-RGRAPH4-' or event == '-RSLIDER4-' or event == '-RRAINBOW4-' or event == '-RFADE4-' or event == '-RCOLORINPUT4-':
		set_color(7, event == '-RCOLORINPUT4-', event == '-RGRAPH4-') 
	if event == '-SALTBPS-':
		o_serial["aime_alt_baudrate"] = [1] if values['-SALTBPS-'] else [0]
	if event == '-STIMEOUT-':
		y1, y2, y3, y4 = ( int(values['-STIMEOUT-']*1000) & 0xFFFFFFFF).to_bytes(4, 'little')
		o_serial["open_timeout"] = [y1,y2,y3,y4]
		window["-STIMEOUTTXT-"].update("Open timeout: "+str((values['-STIMEOUT-']))+"s")
	if event == '-SNBRETRIES-':
		o_serial["nb_retries"] = [ int(values['-SNBRETRIES-']) ]
	if event == '-CARDUID-':
		if len(values['-CARDUID-']) >= 16:
			for i in range(0,8):
				try:
					o_virtual_card['uid'][i] = int(values['-CARDUID-'][2*i:2*i+2],16)
				except Exception as err:
					sg.Popup("Invalid UID! Expected 16 hex digits (0-9A-F)", title="Card UID error", button_type=3)
	if event == '-FELICA-':
		o_virtual_card['type'] = [2] if values['-FELICA-'] else [1]
	if event == '-ALWAYSFELICA-':
		o_cardio['always_felica'] = [1] if values['-ALWAYSFELICA-'] else [0]
	if event == '-CLONETIMEOUT-':
		window["-CLONETIMEOUTTXT-"].update("Clone mode timeout: "+str((int(values['-CLONETIMEOUT-'])))+"s")
		o_virtual_card['clone_timeout'] = [ int(values['-CLONETIMEOUT-']) ]
	if event == '-ICCAAUTOEJECTTIMER-':
		y1, y2, y3, y4 = ( int(values['-ICCAAUTOEJECTTIMER-']*1000) & 0xFFFFFFFF).to_bytes(4, 'little')
		o_icca['auto_eject_timer'] = [y1,y2,y3,y4]
		window["-ICCAAUTOEJECTTIMERTXT-"].update("Auto eject timer: "+str((values['-ICCAAUTOEJECTTIMER-']))+"s")		
	if event == '-ICCASENDONCE-':
		o_icca["send_once"] = [1] if values['-ICCASENDONCE-'] else [0]
	if event == '-ICCABLANKEJECT-':
		o_icca["keypad_blank_eject"] = [1] if values['-ICCABLANKEJECT-'] else [0]
	if event == '-TOUCHFORCE-':
		o_touch["force_touch"] = [1] if values['-TOUCHFORCE-'] else [0]
	if event == '-TOUCHPORTRAIT-':
		o_touch["touch_portrait"] = [1] if values['-TOUCHPORTRAIT-'] else [0]
	if event == '-TOUCHREVERSE-':
		o_touch["touch_reverse"] = [1] if values['-TOUCHREVERSE-'] else [0]
	if event == '-TOUCHHEIGHT-':
		o_touch['touch_height'] = [int(values['-TOUCHHEIGHT-'])] if values['-TOUCHHEIGHT-'] <= 100 else [100] 
		window["-TOUCHHEIGHTTXT-"].update("Y axis: "+str((int(values['-TOUCHHEIGHT-'])))+"%")			
	if event == '-HIDCARDIOCOOLDOWN-':
		o_hid["cardio_cooldown"] = [int(values['-HIDCARDIOCOOLDOWN-'])]
		window["-HIDCARDIOCOOLDOWNTXT-"].update("Cardio cooldown: "+str((values['-HIDCARDIOCOOLDOWN-']))+"s")
	if event == '-HIDREACTIVEFALLBACK-':
		y1, y2, y3, y4 = ( int(values['-HIDREACTIVEFALLBACK-']*1000) & 0xFFFFFFFF).to_bytes(4, 'little')
		o_hid["reactive_timeout"] = [y1,y2,y3,y4]
		window["-HIDREACTIVEFALLBACKTXT-"].update("Reactive fallback: "+str((values['-HIDREACTIVEFALLBACK-']))+"s")
		
	if event == '-SHOWALL-':
		showall = values['-SHOWALL-']
	if event == '-ADVANCEDSEND-':
		lines = values['-ADVANCEDTXT-'].splitlines()
		if lights != None:
			for line in lines:
				try:
					list = [int(x, 16) for x in line.strip('[]').replace('"', '').replace(' ', '').split(',')]
					data = bytes(bytearray(list))
				except Exception as err:
					sg.Popup("Invalid entry!", title="HID report error", button_type=3)
				lights.send_feature_report(data)
				read_from_board()
	try:
		send_to_board()
	except hid.HIDException as err:
		sliderName, readerName, slider, lights = ('     Device not found','Reader not found',None,None)

window.close()						 
