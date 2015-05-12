#include <stdio.h>
#include <stdlib.h> // exit
#include <time.h>
#include <wiringPiI2C.h>
#include <errno.h>

#define ARDUINO_ADDRESS 0x04
#define TI_ADDRESS 0x08


char* getTime();

int main() {
	int TAG_TEMP = 1;
	float temperature;
	int value;

	int fd;
	int x;
	
	printf("Content-Type: text/html\n\n");
	printf("<html><body>");
	
	printf("I2C: Conectando...<br/>");

	// I2C setup
	if ( (fd = wiringPiI2CSetup (ARDUINO_ADDRESS)) < 0 ) {
		printf("I2C: error de acceso");
		exit(1);
	}
	else printf("I2C: Conectado<br/>");

	// send TEMP request
	if ( (x = wiringPiI2CWrite (fd, TAG_TEMP)) < 0 ) {
		printf("I2C: error de escritura<br/>");
		exit(1);
	}
	else printf("I2C: Escritura correcta: %d<br/>", TAG_TEMP);

	usleep(10000);

	// read reponse
	value = wiringPiI2CRead (fd);
	if ( value < 0 ) {
		printf("I2C: error de lectura");
		exit(1);
	}

	temperature = value / 100.0;
	printf("value: %d<br/>", value);
	printf("temperature: %.2f<br/>", temperature);
	printf("%s<br/>", getTime());
		
	
	printf("<br/>I2C: Terminando...");
	printf("</body></html>");

	return (EXIT_SUCCESS);
}

char* getTime() {
	time_t current_time;
	char* c_time_string;

	// Obtain current time as seconds elapsed since the Epoch.
	current_time = time(NULL);

	// Convert to local time format.
    c_time_string = ctime(&current_time);

    return c_time_string;
}