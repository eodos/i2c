#!/usr/bin/python2

import cgi
import cgitb
import json

cgitb.enable()

print "Content-Type: application/json\n\n"

data = cgi.FieldStorage()

for var in data:
	f = open("vars/"+var,'w')
	f.write(data[var].value)
	f.close()

print "Success"
