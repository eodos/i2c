#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> // O_CREAT
#include <semaphore.h>
#include "functions.h"

#define ARDUINO 0x04
#define TI 0x08

#define TAG_REFRESH_RATE "REFRESH_RATE"
#define TAG_FORECAST "FORECAST"
#define TAG_TIMESTAMP "TIMESTAMP"
#define TAG_TEMP "TEMP"
#define TAG_RAIN "RAIN"
#define TAG_HUM "HUMIDITY"
#define TAG_ACTUATOR "ACTUATOR"

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
	uint actuator;

	char temperature_char[] = {0};
	char rain_char[] = {0};
	char humidity_char[] = {0};
	char forecast_char[] = {0};
	char actuator_char[] = {0};

	int pid, status;

	sem_t * sem;

	char sem_name[] = "miSemaforo";
	char shm_path[] = "/dev/shm/sem.";
	char sem_path[20];
	strcpy(sem_path, shm_path);
	strcat(sem_path, sem_name);

	sem = sem_open(sem_name, O_CREAT, 0666, 0);

	chmod(sem_path, 0666);

	static struct timespec time_to_wait = {0, 0};

	// Creamos el proceso hijo y le indicamos que ejecute el script para obtener la predicción meteorológica
	pid = fork();

	if (pid == 0) {
		execlp("python", "python", "weather_forecast.py", NULL);
		exit(0);
	}

	// Proceso padre
	else {

		for(;;) {

			// Leemos la tasa de refresco
			buf = readVar(TAG_REFRESH_RATE);
			refresh_rate = atoi((char *) &buf);
			printf("%s: %d\n", TAG_REFRESH_RATE, refresh_rate);

			// Ajustamos el valor del semáforo con nombre
			time_to_wait.tv_sec = time(NULL) + refresh_rate;
		
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

			// Algoritmo para el encendido del sistema de riego
			actuator = 0;

			// Convertimos los valores a char*
			sprintf(temperature_char, "%.2f", temperature);
			sprintf(rain_char, "%d", rain);
			sprintf(humidity_char, "%d", humidity);
			sprintf(forecast_char, "%d", forecast);
			sprintf(actuator_char, "%d", actuator);

			// Creamos los char** que cotienen variables y vectores
			char * variables_array[] = {TAG_TIMESTAMP, TAG_TEMP, TAG_RAIN, TAG_HUM, TAG_FORECAST, TAG_ACTUATOR, NULL};
			char * values_array[] = {timestamp, temperature_char, rain_char, humidity_char, forecast_char, actuator_char, NULL};

			printf("%s: %s\n", TAG_TEMP, temperature_char);
			printf("%s: %s\n", TAG_RAIN, rain_char);
			printf("%s: %s\n", TAG_HUM, humidity_char);
			printf("%s: %s\n", TAG_FORECAST, forecast_char);
			printf("%s: %s\n", TAG_ACTUATOR, actuator_char);
			printf("%s\n", timestamp);

			// Guardamos en el log los valores
			writeVar(variables_array, values_array);

			sem_timedwait(sem, &time_to_wait);
		}

		sem_close(sem);
		sem_unlink(sem_name);
	}

	return 0;
}