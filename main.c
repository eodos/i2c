#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> // O_CREAT
#include <time.h>
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
#define TAG_TIME_ACTUATOR "TIME_ACTUATOR"

#define ACTUATOR_SIGNAL 1

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
	int rain;
	int humidity;
	int forecast;
	int actuator = 0;

	char temperature_char[10];
	char rain_char[10];
	char humidity_char[10];
	char forecast_char[10];
	char actuator_char[10];
	char time_char[10];

	time_t t_comienzo = time (NULL);
	time_t t_actual = time(NULL);
	long t_actuator = 0;	
	uint cambio_actuator = 0;
	int actuator_success;

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

			// Comprobamos que no haya fallos en los sensores
			if (temperature < 0 || temperature > 50) temperature = -1;
			if (rain != 0 && rain != 1) rain = -1;
			if (humidity < 0 || humidity > 100) humidity = -1;

			// Algoritmo para el encendido del sistema de riego
			cambio_actuator = processOutput(temperature, rain, humidity, forecast, actuator, t_actuator);
			if (cambio_actuator == 1) {
				if (actuator == 1) actuator = 0;
				else actuator = 1;
				t_comienzo = time(NULL);
			}

			t_actual = time(NULL);
			t_actuator = difftime(t_actual, t_comienzo);

			// Envio de la orden al actuador
			if (cambio_actuator == 1) {
				do {
					printf("Enviando señal al actuador\n");
					actuator_success = setActuator(ARDUINO, ACTUATOR_SIGNAL);
					sleep(5);
				} while(actuator_success != 1);
			}

			// Convertimos los valores a char*
			sprintf(temperature_char, "%.2f", temperature);
			sprintf(rain_char, "%d", rain);
			sprintf(humidity_char, "%d", humidity);
			sprintf(forecast_char, "%d", forecast);
			sprintf(actuator_char, "%d", actuator);
			sprintf(time_char, "%d", t_actuator);

			// Creamos los char** que cotienen variables y vectores
			char * variables_array[] = {TAG_TIMESTAMP, TAG_TEMP, TAG_RAIN, TAG_HUM, TAG_FORECAST, TAG_ACTUATOR, TAG_TIME_ACTUATOR, NULL};
			char * values_array[] = {timestamp, temperature_char, rain_char, humidity_char, forecast_char, actuator_char, time_char, NULL};

			printf("%s: %s\n", TAG_TEMP, temperature_char);
			printf("%s: %s\n", TAG_RAIN, rain_char);
			printf("%s: %s\n", TAG_HUM, humidity_char);
			printf("%s: %s\n", TAG_FORECAST, forecast_char);
			printf("%s: %s\n", TAG_ACTUATOR, actuator_char);
			printf("%s: %s\n", TAG_TIME_ACTUATOR, time_char);
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