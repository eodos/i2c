#include <stdio.h>
#include <stdlib.h> // exit
#include <fcntl.h> // open modo O_RDWR
#include <linux/i2c-dev.h>
#include <time.h>

#define ARDUINO_ADDRESS 0x04
#define TI_ADDRESS 0x08


char* getTime();

int main() {
	int fd;
	char TAG_TEMP[] = "TEMP";
	uint nBytesTAG_TEMP = 4;
	uint nBytesTEMP = 4;
	float temperature;
	char buf[4];
	int value;
	
	printf("Content-Type: text/html\n\n");
	printf("<html>");
	printf("<body>");
	
	printf("I2C: Conectando...<br/>");
	if ((fd = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("I2C: error de acceso");
		exit(1);
	}
	
	printf("I2C: Accediendo al Arduino...<br/><br/>");
	if (ioctl(fd, I2C_SLAVE, TI_ADDRESS) < 0) {
		printf("I2C: error al acceder al esclavo en 0x%x", TI_ADDRESS);
		exit(1);
	}
	
	// TEMPERATURE request
	printf("var enviada: %s<br/>", TAG_TEMP);
	if (write(fd, TAG_TEMP, nBytesTAG_TEMP) == nBytesTAG_TEMP) {

		usleep(50000);

		if (read(fd, &buf, nBytesTEMP) == nBytesTEMP) {

			value = atoi(buf);
			printf("valor: %d<br/>", value);
			temperature = (float) value / (float) 100.00;
			printf("temperatura: %.2f C<br/>", temperature);
			printf("%s<br/>", getTime());
		}
	}
	else printf("envio incorrecto<br/>");

	printf("<br/>I2C: Terminando...");
	printf("</body></html>");
	close(fd);
	return (EXIT_SUCCESS);
}

char* getTime() {
	time_t current_time;
	char* c_time_string;

	/* Obtain current time as seconds elapsed since the Epoch. */
	current_time = time(NULL);

	/* Convert to local time format. */
    c_time_string = ctime(&current_time);

    return c_time_string;
}