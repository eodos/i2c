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
	char * value = malloc(10 * sizeof(char));

	nombre_fichero = ruta((char *) variable);
	f = fopen((char *) nombre_fichero,"r");

	if (f != NULL) {
		fscanf(f, "%s", &value);
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