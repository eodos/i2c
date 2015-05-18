#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {

	FILE *f;
	char *data;
	char * pch[99];
	char * nombre_fichero;

	printf("Content-Type: application/json\n\n");

	data = getenv("QUERY_STRING");

	if (data != NULL) {

		int i=0;
		pch[0] = (char *) strtok(data,"=");

		while (pch[i] != NULL) {
			i++;
			pch[i] = (char *) strtok(NULL,"=");
		}

	}

	nombre_fichero = ruta(pch[0]);

	f = fopen(nombre_fichero,"w");

	if (f != NULL) {
		fprintf(f, "%s", pch[1]);
		fclose(f);
		printf("Success");
	}
}