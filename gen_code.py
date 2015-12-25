"""This file generates c++ code for each light/device message.
// same thing, different type number
"""


messages = [
#  (className, hasPayload, protocol number)
	("GetService", False, 2),
	("StateService", True, 3),
	("GetHostInfo" , False, 2),
	("StateHostInfo" , True, 3),
	("GetHostFirmware" , False, 4),
	("StateHostFirmware" , True, 5),
	("GetWifiInfo", False, 16),
	("StateWifiInfo", True, 17),
	("GetWifiFirmware", False, 18),
	("StateWifiFirmware", True, 19),
	("GetPower", False, 20),
	("SetPower", True, 21),
	("StatePower", True, 22),
	("GetLabel", False, 23),
	("SetLabel", True, 24),
	("StateLabel", True, 25),
	("GetVersion", False, 32),
	("StateVersion", True, 33),
	("GetInfo", False, 34),
	("StateInfo", True, 35),
	("Acknowledgement", False, 45),  # check if it has payload, I don't think it does
	("GetLocation", False, 48),
	("StateLocation", True, 50),
	("GetGroup", False, 51),
	("StateGroup", True, 53),
	("EchoRequest", True, 58),
	("EchoResponse", True, 59),


	("Get", False, 101),
	("SetColor", True, 102),
	("State", True, 107),
	("GetPower_Light", False, 116),  # same as above  # TODO check that these have the same content too
	("SetPower_Light", True, 117),  # different from above
	("StatePower_Light", True, 118)  # same as StatePower_Device

]

def listPayloadMessages():
	for i in messages:
		if i[1]:
			print "'" + i[0] + "' : [],"

	import sys
	sys.exit(0)


bits_to_name = {
	8:  'uint8_t ',
	16: 'uint16_t',
	"16+":'short   ',
	32: 'uint32_t',
	64: 'uint64_t'
}

(16, 'hue'),(16, 'saturation'),(16, 'brightness'),(16, 'kelvin')

variables = {
	'StateService' : [(8, "service"), (32, "port")],
	'StateHostInfo' : [('float   ', 'signal') , (32, 'tx') , (32, 'rx'), ('16+', 'reserved')],
	'StateHostFirmware' : [(64, 'build'), (64, 'reserved'), (32, 'version')],
	'StateWifiInfo' : [('float   ', 'signal'), (32, 'tx') , (32, 'rx'), ('16+', 'reserved')],
	'StateWifiFirmware' : [(64, 'build'), (64, 'reserved'), (32, 'version')],
	'SetPower' : [(16, 'level')],
	'StatePower' : [(16, 'level')],
	'SetLabel' : [],  # TODO this and next are strings, 32 bytes
	'StateLabel' : [],
	'StateVersion' : [(32, 'vendor'), (32, 'product'), (32, 'version')],
	'StateInfo' : [(64, 'time'), (64, 'uptime') , (64, 'downtime')],
	'StateLocation' : [],  # TODO
	'StateGroup' : [], 
	'EchoRequest' : [('char', 'payload[64]')],
	'EchoResponse' : [('char', 'payload[64]')],
	'SetColor' : [(8, 'reserved'), (16, 'hue'),(16, 'saturation'),(16, 'brightness'),(16, 'kelvin'), (32, 'duration')],
	'State' : [(16, 'hue'),(16, 'saturation'),(16, 'brightness'),(16, 'kelvin'), ("16+", 'reserved'), (16, 'power'), ('char', 'label[32]'), (64, 'reserved')],
	'SetPower_Light' : [(16, 'level'), (32, 'duration')],
	'StatePower_Light' : [(16, 'level')]
}

def convertType(thing):
	if thing in bits_to_name:
		return bits_to_name[thing]
	return thing

with open("gheaders.txt", "w") as headfile:
	with open("gbodies.txt", "w") as bodyfile:
		for i in messages:
			# first do header file
			className = i[0]
			headfile.write("class %s: public Header {\n" % className)
			if i[1]:  # has payload
				headfile.write( "\tstruct {\n" )
				for extra in variables[className]:
					headfile.write(	"\t\t" + convertType(extra[0]) + " " + extra[1] + ";\n")  # TODO optional :len
				headfile.write( "\t} payload;\n" )

				headfile.write( "public:\n")
				headfile.write( "\t%s();\n" % className )  # default constructor
				headfile.write( "\t%s(char *);\n" % className )
				headfile.write( "\tvoid printEverything();\n};" )
			else:
				headfile.write( "public:\n\t%s();\n};" % className )

			headfile.write( "\n\n\n" )
			

			# then do code file
			pass