#!/usr/bin/python2

import cgi
import cgitb
import json

cgitb.enable()

print "Content-Type: application/json\n\n"

data = cgi.FieldStorage()

dict = {}

for var in data.getlist("var"):
	f = open("vars/"+var,'r')
	dict[var]=f.read().rstrip('\n')
	f.close()

print json.dumps(dict)