#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "json-builder.h"

#define RUTA "vars/VARS"

int main() {

	FILE * f;
	char * header;
	char data[80];
	char * vars[99];
	char * values[99];

	int nelements;
	int i;

	size_t len = 0;

	json_value * json_obj = json_object_new(0);

	printf("Content-Type: application/json\n\n");

	// Abrimos el LOG
	f = fopen(RUTA, "r");

	if (f == NULL)
		exit(EXIT_FAILURE);    

    // Leemos el header
	getline(&header, &len, f);

	// Eliminamos el último caracter
	header[strlen(header) - 1] = '\0';
 
    // Buscamos la última línea del archivo y la leemos
    int num_lineas;
    int ch;
    while (fgets(data, 80, f) != NULL) {
    	if (ch = fgetc(f) == EOF)
    		break;
    	else {
    		fseek(f, -1, SEEK_CUR);
    		num_lineas ++;
    	}
    }

    // Eliminamos el último caracter
	data[strlen(data) - 1] = '\0';

    // Cerramos el archivo
	fclose(f);

	// Parseamos el header (vars)
	i=0;
	vars[0] = (char *) strtok(header,",");

	while (vars[i] != NULL) {
		i++;
		vars[i] = (char *) strtok(NULL,",");
	}

	// Parsemos los datos (values)
	i=0;
	values[0] = (char *) strtok(data,",");

	while (values[i] != NULL) {
		i++;
		values[i] = (char *) strtok(NULL,",");
	}
	nelements = i;

	// Convertimos a objeto JSON e imprimimos
		for (i=0; i<nelements; i++) {
		json_object_push(json_obj, (json_char *) vars[i], json_string_new(values[i]));
	}

	char * buf = malloc(json_measure(json_obj));
	json_serialize(buf, json_obj);

	printf("%s\n", buf);
}