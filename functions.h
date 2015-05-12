extern char * ruta(char * variable);
extern char * readVar(char * variable);
//extern int writeVar(char * variable, float value, char * timestamp);
extern int writeVar(char * variables_array[], char * values_array[]);
extern char * getTime();
extern int readSensor(int device, char * sensor, uint nBytesSend, uint nBytesReceive);