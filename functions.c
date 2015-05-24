#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> // open modo O_RDWR
#include <linux/i2c-dev.h>
#include "functions.h"

char * ruta(char * variable) {
	char * temp;
	char * directorio = "vars/";
	if((temp = malloc(strlen(directorio)+strlen(variable)+1)) != NULL) {
		temp[0]='\0';
		strcat(temp, directorio);
		strcat(temp, variable);
		return temp;
	}
}

char * readVar(char * variable) {
	FILE * f;
	char * nombre_fichero;
	//char * value = malloc(99 * sizeof(char));
	char * value;

	nombre_fichero = ruta(variable);
	f = fopen((char *)nombre_fichero, "r");

	if (f != NULL) {
		fgets((char *)&value, 100, f);
		fclose(f);
	}
	return value;
}

int writeVar(char * variables_array[], char * values_array[]) {
	FILE * f;
	char * nombre_fichero;
	int i;

	nombre_fichero = ruta("VARS");

	f = fopen(nombre_fichero, "a+");

	if (f != NULL) {

		for (i=0; variables_array[i] != NULL; i++);
		int n_elements = i;

		// Vemos si hay que imprimir el header
		fseek(f, 0, SEEK_END);
		if (ftell(f) == 0) {
			for (i=0; i<n_elements; i++) {
				if(i != n_elements-1)
					fprintf(f, "%s,", variables_array[i]);
				else
					fprintf(f, "%s\n", variables_array[i]);
			}
		}

		// Escribimos las variables
		for (i=0; i < n_elements; i++) {
			if (i != n_elements-1)
				fprintf(f, "%s,", values_array[i]);
			else
				fprintf(f, "%s\n", values_array[i]);
		}

		fclose(f);
		return 0;
	}
	else
		return -1;
}

char * getTime() {
	time_t current_time;
	char * c_time_string;

	/* Obtain current time as seconds elapsed since the Epoch. */
	current_time = time(NULL);

	/* Convert to local time format. */
    c_time_string = (char *) ctime(&current_time);
    c_time_string[strlen(c_time_string)-1] = '\0';

    return c_time_string;
}

int readSensor(int device, char * sensor, uint nBytesSend, uint nBytesReceive) {
	int fd;
	char buf[4];
	float value = -100;
	
	if ((fd = open("/dev/i2c-0", O_RDWR)) >= 0) {
	
		if (ioctl(fd, I2C_SLAVE, device) >= 0) {
	
			if (write(fd, sensor, nBytesSend) == nBytesSend) {

				usleep(50000);

				if (read(fd, &buf, nBytesReceive) == nBytesReceive) {
					value = atoi(buf);
				}
			}
		}
	}

	close(fd);
	return value;
}

uint processOutput(float temperature, int rain, int humidity, int forecast, int actuator, long t_actuator) {
	uint temp;
	temp = 0;
	if (actuator == 0) {
		if (humidity != -1 && forecast != -1) { // sensor de humedad y pronóstico funcionan
			if (humidity < 50 && rain != 1 && forecast > 0) //humedad < 50, no llueve ni va a llover hoy
				temp = 1;
			else if (humidity < 40 && rain != 1) // humedad < 40, no está lloviendo pero hoy va a llover
				temp = 1;
			else if (humidity < 35) // crítico: humedad < 35
				temp = 1;
		}

		else if (humidity != -1) { // no tenemos pronóstico
			if (humidity < 50 && rain != 1) // humedad < 40, no está lloviendo
				temp = 1;
			else if (humidity < 35) // crítico: humedad < 35
				temp = 1;
		}

		else if (forecast != -1) { // sensor de humedad no funciona
			if (forecast > 0 && rain != 1 && t_actuator > 43200) temp = 1; // no está lloviendo ni va a llover hoy pero hace mas de 12 horas que no se enciende
			else if (rain != 1 && t_actuator > 64800) temp = 1; // va a llover pero hace mas de 18 horas que no se enciende
		}

		else // no funciona el sensor de humedad ni hay pronóstico
			if (rain != 1 && t_actuator > 43200) temp = 1; // no llueve y hace mas de 12 horas que no se enciende
	}

	else {
		if (humidity != -1) { //funciona el sensor de humedad
			if (humidity > 60) temp = 1; // apagamos el actuator si la humedad > 60
		}
		else { // no funciona el sensor de humedad
			if (t_actuator > 1800) temp = 1; // si el riego lleva 30 minutos encendido
		}
	}

	return temp;
}

int setActuator(int device, int signal) {

	int fd;
	unsigned char cmd[16];
	char buff[1];
	int recibido = 0;
	
	if ((fd = open("/dev/i2c-0", O_RDWR)) >= 0) {

		if (ioctl(fd, I2C_SLAVE, device) >= 0) {
		
			cmd[0] = signal;
			if (write(fd, cmd, 1) == 1) {
				usleep(10000);
				if (read(fd, buff, 1) == 1) {
					recibido = (int) buff[0];
				}
			}
		}
	}

	close(fd);

	return recibido;

}