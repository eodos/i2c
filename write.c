#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {

	FILE *f;
	char *data;
	char var[20];
	char * nombre_fichero;
	int value;

	printf("Content-Type: application/json\n\n");

	data = getenv("QUERY_STRING");

	if (data != NULL)
		sscanf(data,"%[^'=']=%d",&var,&value);

	nombre_fichero=ruta(var);

	f = fopen(nombre_fichero,"w");

	if (f != NULL) {
		fprintf(f, "%d",value);
		fclose(f);
		printf("Success");
	}
}