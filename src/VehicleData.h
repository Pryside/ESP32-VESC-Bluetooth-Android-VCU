//#define DEBUG

#define RXD2 16 //SerialRX for VescTx pin
#define TXD2 17 //SerialTx for VescRx pin

#define IN_MSPL 5 //multisampling of analog inputs

#define T_CTRL 50 //ms Interval of Control (this affects PI Controller)

#define THROTTLEPIN 25
#define THROTTLELOW 650
#define THROTTLEHIGH 1900
#define THROTTLEHIGH_ERROR 2300
#define THROTTLERATE 200

#define BRAKEPIN 27 //GPIO Pin for Analog Brake Signal, use 0.1uF Cap!
#define BRAKELOW 100 //Lowest brake signal
#define BRAKEHIGH 2300  //Highes brake signal

#define PAS_PIN 14  //GPIO Pullup pin for PAS, use 100pF & 4.7k external (extra) pullup

#define PAS_MAGNETS 20  //Number of magnets on the PAS Disk * 2
#define PAS_SMOOTHING 14 //removes fluctuation in cadence readings
#define PAS_MAXTIME 300 //how long to wait before stopping, defines slowest speed
//max output power (254 full throttle, 127 no power, 0 full brake)
#define PAS_MAXOUTPUT 240 //highes Power PAS out (127-254)
#define PAS_MINOUTPUT 10 //highes braking PAS output (127-0)

#define PAS_MAXOUTPUTLOCKED 180 //highest power when power when locked
#define PAS_MINOUTPUTLOCKED 50 //highest braking when in locked mode

#define PAS_NOOUTPUT 127 //middle setting, no output
//wanted cadence on PAS
#define PAS_SETSPEED 70 //wanted confortable cadence (for control loop)
//PAS controller P and I constants
#define KP_PAS 1.5F  //Kp constant for controller
#define KI_PAS 1.1F //Ki constant for controller !T_CTRL DEPENDENT!
#define KD_PAS 0.2F  //Kd constant for controller !T_CTRL DEPENDENT!
//set the max speed in lockdown mode
#define MAX_SPEED_LOCK 23.0 //speed at which lock starts
#define MAX_SPEED_LOCK_END 28.0 //speed at which lock ends
#define LOCKDOWN_EN 6 //pulses requires for lockdown
#define LOCKDOWN_DIS 20 //puleses to remove lockdown

//Motor and Wheel Specs
#define WHEELCIRCUM 2.3F //circumference in meter
#define GEARRATIO 1.0F  //Gear ratio
#define POLEPAIRS 23    //Number of Motor Magnets / 2
#define CORRECTION 6    //Correction factor for distance measurement


//Battery Specs
#define SHUTDOWN_THESH_V 15.5F //Shutdown voltage
#define BATTERY_IR 0.03F //IR of the whole Pack
#define BATTERY_V_FULL 49.8F
#define BATTERY_V_EMPTY 38.4F
#define BATTERY_FULL_WH 878.4F