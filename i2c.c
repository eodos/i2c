#include <stdio.h>
#include <stdlib.h> // exit
#include <fcntl.h> // open modo O_RDWR
#include <linux/i2c-dev.h>
 
#define ADDRESS 0x04
 
int main() {
    int file;
    unsigned char cmd[16];
    int enviado = 1;
    char buff[1];
    int recibido;
    
    printf("Content-Type: text/html\n\n");
    printf("<html>");
    printf("<head><title>Test</title></head");
    printf("<body>");
 
    
    printf("I2C: Conectando...</br>");
    if ((file = open("/dev/i2c-0", O_RDWR)) < 0) {
        printf("I2C: error de acceso");
        exit(1);
    }
    
    printf("I2C: Accediendo al dispositivo</br>");
    if (ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
        printf("I2C: error al acceder al esclavo en 0x%x", ADDRESS);
        exit(1);
    }
    
    printf("Enviado %d</br>", enviado);
    cmd[0] = enviado;
    if (write(file, cmd, 1) == 1) {
        usleep(10000);
        if (read(file, buff, 1) == 1) {
            recibido = (int) buff[0];
            printf("Recibido %d</br>", recibido);
        }
    }
    
    printf("I2C: Terminando...");
    printf("</html>");
    close(file);
    return (EXIT_SUCCESS);
}