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
	"""For help generating the 'variables' dictionary"""
	for i in messages:
		if i[1]:
			print "'" + i[0] + "' : [],"

	import sys
	sys.exit(0)

def mapMe():
	"""For help generating the map of int->constructor"""
	for m in messages:
		print 'typeToCon[%s] = (Header *(*)(char *)) %s;' % (str(m[2]), m[0]+"Fac")

# mapMe()
# import sys
# sys.exit(0)

bits_to_name = {
	8:  'uint8_t ',
	16: 'uint16_t',
	"16+":'short   ',
	32: 'uint32_t',
	64: 'uint64_t',
	"string": 'char  '
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
	'SetLabel' : [('string', 'payload[32]')],
	'StateLabel' : [('string', 'payload[32]')],
	'StateVersion' : [(32, 'vendor'), (32, 'product'), (32, 'version')],
	'StateInfo' : [(64, 'time'), (64, 'uptime') , (64, 'downtime')],
	'StateLocation' : [('string', 'location[16]'), ('string', 'label[32]'), (64, 'updated_at')],
	'StateGroup' : [('string', 'location[16]'), ('string', 'label[32]'), (64, 'updated_at')], 
	'EchoRequest' : [('string', 'payload[64]')],
	'EchoResponse' : [('string', 'payload[64]')],
	'SetColor' : [(8, 'reserved'), (16, 'hue'),(16, 'saturation'),(16, 'brightness'),(16, 'kelvin'), (32, 'duration')],
	'State' : [(16, 'hue'),(16, 'saturation'),(16, 'brightness'),(16, 'kelvin'), ("16+", 'reserved'), (16, 'power'), ('string', 'label[32]'), (64, 'reserved')],
	'SetPower_Light' : [(16, 'level'), (32, 'duration')],
	'StatePower_Light' : [(16, 'level')]
}

def convertType(thing):
	if thing in bits_to_name:
		return bits_to_name[thing]
	return thing
	
def toBits(thing):
	var_name = thing[1]
	thing = thing[0]
	if type(thing) is int:
		return thing
	elif type(thing) is str:
		thing = thing.strip()
		if thing == "float":
			return 32
		elif thing == "string":
			start = var_name.find('[') + 1
			end = var_name.find(']')
			return 8*int(var_name[start:end])
		# convert greedily to number
		i = 0
		while i < len(thing):
			if not thing[i].isdigit():
				break;
			i += 1
		if i != 0:
			return int(thing[0:i])
		
	raise ValueError("Unknown number of bits: " + str(thing))

def makeParams(className, insertUnderscoreNames=False):
	"""To be used in header definition (False) and class implementation (True)"""
	var_list = variables[className]
	if len(var_list) == 0:
		return ""
	
	def operate(a_var):
		"""Returns the type name (and optionally the variable name too) for the passed in tuple"""
		temp = False
		if a_var[1] != "reserved":
			type_name = convertType(a_var[0]).strip()
			if a_var[0] == "string":  # the only time we don't want this is in the payload struct
				type_name = "string"
			temp = type_name
			if insertUnderscoreNames:
				if a_var[0] == "string":
					end = a_var[1].find('[')
					temp += " _" + a_var[1][0:end]
				else:
					temp += " _" + a_var[1]
		return temp

	# get first word
	i = 0
	while i < len(var_list):
		ret = operate(var_list[i])
		i += 1
		if ret == False:
			pass
		else:
			break
	# get the rest of the params, separated by commas
	while i < len(var_list):
		temp = operate(var_list[i])
		i += 1
		if temp != False:
			ret += ", " + temp
	return ret

# print makeParams("SetColor", False)
# print "~~~~~~"
# print makeParams("SetColor", True)
# import sys
# sys.exit(0)

def appendWarning(file_var):
	file_var.write("// Stop!  Do not modify this file as it was auto-generated by " + __file__ + "\n\n")

def addThisToThat(srcPath, file_var):
	with open(srcPath, 'r') as src:
		for i in src:
			file_var.write(i)

with open("Messages.h", "w") as headfile:
	with open("Messages.cpp", "w") as bodyfile:
		
		appendWarning(headfile)
		appendWarning(bodyfile)

		addThisToThat("static_files/head_start.h", headfile)
		addThisToThat("static_files/body_start.cpp", bodyfile)

		for message_spec in messages:
			# first do header (.h) file
			className = message_spec[0]
			headfile.write("struct %s: public Header {\n" % className)
			if message_spec[1]:  # has payload
				headfile.write( "\tstruct {\n" )
				for a_var in variables[className]:
					var_name = a_var[1]
					# if not used, then make the variable look like "uint8_t :8;"
					if var_name == "reserved":  
						var_name = ":" + str(toBits(a_var))
					headfile.write(	"\t\t" + convertType(a_var[0]) + " " + var_name + ";\n")  # TODO optional :len
				headfile.write( "\t} payload;\n" )

				headfile.write( "public:\n")
				headfile.write( "\t%s(%s);\n" % (className, makeParams(className) ) )
				headfile.write( "\t%s(char *);\n" % className )

				headfile.write( "\tMessageBuffer * makeBuffer();\n")
				headfile.write( "\tvoid printEverything();\n" )
			else:
				headfile.write( "public:\n")
				headfile.write( "\t%s();\n" % (className) )
				headfile.write( "\t%s(char *);\n" % className )
			# 

			headfile.write( "};\n\n" )
			headfile.write( "%s * %sFac(char *);\n\n\n" % (className, className))


			# now do code (.cpp) file

			if message_spec[1]:  # has payload
				# parameter constructor
				bodyfile.write("%s::%s(%s) {\n" % (className, className, makeParams(className, True)) )
				bodyfile.write("\thead.type = %d;\n" % message_spec[2] )
				bodyfile.write("\thead.size = sizeof(head) + sizeof(payload);\n")
				bodyfile.write("\tmemset(&payload, 0, sizeof(payload));\n")
				var_list = variables[className]
				for a_var in var_list:
					if a_var[1] != "reserved":
						if a_var[0] == "string":
							end = a_var[1].find('[')
							end2 = a_var[1].find(']')
							var_name = a_var[1][0:end]
							param_name = "_" + var_name
							# size = int(a_var[1][end + 1:end2])
							bodyfile.write("\tmemcpy(&payload.%s, %s.c_str(), %s.size());\n" % (var_name, param_name, param_name) )
						else:
							bodyfile.write("\tpayload.%s = _%s;\n" % (a_var[1], a_var[1]) )
				bodyfile.write("}\n\n")
				# deserialize
				bodyfile.write("%s::%s(char * buf) : Header(buf) {\n" % (className, className) )
				bodyfile.write("\tmemcpy(&payload, buf + sizeof(head), sizeof(payload));\n")
				bodyfile.write("}\n\n")
				# sending code
				bodyfile.write("MessageBuffer * %s::makeBuffer() {\n" % className)
				bodyfile.write("\tchar buf[ sizeof(head) + sizeof(payload)];\n")
				bodyfile.write("\tint size = sizeof(head) + sizeof(payload);\n")
				bodyfile.write("\tmemcpy(buf, &head, sizeof(head));\n")
				bodyfile.write("\tmemcpy(buf + sizeof(head), &payload, sizeof(payload));\n")
				bodyfile.write("\tMessageBuffer * mb = new MessageBuffer(buf, size);\n")
				bodyfile.write("\treturn mb;\n")
				bodyfile.write("}\n\n")
				# debug code
				bodyfile.write('void %s::printEverything() {\n' % className)
				bodyfile.write('\tHeader::printEverything();\n')
				bodyfile.write('\tcout << "~~~~~~ Payload ~~~~~~" << endl;\n')
				for a_var in var_list:
					if a_var[1] != "reserved":
						bodyfile.write("\tcout << fcol << \"%s\" << tab << payload.%s << endl;\n" % (a_var[1], a_var[1]) )
					else:
						bodyfile.write("\tcout << fcol << \"%s\" << tab << \"%s bits\" << endl;\n" % (a_var[1], str(a_var[0])) )
				bodyfile.write("}\n\n")

			else:  # no payload
				# default constructor
				bodyfile.write("%s::%s() {\n" % (className, className) )
				bodyfile.write("\thead.type = %d;\n" % message_spec[2] )
				bodyfile.write("}\n\n")
				# deserialize
				bodyfile.write("%s::%s(char * buf) : Header(buf) {\n" % (className, className) )
				bodyfile.write("}\n\n")
			# factory function
			bodyfile.write( "%s * %sFac(char * buf){\n" % (className, className))
			bodyfile.write( "\treturn new %s(buf);\n" % className)
			bodyfile.write( "}\n\n\n")
		addThisToThat("static_files/head_end.h", headfile)
