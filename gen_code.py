"""This file generates c++ code for each light/device message.
// same thing, different type number
"""


li = [
#  (className, hasPayload, protocol number)
	# ("GetService", False, 2),
	# ("StateService", True, 3),
	# ("GetHostInfo" , False, 2),
	# ("StateHostInfo" , True, 3),
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

for i in li:
	className = i[0]
	if not i[1]:
		first = "class %s: public Header {\npublic:\n\t%s();\n};" % (className,className)
	else:
		first = "class %s: public Header {\n\tstruct {\n\t\t\n\t} payload;\npublic:\n\t%s();\n\t%s(char *);\n\tvoid printEverything();\n};" % (className,className,className)

	first += "\n\n"
	print first