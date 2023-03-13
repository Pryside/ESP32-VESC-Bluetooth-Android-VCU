//#define DEBUG

#define RXD2 16
#define TXD2 17

#define THROTTLEPIN 34
#define THROTTLELOW 1200
#define THROTTLEHIGH 2900
#define THROTTLERATE 200

#define BRAKEPIN 35 //NOT FINAL PIN!!
#define BRAKELOW 1900
#define BRAKEHIGH 2500

#define PAS_PIN 36

#define PAS_SMOOTHING 4
#define PAS_MAXTIME 1000
//max output power (254 full, 127 no power)
#define PAS_MAXOUTPUT 180
//wanted cadence on PAS
#define PAS_SETSPEED 50
//PAS controller P and I constants
#define KP_PAS 1
#define KI_PAS 0.1f


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
#define BATTERY_IR 0.03F //IR of the whole Pack
#define BATTERY_V_FULL 49.8F
#define BATTERY_V_EMPTY 38.4F
#define BATTERY_FULL_WH 1011.0F