#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "json-builder.h"

int main() {

	FILE * f;
	char * data;
	char * pch[20];
	char * vars[20];
	char values[20];
	char * nombre_fichero;
	json_value * json_obj = json_object_new(0);

	printf("Content-Type: application/json\n\n");

	data = (char *) getenv("QUERY_STRING");

	if (data != NULL) {

		int i=0;
		pch[0] = (char *) strtok(data,"&");

		while (pch[i] != NULL) {
			i++;
			pch[i] = (char *) strtok(NULL,"&");
		}

		int nelements=i;

		for (i=0; i<nelements; i++) {
			sscanf(pch[i],"var=%s", &vars[i]);
			nombre_fichero = ruta((char *) &vars[i]);
			f = fopen((char *) nombre_fichero,"r");

			if (f != NULL) {
				fscanf(f, "%s", &values[i]);
				json_object_push(json_obj, (json_char *) &vars[i], json_string_new(&values[i]));
				fclose(f);
			}
		}

		char * buf = malloc(json_measure(json_obj));
		json_serialize(buf, json_obj);

		printf("%s\n", buf);
	}
}