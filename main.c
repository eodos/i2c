#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"

#define ARDUINO 0x04
#define TI 0x08

#define TAG_REFRESH_RATE "REFRESH_RATE"
#define TAG_FORECAST "FORECAST"
#define TAG_TIMESTAMP "TIMESTAMP"
#define TAG_TEMP "TEMP"
#define TAG_RAIN "RAIN"
#define TAG_HUM "HUMIDITY"

int main() {
	char * buf;

	int refresh_rate;

	char * timestamp;

	uint nBytesTAG_TEMP = sizeof(TAG_TEMP)-1; // no enviamos el caracter '\0'
	uint nBytesTAG_RAIN = sizeof(TAG_RAIN)-1;
	uint nBytesTAG_HUM = sizeof(TAG_HUM)-1;
	uint nBytesTEMP = 4;
	uint nBytesRAIN = 1;
	uint nBytesHUM = 3;

	float temperature;
	uint rain;
	uint humidity;
	int forecast;

	char temperature_char[] = {0};
	char rain_char[] = {0};
	char humidity_char[] = {0};
	char forecast_char[] = {0};

	int pid, status;

	buf = readVar(TAG_REFRESH_RATE);
	refresh_rate = atoi((char *) &buf);
	printf("%s: %d\n", TAG_REFRESH_RATE, refresh_rate);

	// Creamos el proceso hijo y le indicamos que ejecute el script para obtener la predicción meteorológica
	pid = fork();

	if (pid == 0) {
		execlp("python", "python", "weather_forecast.py", NULL);
		exit(0);
	}

	// Proceso padre
	else {

		for(;;) {
		
			// Leemos los valores se los sensores
			temperature = readSensor(TI, TAG_TEMP, nBytesTAG_TEMP, nBytesTEMP) / 100.00;
			usleep(50000);
			rain = readSensor(TI, TAG_RAIN, nBytesTAG_RAIN, nBytesRAIN);
			usleep(50000);
			humidity = readSensor(TI, TAG_HUM, nBytesTAG_HUM, nBytesHUM);

			// Obtenemos el timestamp
			timestamp = getTime();

			// Leemos el pronóstico del fichero
			buf = readVar(TAG_FORECAST);
			forecast = atoi((char *) &buf);

			// Convertimos los valores a char*
			sprintf(temperature_char, "%.2f", temperature);
			sprintf(rain_char, "%d", rain);
			sprintf(humidity_char, "%d", humidity);
			sprintf(forecast_char, "%d", forecast);

			// Creamos los char** que cotienen variables y vectores
			char * variables_array[] = {TAG_TIMESTAMP, TAG_TEMP, TAG_RAIN, TAG_HUM, TAG_FORECAST, NULL};
			char * values_array[] = {timestamp, temperature_char, rain_char, humidity_char, forecast_char, NULL};

			printf("%s: %s\n", TAG_TEMP, temperature_char);
			printf("%s: %s\n", TAG_RAIN, rain_char);
			printf("%s: %s\n", TAG_HUM, humidity_char);
			printf("%s: %s\n", TAG_FORECAST, forecast_char);
			printf("%s\n", timestamp);

			// Guardamos en el log los valores
			writeVar(variables_array, values_array);

			usleep(refresh_rate * 1000000);
		}
	}

	return 0;
}