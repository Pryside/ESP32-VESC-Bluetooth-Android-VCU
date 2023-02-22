#define RXD2 16
#define TXD2 17

#define THROTTLEPIN 34
#define THROTTLELOW 1200
#define THROTTLEHIGH 2900
#define THROTTLERATE 200

#define BRAKEPIN 35 //NOT FINAL PIN!!
#define BRAKELOW 1900
#define BRAKEHIGH 2500


// const float WheelCircum = 0.615;
// const float GearRatio = 1;
// const int PolePairs = 14;
#define WHEELCIRCUM 0.615
#define GEARRATIO 1.0
#define POLEPAIRS 15
#define CORRECTION 6


#define SIZEOF_BT_RECIEVE 7
#define ENDOF_RECIEVE 'set\n'
#define ENDOF_RECIEVE_LEN 4


#define BLUETOOTH_NAME "ESP32test"

#define SHUTDOWN_THESH_V 15.5F //Shutdown voltage