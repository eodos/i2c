#!/usr/bin/python2

# Este programa realiza una peticipn a la API de Yahoo Weather para obtener el pronostico del tiempo de 4 dias y analiza si alguno de ellos
# indica lluvia. En caso afirmativo almacena el numero del dia en el fichero. En caso negativo almacena -1. Si no puede conectar al servidor 
# escribe NULL. Obtiene la localizacion de la cual buscar el pronostico del fichero vars/LOCATION. Debe estar en formato "Ciudad, Pais". 
# Guarda la salida en vars/FORECAST.

import urllib2, urllib, json
from time import sleep

while True:

	forecast = [0,0,0,0]
	next_rain = -1

	# Leemos el tiempo de espera entre dos peticiones para obtener el pronostico, en minutos
	f = open("vars/FORECAST_RATE", 'r')
	rate = f.read()
	f.close()

	# Leemos la localizacion de la cual obtener el pronostico
	f = open("vars/LOCATION", 'r')
	location = f.read()
	f.close()

	# Creamos la URL que contiene la peticion
	baseurl = "https://query.yahooapis.com/v1/public/yql?"
	yql_query = "select item.forecast.code from weather.forecast where woeid in (select woeid from geo.places where text='" + location + "') and u='c'"
	yql_url = baseurl + urllib.urlencode({'q':yql_query}) + "&format=json"

	# Realizamos la peticion a la API de Yahoo Weather
	try:
		result = urllib2.urlopen(yql_url).read()
		data = json.loads(result)
		for i in range(0, 4):

			# Obtenemos el codigo que indica el pronostico meteorologico de cada dia
			forecast[i] = data['query']['results']['channel'][i]['item']['forecast']['code']

			# Analizamos si ese codigo indica lluvia. Si es asi anotamos el dia en el que va a llover y pasamos a la escritura de la variable
			if not (int(forecast[i]) >= 19 and int(forecast[i]) <= 34) or (int(forecast[i]) == 36) or (int(forecast[i]) == 44):
				next_rain = i
				break

	# Si no podemos conectar a la API, escribimos NULL en el fichero salida
	except:
		next_rain = "NULL"

	f = open("vars/FORECAST", 'w')
	f.write(str(next_rain))
	f.close()

	sleep(int(rate)*60)