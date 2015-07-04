extern char * ruta(char * variable);
extern char * readVar(char * variable);
extern int writeVar(char * variables_array[], char * values_array[]);
extern char * getTime();
extern int readSensor(int device, char * sensor, uint nBytesSend, uint nBytesReceive);
extern uint processOutput(float temperature, int rain, int humidity, int forecast, int actuator, long t_actuator, int humidity_min, int humidity_forecast_min, int humidity_critical_min, int humidity_off, int humidity_time, int humidity_forecast_time, int humidity_time_off);
extern int setActuator(int device, int signal);