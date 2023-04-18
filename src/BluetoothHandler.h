#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "VehicleData.h"
#include "FlashSave.h"
#include "VescUart.h"
#include "BatteryModel.h"

#define BT_VARS 14
#define BT_NOTIFYS 4
#define BT_READS 10

static BLEServer* g_pServer = nullptr;

//static BLECharacteristic* g_pCharWrite = nullptr;

static BLECharacteristic* g_pCharNotify[BT_NOTIFYS] =
{nullptr, nullptr, nullptr, nullptr};

static BLECharacteristic* g_pCharRead[BT_READS] =
{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr};

static bool g_centralConnected = false;
static std::string g_cmdLine;

#define SERVICE_UUID "25AE1441-05D3-4C5B-8281-93D4E07420CF"

enum varnames {
    batCurr, kmh, inpVolt, motorCurr, tempMotor, ampHours, ampHoursC,
    km, wattHours, wattHoursC, totalWh, totalWhC, totalkm, batPercent
};

const String Notify_UUIDs[BT_NOTIFYS][2] ={
{"25AE1447-05D3-4C5B-8281-93D4E07420CF" , "batCurr"     },
{"25AE1448-05D3-4C5B-8281-93D4E07420CF" , "kmh"         },
{"25AE1449-05D3-4C5B-8281-93D4E07420CF" , "inpVolt"     },
{"25AE1446-05D3-4C5B-8281-93D4E07420CF" , "motorCurr"   }
};

const String Read_UUIDs[BT_READS][2] ={
{"25AE1445-05D3-4C5B-8281-93D4E07420CF" , "tempMotor"   },
{"25AE1450-05D3-4C5B-8281-93D4E07420CF" , "ampHours"    },
{"25AE1451-05D3-4C5B-8281-93D4E07420CF" , "ampHoursC"   },
{"25AE1452-05D3-4C5B-8281-93D4E07420CF" , "km"          },
{"25AE1453-05D3-4C5B-8281-93D4E07420CF" , "wattHours"   },
{"25AE1454-05D3-4C5B-8281-93D4E07420CF" , "wattHoursC"  },
{"25AE1455-05D3-4C5B-8281-93D4E07420CF" , "totalWh"     },
{"25AE1456-05D3-4C5B-8281-93D4E07420CF" , "totalWhC"    },
{"25AE1457-05D3-4C5B-8281-93D4E07420CF" , "totalkm"     },
{"25AE1458-05D3-4C5B-8281-93D4E07420CF" , "batPercent"  }
};

class BluetoothHandler{

private:
    
    float SendData[BT_VARS];
    
    const float ratioPulseDistanceKm = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION)/1000.0;
    const float ratioSpeedKmh = 60 / ((float)(POLEPAIRS)*1000.0) * GEARRATIO * WHEELCIRCUM;
    
    int notify_counter = 0;
    int read_counter = BT_NOTIFYS;

public:

    BatteryModel BatModel;
    FlashSave Flash;

    float lastAmpHours, lastInputVoltage, lastAmpHoursCharged;

    void CalculateData(VescUart getUart);
    
    void SendNotifyBTData();
    void SetReadBTData();

    float getBTValue(varnames name);

    void init();

    bool GetBTData(uint8_t *buffer);

};
