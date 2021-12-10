// v2  added support of 2438
// v3  added support of house heating
// v4  new prototype board with RJ45
// v5  real time - not succesfull
// v6  added config page, heating removed
// v7  added control via terminal
// v8  added sauna button control 
// v9  added post time 
// v10 added post email 
// v11 added tank readings 
// v12 25/12/2020 added ATREA motor 
// v13 11/01/2021 added added 18B20 test pin for C2 and heating 2NP
// v14 12/02/2021 new version sauna control + safety sauna timeout

#include <OneWire.h>                // verze 2.3.5
#include <DallasTemperature.h>      // verze 3.8.0
//#include <Wire.h>
#include <BH1750FVI.h>
//#include <SPI.h>                    // verze 1.1.1
#include <Ethernet.h>               // verze 2.0.0
#include <DS2438.h>                 // local installation

  String current_version_text      = "v14";

//define the 1-Wire for humidity
//26F812BC000000CE
 // uint8_t DS2438_address[] = { 0x26, 0xF8, 0x12, 0xBC, 0x00, 0x00, 0x00, 0xCE };
//  uint8_t DS2438_address[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  
//  uint8_t DS2438_address_humidity[] = { 0x26, 0xF8, 0x12, 0xBC, 0x00, 0x00, 0x00, 0xCE};
//  uint8_t DS2438_address_tank[]     = { 0x26, 0x4e, 0x30, 0xbc, 0x00, 0x00, 0x00, 0x00 }; // tlakove cidlo
//  uint8_t DS2438_address_tank2[]    = { 0x26, 0x15, 0xb1, 0xdb, 0x00, 0x00, 0x00, 0xc6 }; // tlakove cidlo sklipek

  uint8_t DS2438_address_humidity[] = { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 };
  uint8_t DS2438_address_tank[]     = { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22 }; // tlakove cidlo
  uint8_t DS2438_address_tank2[]    = { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33 }; // tlakove cidlo sklipek



//  BH1750FVI LightSensor;
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes); 

  #define pin_IN_Garage_door_open       41     // Garage open doors
  #define pin_OUT_Garage_LED            37     // Garage LED info
  #define pin_OUT_Garage_door_control   35     // Garage dvere ovladani
  #define pin_OUT_Brana_control         39     // Vrata ovladani
  
  #define pin_IN_Motor_ATREA            42     // motor ATREA
  #define pin_OUT_Motor_ATREA           47     // motor ATREA

  #define pin_IN_HDO                    33     // HDO info
  #define pin_OUT_HDO                   45     // HDO rele

  #define pin_IN_kotel                  31     // kotel info
  #define pin_IN_Armed_house            29     // Armed house - Jablotron

  #define pin_OUT_rele_heating_2NP      46     // sauna heating 2NP

  #define pin_OUT_rele_sauna            49     // sauna rele
  #define pin_OUT_sauna_LED_OK          23     // sauna teplota OK
  #define pin_OUT_sauna_LED_low         25     // sauna teplota nizka
  #define pin_IN_sauna_button           27     // sauna tlacotko on/off
  
  #define pin_counter_1                 2
  #define pin_counter_2                 3
  #define pin_counter_3                18
  #define pin_counter_4                19

  #define pin_DS2438_humi               7
  #define pin_DS2438_tank               6
  #define pin_DS2438_tank2              4

  #define pin_OUT_rele_min             43     // out rele min
  #define pin_OUT_rele_max             49     // out rele max

// Data connection of temp sensors ds1820 on pins 11,12,5
  #define ds18b20_BUS1   11 // 3-wire bus with counters 
  #define ds18b20_BUS2   12 // 3-wire bus
  #define ds18b20_BUS3   5  // 2-wire bus 
  #define ds18b20_BUS4   8  // 2-wire bus - Atrea C2 reading 

// Create a oneWire instance for communication
  OneWire oneWire_18B20_BUS1(ds18b20_BUS1);
  OneWire oneWire_18B20_BUS2(ds18b20_BUS2);
  OneWire oneWire_18B20_BUS3(ds18b20_BUS3);  
  OneWire oneWire_18B20_BUS4(ds18b20_BUS4);
  OneWire oneWire_humi(pin_DS2438_humi);
  OneWire oneWire_tank(pin_DS2438_tank);
  OneWire oneWire_tank2(pin_DS2438_tank2);

// Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors_18B20_BUS1(&oneWire_18B20_BUS1);
  DallasTemperature sensors_18B20_BUS2(&oneWire_18B20_BUS2);
  DallasTemperature sensors_18B20_BUS3(&oneWire_18B20_BUS3);
  DallasTemperature sensors_18B20_BUS4(&oneWire_18B20_BUS4);
  
  DS2438 ds2438_humi(&oneWire_humi, DS2438_address_humidity);
  DS2438 ds2438_tank(&oneWire_tank, DS2438_address_tank);
  DS2438 ds2438_tank2(&oneWire_tank2, DS2438_address_tank2);

// Used to store device address
  DeviceAddress deviceAddress;
  String  str;
  String  HW_adresses_BUS1[15];
  String  HW_adresses_BUS2[15];
  String  HW_adresses_BUS3[15];
  String  HW_adresses_BUS4[15];

  String  HW_adresses_DS2438_humi    ="";
  String  HW_adresses_DS2438_tank    ="";
  String  HW_adresses_DS2438_tank2   ="";
    
  int DS18B20_count_BUS1=0;
  int DS18B20_count_BUS2=0;
  int DS18B20_count_BUS3=0;
  int DS18B20_count_BUS4=0;
  

  const long interval_sauna_safety                = 9000000;        // interval for sauna safety switch off 2.5h
  const long interval_sauna                       = 1000;           // interval for sauna control
  const long interval_heating_2NP                 = 1000;           // interval for hesting 2NP control
  const long interval_sensor_read_DS18B20         = 30000;          // interval for sensors reading
  const long interval_sensor_read_DS2438          = 60000;          // interval for sensors reading
  const long interval_counter_check               = 60000;          // interval for checking counters and setting statuses

  unsigned long previousMillis_sauna                    = 0;
  unsigned long millis_sauna_safety_begin               = 0;
  unsigned long previousMillis_heating_2NP              = 0;
  unsigned long previousMillis_sensor_read_DS18B20      = 0;  
  unsigned long previousMillis_sensor_read_DS2438       = 0;  
  unsigned long previousMillis_counter_check            = 0;

  volatile  unsigned long counter1 = 0;
  volatile  unsigned long counter2 = 0;
  volatile  unsigned long counter3 = 0;
  volatile  unsigned long counter4 = 0;

  volatile  unsigned long prev_counter1 = 0;
  volatile  unsigned long prev_counter2 = 0;
  volatile  unsigned long prev_counter3 = 0;
  volatile  unsigned long prev_counter4 = 0;

  int status_post     = 0;
  int status_post_2   = 0; // for email post acknovledgement
  long time_post      = 0;
  int status_boiler   = 0;
  int status_kotel    = 0;
  int status_Atrea    = 0;

  volatile short  sauna_status          = 0;
  String sauna_status_text              = "idle";
  int sauna_temp_required               = 12;
  int sauna_temp_actual                 = 1000;
  const int sauna_temp_required_const   = 95;
  int sauna_HDO_used                    = 1;  //je pozadovano rizeni tarifem HDO

  volatile short heating_2NP_status     = 0;
  String heating_2NP_status_text        = "idle";
  int heating_2NP_temp_required         = 21;
  int heating_2NP_temp_actual           = 1000;

  unsigned long time;
  String command;
 
  // Setup the static network info for you arduino
  IPAddress ip(192, 168, 111, 90);
  byte subnet[]   = { 255, 255, 255, 0 };                   // Subnet Mask
  byte gateway[]  = { 192, 168, 111, 1 };                   // Gateway
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
 
  EthernetServer server = EthernetServer(80); // Port 80
  EthernetClient client;

  String HTTPget = "";
  boolean reading = false;

  float humidity_temperature;
  float humidity_value;
  float humidity_CHA;
  float humidity_CHB;

  float tank_temperature;
  float tank_value, tank_valuex;
  float tank_CHA;
  float tank_CHB;
  float tank_empty = 0.21;
  float tank_full  = 1.4;

  float tank2_temperature;
  float tank2_value, tank2_valuex;
  float tank2_CHA;
  float tank2_CHB;
  float tank2_empty = 0.18;
  float tank2_full  = 1.45;

  
// ========================================   18B20 init ============================================  
void init_DS1820()
{ 
  // Start up the library
    sensors_18B20_BUS1.begin();  
    sensors_18B20_BUS2.begin();
    sensors_18B20_BUS3.begin();      
    sensors_18B20_BUS4.begin();  

  Serial.println("-------------- BUS 1 - 3wire with counters  -----------------");
  DS18B20_count_BUS1 = sensors_18B20_BUS1.getDeviceCount();
  Serial.print("Bus 1: "); Serial.print(DS18B20_count_BUS1); Serial.println(" device(s) connected.");
  for (int i = 0; i < DS18B20_count_BUS1; i++)
  {
    HW_adresses_BUS1[i]="";
    sensors_18B20_BUS1.getAddress(deviceAddress, i);    
    for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ HW_adresses_BUS1[i]+="0";}
        HW_adresses_BUS1[i]+=String(deviceAddress[ii], HEX);
      }
    HW_adresses_BUS1[i].toUpperCase();
    Serial.println(HW_adresses_BUS1[i]);
  }
  
  Serial.println("-------------------- BUS 2 - 2wire  -------------------------");
  DS18B20_count_BUS2 = sensors_18B20_BUS2.getDeviceCount();
  Serial.print("Bus 2: "); Serial.print(DS18B20_count_BUS2); Serial.println(" device(s) connected.");
  for (int i = 0; i < DS18B20_count_BUS2; i++)
  {
    HW_adresses_BUS2[i]="";
    sensors_18B20_BUS2.getAddress(deviceAddress, i);    
    for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ HW_adresses_BUS2[i]+="0";}
        HW_adresses_BUS2[i]+=String(deviceAddress[ii], HEX);
      }
    HW_adresses_BUS2[i].toUpperCase();
    Serial.println(HW_adresses_BUS2[i]);
  }

  Serial.println("-------------------- BUS 3 - 3wire  -------------------------");
  DS18B20_count_BUS3 = sensors_18B20_BUS3.getDeviceCount();
  Serial.print("Bus 3: "); Serial.print(DS18B20_count_BUS3); Serial.println(" device(s) connected.");
  for (int i = 0; i < DS18B20_count_BUS3; i++)
  {
    HW_adresses_BUS3[i]="";
    sensors_18B20_BUS3.getAddress(deviceAddress, i);  
      for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ Serial.print("0");}
        Serial.print(deviceAddress[ii], HEX);
        if( deviceAddress[ii] < 0x10){ HW_adresses_BUS3[i]+="0";}
        HW_adresses_BUS3[i]+=String(deviceAddress[ii], HEX);
        HW_adresses_BUS3[i].toUpperCase();
      }
    Serial.println();
  }

  Serial.println("-------------------- BUS 4 - 3wire Atraea C2 -------------------------");
  DS18B20_count_BUS4 = sensors_18B20_BUS4.getDeviceCount();
  Serial.print("Bus 4: "); Serial.print(DS18B20_count_BUS4); Serial.println(" device(s) connected.");
  for (int i = 0; i < DS18B20_count_BUS4; i++)
  {
    HW_adresses_BUS4[i]="";
    sensors_18B20_BUS4.getAddress(deviceAddress, i);  
      for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ Serial.print("0");}
        Serial.print(deviceAddress[ii], HEX);
        if( deviceAddress[ii] < 0x10){ HW_adresses_BUS4[i]+="0";}
        HW_adresses_BUS4[i]+=String(deviceAddress[ii], HEX);
        HW_adresses_BUS4[i].toUpperCase();
      }
    Serial.println();
  }

  
  // Get bit resolution of our device
  int resolution = sensors_18B20_BUS1.getResolution(deviceAddress);
  Serial.print("Resolution is set to: ");Serial.print(resolution);  Serial.println(" bits."); 

  sensors_18B20_BUS1.requestTemperatures();  // nacist data aby byly k dispozici od zacatku
  sensors_18B20_BUS2.requestTemperatures();
  sensors_18B20_BUS3.requestTemperatures();
  sensors_18B20_BUS4.requestTemperatures();
}

// ========================================  read data DS18B20============================================  
void read_sensor_data_DS18B20(void)
{
    unsigned long currentMillis_sensor_read_DS18B20 = millis();
    if (currentMillis_sensor_read_DS18B20 - previousMillis_sensor_read_DS18B20 >= interval_sensor_read_DS18B20)  // kontrola uplynuti 30 sec
    { previousMillis_sensor_read_DS18B20 = currentMillis_sensor_read_DS18B20;
      sensors_18B20_BUS1.requestTemperatures();
      sensors_18B20_BUS2.requestTemperatures();
      sensors_18B20_BUS3.requestTemperatures();
      sensors_18B20_BUS4.requestTemperatures();

      for (int i = 0; i < DS18B20_count_BUS1; i++)  // nacteni teploty sauny pro regulaci
          {
              if (HW_adresses_BUS1[i]=="28FFABE20315020E") 
                {sauna_temp_actual = sensors_18B20_BUS1.getTempCByIndex(i);
                }
          }

      for (int i = 0; i < DS18B20_count_BUS2; i++)  // nacteni teploty 2NP vychod pro regulaci heating 2NP
          {
              if (HW_adresses_BUS2[i]=="281F5AF30100001B") 
                {heating_2NP_temp_actual = sensors_18B20_BUS2.getTempCByIndex(i);
                }
          }

          
    } 
}

// ========================================  read humidity DS2348============================================  
void read_humidity(void)
  {
        ds2438_humi.update();
        humidity_temperature  =   ds2438_humi.getTemperature();
        humidity_CHA          =   ds2438_humi.getVoltage(DS2438_CHA);
        humidity_CHB          =   ds2438_humi.getVoltage(DS2438_CHB);
        float rh              =   (ds2438_humi.getVoltage(DS2438_CHA) / ds2438_humi.getVoltage(DS2438_CHB) - 0.16) / 0.0062;
        humidity_value        =   (float)(rh / (1.0546 - 0.00216 * humidity_temperature));
        if (humidity_value < 0.0) {
            humidity_value = 0.0;
        } else if (humidity_value > 100.0) {
            humidity_value = 100.0;
        }
}

// ========================================  read tank DS2348============================================  
void read_tank(void)
{
        ds2438_tank.update();
        tank_temperature  =   ds2438_tank.getTemperature();
        tank_CHA          =   ds2438_tank.getVoltage(DS2438_CHA);
        tank_CHB          =   ds2438_tank.getVoltage(DS2438_CHB);
        tank_valuex        =   (tank_CHA - tank_empty)/(tank_full- tank_empty);
        tank_value        =   tank_valuex < 0 ? -1: 100*tank_valuex;       
}

// ========================================  read tank2 DS2348============================================  
void read_tank2(void)
{
        ds2438_tank2.update();
        tank2_temperature  =   ds2438_tank2.getTemperature();
        tank2_CHA          =   ds2438_tank2.getVoltage(DS2438_CHA);
        tank2_CHB          =   ds2438_tank2.getVoltage(DS2438_CHB);
        tank2_valuex        =   (tank2_CHA - tank2_empty)/(tank2_full - tank2_empty);
        tank2_value        =   tank2_valuex < 0 ? -1: 100*tank2_valuex;
}

// ========================================   DS2348 init ============================================  
void init_DS2348()
{ 
  // ----------------   Start up the library - humidity sensor   --------------------------------
     while(oneWire_humi.search(deviceAddress)) {
      for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ HW_adresses_DS2438_humi+="0";}
        HW_adresses_DS2438_humi+=String(deviceAddress[ii], HEX);
        DS2438_address_humidity[ii]=deviceAddress[ii];
      }
      HW_adresses_DS2438_humi.toUpperCase();
     }
    ds2438_humi.begin();
    read_humidity();

   Serial.println("----- humidity device DS2438  on pin "+String(pin_DS2438_humi)+" has HW address "+HW_adresses_DS2438_humi+" -----");
   Serial.println("temperature is "+String(ds2438_humi.getTemperature())+"C");
   Serial.println("CH_A/CH_B is "+String(ds2438_humi.getVoltage(DS2438_CHA))+"/"+String(ds2438_humi.getVoltage(DS2438_CHB)));
   Serial.println("status is  "+String(ds2438_humi.isError()));


  // ----------------   Start up the library - tank sensor   --------------------------------
     while(oneWire_tank.search(deviceAddress)) {
      for (uint8_t ii = 0; ii < 8; ii++)
      { if( deviceAddress[ii] < 0x10){ HW_adresses_DS2438_tank+="0";}
        HW_adresses_DS2438_tank+=String(deviceAddress[ii], HEX);
        DS2438_address_tank[ii]=deviceAddress[ii];
      }
      HW_adresses_DS2438_tank.toUpperCase();
      HW_adresses_DS2438_tank = HW_adresses_DS2438_tank.substring(0 ,16);
     }

    ds2438_tank.begin();
    read_tank();

   Serial.println("-----     tank device DS2438  on pin "+String(pin_DS2438_tank)+" has HW address "+HW_adresses_DS2438_tank+" -----");
   Serial.println("temperature is "+String(ds2438_tank.getTemperature())+"C");
   Serial.println("CH_A/CH_B is "+String(ds2438_tank.getVoltage(DS2438_CHA))+"/"+String(ds2438_tank.getVoltage(DS2438_CHB)));
   Serial.println("status is  "+String(ds2438_tank.isError()));

  // ----------------   Start up the library - tank2 sensor   --------------------------------
     while(oneWire_tank2.search(deviceAddress)) {
     
      for (uint8_t ii = 0; ii < 8; ii++)
      {
       if( deviceAddress[ii] < 0x10){ HW_adresses_DS2438_tank2+="0";}
        HW_adresses_DS2438_tank2+=String(deviceAddress[ii], HEX);
        DS2438_address_tank2[ii]=deviceAddress[ii];
      }
      HW_adresses_DS2438_tank2.toUpperCase();
      HW_adresses_DS2438_tank2 = HW_adresses_DS2438_tank2.substring(0 ,16);
     }

    ds2438_tank2.begin();
    read_tank2();

   Serial.println("-----     tank2 device DS2438  on pin "+String(pin_DS2438_tank2)+" has HW address "+HW_adresses_DS2438_tank2+" -----");
   Serial.println("temperature is "+String(ds2438_tank2.getTemperature())+"C");
   Serial.println("CH_A/CH_B is "+String(ds2438_tank2.getVoltage(DS2438_CHA))+"/"+String(ds2438_tank2.getVoltage(DS2438_CHB)));
   Serial.println("status is  "+String(ds2438_tank2.isError()));

}

// ========================================  read data DS2348============================================  
void read_sensor_data_DS2438(void)
{
    unsigned long currentMillis_sensor_read_DS2438 = millis();
    if (currentMillis_sensor_read_DS2438 - previousMillis_sensor_read_DS2438 >= interval_sensor_read_DS2438)  // kontrola uplynuti 5min
    { previousMillis_sensor_read_DS2438 = currentMillis_sensor_read_DS2438;
        read_humidity();
        read_tank();
        read_tank2();
    } 
}

// ========================================   sent 2438 data to HTTP ============================================  
void sent_2438_data_to_HTTP(void)
{
  ds2438_humi.update();
  client.print("Humidity : "+HW_adresses_DS2438_humi+" on pin "+String(pin_DS2438_humi)+" temp: "+String(ds2438_humi.getTemperature())+" Vcc-Vad: ");
  client.print(String(ds2438_humi.getVoltage(DS2438_CHA))+"-"+String(ds2438_humi.getVoltage(DS2438_CHB))+", is error? ");
  client.println(String(ds2438_humi.isError()));

  ds2438_tank.update();
  client.print("Tank     : "+HW_adresses_DS2438_tank+" on pin "+String(pin_DS2438_tank)+" temp: "+String(ds2438_tank.getTemperature())+" Vcc-Vad: ");
  client.print(String(ds2438_tank.getVoltage(DS2438_CHA))+"-"+String(ds2438_tank.getVoltage(DS2438_CHB))+", is error? ");
  client.println(String(ds2438_tank.isError()));

  ds2438_tank2.update();
  client.print("Tank2    : "+HW_adresses_DS2438_tank2+" on pin "+String(pin_DS2438_tank2)+" temp: "+String(ds2438_tank2.getTemperature())+" Vcc-Vad: ");
  client.print(String(ds2438_tank2.getVoltage(DS2438_CHA))+"-"+String(ds2438_tank2.getVoltage(DS2438_CHB))+", is error? ");
  client.println(String(ds2438_tank2.isError()));

}

// ========================================   sent data to HTTP ============================================  
void sent_data_to_HTTP(void)
{
  for (int i = 0; i < DS18B20_count_BUS1; i++)
  {
  client.print(HW_adresses_BUS1[i]+":"+String(sensors_18B20_BUS1.getTempCByIndex(i))+"\n"); 
    }      
  for (int i = 0; i < DS18B20_count_BUS2; i++)
  {
  client.print(HW_adresses_BUS2[i]+":"+String(sensors_18B20_BUS2.getTempCByIndex(i))+"\n"); 
    }    
  
  for (int i = 0; i < DS18B20_count_BUS3; i++)
  {
  client.print(HW_adresses_BUS3[i]+":"+String(sensors_18B20_BUS3.getTempCByIndex(i))+"\n"); 
    }    
  
  for (int i = 0; i < DS18B20_count_BUS4; i++)
  {
  client.print(HW_adresses_BUS4[i]+":"+String(sensors_18B20_BUS4.getTempCByIndex(i))+"\n"); 
    }    

  client.print(HW_adresses_DS2438_humi+":"+String(humidity_temperature)+":"+String(humidity_CHA)+":"+String(humidity_CHB)+"\n");
  client.print(HW_adresses_DS2438_tank+":"+String(tank_temperature)+":"+String(tank_CHA)+":"+String(tank_CHB)+"\n");
  client.print(HW_adresses_DS2438_tank2+":"+String(tank2_temperature)+":"+String(tank2_CHA)+":"+String(tank2_CHB)+"\n");
 
  client.print("BH1750_svit:"+String(LightSensor.GetLightIntensity())+"\n");

  client.print("counter_2_post:"    +String(counter1)+"\n");
  client.print("counter_3_ATREA:"   +String(counter2)+"\n");       
  client.print("counter_18_boiler:" +String(counter3)+"\n");     
  client.print("counter_19_kotel:"  +String(counter4)+"\n");  

  client.print("time_post:"+String(time_post)+"\n");
  client.print("status_post:"+String(status_post)+"\n");
  client.print("status_post_2:"+String(status_post_2)+"\n");
  client.print("status_boiler:"+String(status_boiler)+"\n");       
  client.print("status_kotel:"+String(status_kotel)+"\n");     
  client.print("status_Atrea:"+String(status_Atrea)+"\n");  

  client.print("sauna_text:"+sauna_status_text+"\n");          
  client.print("sauna_status:"+String(sauna_status)+"\n");          
  client.print("sauna_HDO_used:"+String(sauna_HDO_used)+"\n"); 
  client.print("sauna_temp_required:"+String(sauna_temp_required)+"\n");  
  client.print("sauna_temp_actual:"+String(sauna_temp_actual)+"\n"); 
  if (sauna_status == 0)    client.print("sauna_safety: NA\n");
  else                      client.print("sauna_safety:"+String((interval_sauna_safety - millis()+ millis_sauna_safety_begin) / 60000 )+"\n"); 

  client.print("heating_2NP_text:"+heating_2NP_status_text+"\n");          
  client.print("heating_2NP_status:"+String(heating_2NP_status)+"\n");          
  client.print("heating_2NP_temp_required:"+String(heating_2NP_temp_required)+"\n");  
  client.print("heating_2NP_temp_actual:"+String(heating_2NP_temp_actual)+"\n"); 

  client.print("info_house_armed:"+String(digitalRead(pin_IN_Armed_house)?0:1)+"\n"); 
  client.print("info_kotel:"+String(digitalRead(pin_IN_kotel))+"\n");            
  client.print("info_HDO:"+String(digitalRead(pin_IN_HDO))+"\n");             
  client.print("info_garage:"+String(digitalRead(pin_IN_Garage_door_open))+"\n");           
  client.print("info_motor_ATREA:"+String(!digitalRead(pin_IN_Motor_ATREA))+"\n");           

  client.print("humidity_temperature:"+String(humidity_temperature)+"\n");           
  client.print("humidity_value:"+String(humidity_value)+"\n");           
  client.print("tank_temperature:"+String(tank_temperature)+"\n");           
  client.print("tank_value:"+String(tank_value)+"\n");           
  client.print("tank2_temperature:"+String(tank2_temperature)+"\n");           
  client.print("tank2_value:"+String(tank2_value)+"\n");      
  
   for (int i = pin_OUT_rele_min; i <= pin_OUT_rele_max; i++)
   {
    client.print("rele"+String(i)+":"+String(digitalRead(i))+"\n");           
   } 
    
  client.print("uptime:"+String((long)millis()/1000)+"\n");  
  client.print((char)4); // EOF character
}

// ========================================  Counters Interupt init ============================================  
void init_interupt()
{
    pinMode(pin_counter_1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_counter_1), interupt1_inc, CHANGE); 
    pinMode(pin_counter_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_counter_2), interupt2_inc, CHANGE); 
    pinMode(pin_counter_3, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_counter_3), interupt3_inc, CHANGE); 
    pinMode(pin_counter_4, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_counter_4), interupt4_inc, CHANGE); 
}

// ========================================   Increment counters  ============================================  
  void interupt1_inc(void)
    {  counter1+=1;    }
  void interupt2_inc(void)
    {  counter2+=1;    }
  void interupt3_inc(void)
    {  counter3+=1;    }
   void interupt4_inc(void)
    {  counter4+=1;    }

// ========================================   checking counters ============================================  
void checking_counters(void)
{
    unsigned long currentMillis_counter_check = millis();
    if (currentMillis_counter_check - previousMillis_counter_check >= interval_counter_check) {   // kontrola uplynuti 60 sec
    previousMillis_counter_check = currentMillis_counter_check;

  if (prev_counter1 != counter1) 
        {status_post     = 1;
         status_post_2   = 1;
         time_post       = (long)millis()/1000;
        };
        
  if (prev_counter2 != counter2) 
        {status_Atrea     = 1;
        }
        else   
        {status_Atrea     = 0;
        };
        
  if (prev_counter3 != counter3) 
        {status_boiler     = 1;
        }
        else   
        {status_boiler     = 0;
        };
    
  if (prev_counter4 != counter4) 
        {status_kotel     = 1;
        }
        else   
        {status_kotel     = 0;
        };
        
    prev_counter1 = counter1;
    prev_counter2 = counter2;
    prev_counter3 = counter3;
    prev_counter4 = counter4;
        }  
}

// ========================================   sauna control ============================================  
void sauna_control(void)
 {
          // status = 2 topi, bylo zapnuto HW
          // status = 1 topi, bylo zapnuto SW
          // status = 0 vypnuto
          // status = -1 vypnuto SW

//     Serial.println("Pin  "+String(pin_IN_sauna_button)+" status is  "+String(digitalRead(pin_IN_sauna_button))+" sauna status is  "+String(sauna_status)+" " +sauna_status_text+"Pin 49  "+String(digitalRead(49)) );
    if (!digitalRead(pin_IN_sauna_button))   //  vypinac na saune zapnuty
            {if (sauna_status > -1 ) {sauna_status=2; sauna_temp_required = sauna_temp_required_const;}; }  // pokud bylo SW vypnuto tak uz nezapinat
    else                                     //  vypinac na saune vypnuty
            {if (sauna_status == 2 or sauna_status == -1) {sauna_status=0; sauna_temp_required = 0;};};                          // pokud bylo HW zapnuto tak lze i HW vypnout

    unsigned long currentMillis_sauna         = millis();

    if ((millis() - millis_sauna_safety_begin) >= interval_sauna_safety) {   // bezpecnostni vypnuti
      sauna_status=0; 
      sauna_temp_required = 0;
    }

    if (currentMillis_sauna - previousMillis_sauna >= interval_sauna) {   // kontrola uplynuti 1 sec
    previousMillis_sauna = currentMillis_sauna;

  if (sauna_status == 0 or sauna_status == -1) 
    {sauna_status_text = "idle";
     digitalWrite(pin_OUT_rele_sauna, LOW); 
     digitalWrite(pin_OUT_sauna_LED_OK,  LOW);   // LED indikace vypnuto
     digitalWrite(pin_OUT_sauna_LED_low,  LOW);   
    }
    else if (digitalRead(pin_IN_HDO) == 1 and sauna_HDO_used == 1) // je tarif HIGH a zaroven je pozadovano rizeni tarifem
    {sauna_status_text = "HDO-high";
     digitalWrite(pin_OUT_rele_sauna, LOW); 
     digitalWrite(pin_OUT_sauna_LED_OK,  LOW);   
     digitalWrite(pin_OUT_sauna_LED_low,  HIGH);   
     }
    else if (sauna_temp_actual < 5 or sauna_temp_actual > 110 ) // teplota merena senzorem je nesmyslna
    {sauna_status_text = "wierd_temp";
     digitalWrite(pin_OUT_rele_sauna, LOW); 
     digitalWrite(pin_OUT_sauna_LED_low,  LOW);   
     digitalWrite(pin_OUT_sauna_LED_OK,  ((currentMillis_sauna/1000)%2));   // indikace poruchu - blika zelena LED
    }
    
    else if (sauna_temp_actual < sauna_temp_required) // teplota je mensi nez pozadovana tak zapni topeni
    {sauna_status_text = "heating-on";
     digitalWrite(pin_OUT_rele_sauna, HIGH); 
     digitalWrite(pin_OUT_sauna_LED_OK,  LOW);   // LED indikace teplota nizka
     digitalWrite(pin_OUT_sauna_LED_low,  ((currentMillis_sauna/1000)%2));   // indikace topeni zapnuto - blika cervena LED
    }
    else 
    {sauna_status_text = "heating-off";
     digitalWrite(pin_OUT_rele_sauna, LOW); 
     digitalWrite(pin_OUT_sauna_LED_OK,  HIGH);   // LED indikace teplota OK
     digitalWrite(pin_OUT_sauna_LED_low,  LOW);   
    }
  }  
}

// ========================================   heating 2NP control ============================================  
void heating_2NP_control(void)
 {

    unsigned long currentMillis_heating_2NP = millis();
    if (currentMillis_heating_2NP - previousMillis_heating_2NP >= interval_heating_2NP) {   // kontrola uplynuti 1 sec
    previousMillis_heating_2NP = currentMillis_heating_2NP;

  
  if (heating_2NP_status == 0) 
    { heating_2NP_status_text = "idle";
      digitalWrite(pin_OUT_rele_heating_2NP, LOW); 
    }

    else if ( digitalRead(pin_IN_HDO)) // neni vysoky tarif
    { heating_2NP_status_text = "HDO-high";
      digitalWrite(pin_OUT_rele_heating_2NP, LOW); 
    }

    else if ( digitalRead(pin_IN_Motor_ATREA)) // motor se musi tocit - pin_IN je invertovany -> 0 = motor se toci
    { heating_2NP_status_text = "motor-off";
      digitalWrite(pin_OUT_rele_heating_2NP, LOW); 
    }

    else if ((heating_2NP_temp_actual < heating_2NP_temp_required) ) // teplota je mensi nez pozadovana tak zapni topeni 
    { heating_2NP_status_text = "heating-on";
      digitalWrite(pin_OUT_rele_heating_2NP, HIGH); 
    }
      else 
    { 
      heating_2NP_status_text = "heating-off";
      digitalWrite(pin_OUT_rele_heating_2NP, LOW); 
    }
 }  
 }
// ========================================  config page ============================================  
void show_config_HTTP()
  {
    client.print("-------------------------------------------------------------\n");
    client.print("|           Configuration    for Arduino                     |\n");
    client.print("-------------------------------------------------------------\n");
    client.println("version is "+current_version_text);
    client.println("Configuration for device DS2438:");
    client.println("Humidity on pin "+String(pin_DS2438_humi)+" has HW address "+HW_adresses_DS2438_humi+   ", is error? "+String(ds2438_humi.isError()));
    client.println("Tank     on pin "+String(pin_DS2438_tank)+" has HW address "+HW_adresses_DS2438_tank+   ", is error? "+String(ds2438_tank.isError()));
    client.println("Tank2    on pin "+String(pin_DS2438_tank2)+" has HW address "+HW_adresses_DS2438_tank2+ ", is error? "+String(ds2438_tank2.isError()));
    client.println();

    String IPADR = String(Ethernet.localIP()[0]) + "." +  String(Ethernet.localIP()[1]) + "." + String(Ethernet.localIP()[2]) + "." + String(Ethernet.localIP()[3]);   
    client.println("IP address:  "+IPADR);
    client.println();
    
    client.println("BUS 1 - 3wire with counters  on pin "+String(ds18b20_BUS1)+" has "+String(DS18B20_count_BUS1)+" device(s)");
    for (int i = 0; i < DS18B20_count_BUS1; i++){
      client.println(HW_adresses_BUS1[i]);
    }
 
    client.println("BUS 2 - 3wire                on pin "+String(ds18b20_BUS2)+" has "+String(DS18B20_count_BUS2)+" device(s)");
    for (int i = 0; i < DS18B20_count_BUS2; i++){
      client.println(HW_adresses_BUS2[i]);
    }

    client.println("BUS 3 - 2wire                on pin "+String(ds18b20_BUS3)+" has "+String(DS18B20_count_BUS3)+" device(s)");
    for (int i = 0; i < DS18B20_count_BUS3; i++){
      client.println(HW_adresses_BUS3[i]);
    }
  
    client.println("BUS 4 - 2wire atrea c2       on pin "+String(ds18b20_BUS4)+" has "+String(DS18B20_count_BUS4)+" device(s)");
    for (int i = 0; i < DS18B20_count_BUS4; i++){
      client.println(HW_adresses_BUS4[i]);
    }
  }

// ========================================   reset  ============================================  

void softReset(){
      client.println("reseting........  ");
      asm volatile ("  jmp 0");
}

// ========================================  manual page ============================================  
  void show_help()
  {
    Serial.print("-------------------------------------------------------------\n");
    Serial.print("|           List of commands for Arduino                     |\n");
    Serial.print("-------------------------------------------------------------\n");
    Serial.print("saunaOn xx     turn on sauna and set required temperature xxC\n");
    Serial.print("saunaOff       turn off sauna \n");
    Serial.print("HDO on/off     sauna is/isn't controled by HDO \n");
    Serial.print("pulse xx       pulse 500 ms at pin xx \n");
    Serial.print("switchOn xx    set pin xx to HIGH \n");
    Serial.print("switchOff xx   set pin xx to LOW \n");
    Serial.print("get            get operational values \n");
    Serial.print("man            show this manual \n ");
  }
// ========================================  manual page ============================================  
void show_help_HTTP()
  {
    client.print("---------------------------------------------------------------------\n");
    client.print("|              List of commands for Arduino                          |\n");
    client.print("---------------------------------------------------------------------\n");
    client.print("set_sauna%xx^xx     turn on/off sauna and is/isn't controled by HDO\n");
    client.print("saunaOn xx          turn on sauna and set required temperature xxC\n");
    client.print("saunaOff            turn off sauna \n");
    client.print("sauna_temp%xx       set sauna temperature \n");
    client.print("HDO%on/off          sauna is/isn't controled by HDO \n");
    client.print("heating2NP_On%xx    turn on heating 2NP and set required temperature xxC\n");
    client.print("heating2NP_Off      turn off heating 2NP \n");
    client.print("pulse%xx            pulse 1000 ms at pin xx \n");
    client.print("switchOn%xx         set pin xx to HIGH \n");
    client.print("switchOff%xx        set pin xx to LOW \n");
    client.print("get_2438            get DS2438 operational values \n");    
    client.print("get                 get operational values \n");    
    client.print("config              get configuration \n");    
    client.print("reset               reset the board \n");    
    client.print("readPin%xx          read status of pin xx \n");
    client.print("postAck             post acknowledge (ack from mobile aplication) \n");
    client.print("postAckEmail        post acknowledge email 2 (confirmation email was send)\n");
    client.print("man                 show this manual \n ");
  }

// ========================================   read commands from HTTP  ============================================  
void readHTTPline()
{
  // listen for incoming clients
  client = server.available();
  if (client) 
  {
    // send http reponse header
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    // process request.
    processClient(client);
  }
}

// ========================================   process commands from HTTP  ============================================  

void processClient(EthernetClient client)
{
  // http request will end with a blank line
  boolean lineIsBlank = true;
 
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
 
      if(reading && c == ' ') reading = false;
      if(c == '?') reading = true; // ? in GET request was found, start reading the info
 
      //check that we are reading, and ignore the '?' in the URL, then append the get parameter into a single string
      if(reading && c != '?') HTTPget += c;
 
      if (c == '\n' && lineIsBlank)  break;
 
      if (c == '\n')
      {
        lineIsBlank = true;
      }
      else if (c != '\r')
      {
        lineIsBlank = false;
      }
     }
  }

  int divider_position  = HTTPget.indexOf('%');
  int divider_position2 = HTTPget.indexOf('^');
  
  String part1=HTTPget.substring(0,divider_position);
  String part2=HTTPget.substring(divider_position+1,divider_position2);
  String part3=HTTPget.substring(divider_position2+1);

    if (part1.equalsIgnoreCase("set_sauna"))
      {
          if (part3 == "on")    {sauna_HDO_used = 1;}
          if (part3 == "off")   {sauna_HDO_used = 0;}

          if (part2 == "on") {  
            sauna_temp_required = sauna_temp_required_const;
            sauna_status = 1;
            sauna_status_text = "heating-on";
            millis_sauna_safety_begin = millis();
            if (digitalRead(pin_IN_HDO) == 1 and sauna_HDO_used == 1) { // je tarif HIGH a zaroven je pozadovano rizeni tarifem
            sauna_status_text = "HDO-high";
            }
          }
          else {
            sauna_temp_required = 0;
            if (sauna_status == 2)   {sauna_status = -1;}
            else                     {sauna_status =  0;}  
            sauna_status_text = "idle";          
          }
                      
          sent_data_to_HTTP();
       }

          // status = 2 topi,
          // status = 0 vypnuto
          // status = -1 vypnuto SW

    if (part1.equalsIgnoreCase("saunaOn"))
      {
          sauna_temp_required = part2.toInt();
          sauna_status = 1;
          millis_sauna_safety_begin = millis();
          sent_data_to_HTTP();
       }
       
    else if (part1.equalsIgnoreCase("saunaOff"))
      {
         sauna_temp_required = 0;
          if (sauna_status == 2)   {sauna_status = -1;}
          else                     {sauna_status =  0;}
          sent_data_to_HTTP();
      }  
                       
    else if (part1.equalsIgnoreCase("HDO"))
      {
          if (part2 == "off") 
            {sauna_HDO_used = 0;}
          else
            {sauna_HDO_used = 1;}
          client.print("HDO je ");
          client.println(sauna_HDO_used);        }

    else if (part1.equalsIgnoreCase("sauna_temp"))
      {
          sauna_temp_required = part1.toInt();
          sent_data_to_HTTP();        
          }
        
    else if (part1.equalsIgnoreCase("pulse"))
      {          
        int device_id = part2.toInt();
        pinMode(device_id, OUTPUT);
        digitalWrite(device_id, HIGH);
        delay(1000); 
        digitalWrite(device_id, LOW); 
        sent_data_to_HTTP();
        }

    else if (part1.equalsIgnoreCase("heating2NP_On"))
      {   
        heating_2NP_temp_required = part2.toInt();
        heating_2NP_status = 1;
        sent_data_to_HTTP();
        }
        
    else if (part1.equalsIgnoreCase("heating2NP_Off"))
      {   
        heating_2NP_status = 0;
        sent_data_to_HTTP();
        }
        
    else if (part1.equalsIgnoreCase("switchOn"))
      {   
        int device_id = part2.toInt();
        pinMode(device_id, OUTPUT);
        digitalWrite(device_id, HIGH); 
        sent_data_to_HTTP();
        }
        
    else if (part1.equalsIgnoreCase("switchOff"))
      {   
        int device_id = part2.toInt();
        pinMode(device_id, OUTPUT);
        digitalWrite(device_id, LOW); 
        sent_data_to_HTTP();
        }
        
    else if (part1.equalsIgnoreCase("get"))
      {
        sent_data_to_HTTP();
        } 

    else if (part1.equalsIgnoreCase("get_2438"))
      {
        sent_2438_data_to_HTTP();
        } 
        
    else if (part1.equalsIgnoreCase("reset"))
      {
        softReset();
        } 

    else if (part1.equalsIgnoreCase("config"))
      {
        show_config_HTTP();
        } 
           
    else if (part1.equalsIgnoreCase("readPin"))
      {
        int device_id = part2.toInt();
          pinMode(device_id, INPUT);
          client.println(digitalRead(device_id));        
          }

    else if (part1.equalsIgnoreCase("postAck"))
      {
        status_post     = 0;
        time_post     = 0;
        sent_data_to_HTTP();
      }

    else if (part1.equalsIgnoreCase("postAckEmail"))
      {
        status_post_2     = 0;
        client.println(String(status_post_2));        
      }

   else if (part1.equalsIgnoreCase("man"))
      {
        show_help_HTTP();
      }   
      else
      {};
   /* else if (part1.equalsIgnoreCase(""))
      {
       Serial.println();
      }
    
    else {
        Serial.println("haha recognized command: "+part1);
 */   

  delay(1); // give the web browser a moment to recieve
  client.stop(); // close connection
  HTTPget = ""; // clear out the get param we saved
}

// ========================================   sent data to USB ============================================  
void sent_data_to_USB(void)
{
  for (int i = 0; i < DS18B20_count_BUS1; i++){  Serial.print(HW_adresses_BUS1[i]+":"+String(sensors_18B20_BUS1.getTempCByIndex(i))+"\n");  }    
  for (int i = 0; i < DS18B20_count_BUS2; i++){  Serial.print(HW_adresses_BUS2[i]+":"+String(sensors_18B20_BUS2.getTempCByIndex(i))+"\n");  }    
  for (int i = 0; i < DS18B20_count_BUS3; i++){  Serial.print(HW_adresses_BUS3[i]+":"+String(sensors_18B20_BUS3.getTempCByIndex(i))+"\n");  }    
  for (int i = 0; i < DS18B20_count_BUS4; i++){  Serial.print(HW_adresses_BUS4[i]+":"+String(sensors_18B20_BUS4.getTempCByIndex(i))+"\n");  }    

// uint16_t lux = LightSensor.GetLightIntensity();// Get Lux value
 
  Serial.print("BH1750_svit:"       +String(LightSensor.GetLightIntensity())+"\n");
  Serial.print("counter_2_post:"    +String(counter1)+"\n");
  Serial.print("counter_3_ATREA:"   +String(counter2)+"\n");       
  Serial.print("counter_18_boiler:" +String(counter3)+"\n");     
  Serial.print("counter_19_kotel:"  +String(counter4)+"\n");      
  Serial.print("sauna:"+sauna_status_text+"\n");          
  Serial.print("sauna_temp_required:"+String(sauna_temp_required)+"\n");  
  Serial.print("sauna_temp_actual:"  +String(sauna_temp_actual)+"\n");     
  Serial.print("info_kotel:"+String(digitalRead(pin_IN_kotel))+"\n");            
  Serial.print("info_HDO:"+String(digitalRead(pin_IN_HDO))+"\n");             
  Serial.print("info_garage:"+String(digitalRead(pin_IN_Garage_door_open))+"\n");           
    for (int i = pin_OUT_rele_min; i <= pin_OUT_rele_max; i++)
  {
  Serial.print("rele"+String(i)+":"+String(digitalRead(i))+"\n");           
  } 
  Serial.print("uptime:"+String((long)millis()/1000)+"\n");  
  Serial.print((char)4); // EOF character
}


// ========================================   read commands from USB  ============================================  

    void readcommandline()
    {
    if(Serial.available())                
    {
      char c = Serial.read();
      if (c == 'x' or c == '\n')
      {
        parseCommand(command);
        command = "";
      }
      else 
      {
        command+=c;
        }
    }
    }

// ========================================   parseCommand ============================================  
  void parseCommand(String com)
  {
    String part1;
    String part2;

    com.trim();
    part1 = com.substring(0,com.indexOf(" "));
    part2 = com.substring(com.indexOf(" ")+1,com.length());

    if (part1.equalsIgnoreCase("saunaOn"))
      {
          sauna_temp_required = part2.toInt();
          sauna_status = 1;
          Serial.print("sauna zapnuta a teplota je ");
          Serial.println(sauna_temp_required);
       }
       
    else if (part1.equalsIgnoreCase("saunaOff"))
      {
          sauna_temp_required = 0;
          sauna_status = 0; 
          Serial.print("sauna vypnuta a teplota je ");
          Serial.println(sauna_temp_required);        }
        
    else if (part1.equalsIgnoreCase("HDO"))
      {
          if (part2 == "off") 
            {sauna_HDO_used = 0;}
          else
            {sauna_HDO_used = 1;}
          Serial.print("HDO je ");
          Serial.println(sauna_HDO_used);        }
        
    else if (part1.equalsIgnoreCase("pulse"))
      {          
        int device_id = part2.toInt();
        pinMode(device_id, OUTPUT);
        digitalWrite(device_id, HIGH);
        delay(500); 
        digitalWrite(device_id, LOW); 
        Serial.println(device_id);
        }
        
    else if (part1.equalsIgnoreCase("switchOn"))
      {   
        int device_id = part2.toInt();
          pinMode(device_id, OUTPUT);
          digitalWrite(device_id, HIGH); 
          Serial.print("rele"+String(device_id)+":"+String(digitalRead(device_id))+"\n");           
        }
        
    else if (part1.equalsIgnoreCase("switchOff"))
      {   
        int device_id = part2.toInt();
          pinMode(device_id, OUTPUT);
          digitalWrite(device_id, LOW); 
          Serial.print("rele"+String(device_id)+":"+String(digitalRead(device_id))+"\n");           
        }
        
    else if (part1.equalsIgnoreCase("get"))
      {
        sent_data_to_USB();
        }

   else if (part1.equalsIgnoreCase("man"))
      {
        show_help();
      }   
    else if (part1.equalsIgnoreCase("switchOff"))
      {   
        int device_id = part2.toInt();
          pinMode(device_id, OUTPUT);
          digitalWrite(device_id, LOW); 
          Serial.println(device_id);
        }
        
      else
      {};
   /* else if (part1.equalsIgnoreCase(""))
      {
       Serial.println();
      }
    
    else {
        Serial.println("haha recognized command: "+part1);
 */  
}


void setup(void)
{
  Serial.begin(9600);
  Serial.setTimeout(100);
  LightSensor.begin(); // BH1750
  //LightSensor.SetAddress(Device_Address_L); //Address 0x5C
//  LightSensor.SetMode(Continuous_H_resolution_Mode);
      
  init_interupt();
  init_DS1820();
  init_DS2348();
  read_sensor_data_DS18B20;
        
  pinMode(53, OUTPUT); //On the Mega, the hardware SS pin, 53, is not used to select either the W5100 or the SD card, 
  //but it must be kept as an output or the SPI interface won't work.  pin 10, 11, 12 and 13 are used by the ethernet shield

  for (int device_id = pin_OUT_rele_min; device_id <= pin_OUT_rele_max; device_id++)  // set all relays to LOW
  {
        pinMode(device_id, OUTPUT);
        digitalWrite(device_id, LOW); 
  } 
  
    Ethernet.begin(mac, ip, gateway, subnet); // setup ethernet with params from above
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      }
    
    server.begin();

    String IPADRESS = String(Ethernet.localIP()[0]) + "." +  String(Ethernet.localIP()[1]) + "." + String(Ethernet.localIP()[2]) + "." + String(Ethernet.localIP()[3]);   
    Serial.println("IP address:  "+IPADRESS+"\n");

  pinMode(pin_OUT_rele_sauna, OUTPUT);
  pinMode(pin_OUT_sauna_LED_OK, OUTPUT);                // set pin to output
  pinMode(pin_OUT_sauna_LED_low, OUTPUT);               // set pin to output
  pinMode(pin_IN_sauna_button, INPUT);                  // set pin to input
  digitalWrite(pin_IN_sauna_button, HIGH);              // turn on pullup resistors 

  pinMode(pin_OUT_Garage_LED, OUTPUT);                // set pin to output
  
  pinMode(pin_IN_Garage_door_open, INPUT);            // set pin to input
  digitalWrite(pin_IN_Garage_door_open, HIGH);        // turn on pullup resistors  

  pinMode(pin_IN_Motor_ATREA, INPUT);                 // set pin to input
  digitalWrite(pin_IN_Motor_ATREA, HIGH);             // turn on pullup resistors  
  
  pinMode(pin_OUT_HDO, OUTPUT);                       // set pin to output
  
  pinMode(pin_IN_HDO, INPUT);                         // set pin to input
  digitalWrite(pin_IN_HDO, HIGH);                     // turn on pullup resistors

  pinMode(pin_IN_kotel, INPUT);                       // set pin to input
  digitalWrite(pin_IN_kotel, HIGH);                   // turn on pullup resistors

  pinMode(pin_IN_Armed_house, INPUT);                 // set pin to input
  digitalWrite(pin_IN_Armed_house, HIGH);             // turn on pullup resistors
  }

void loop(void)
{ 
    digitalWrite(pin_OUT_Garage_LED,  digitalRead(pin_IN_Garage_door_open));   // LED indikace otevrenych vrat garaze
    digitalWrite(pin_OUT_HDO,         digitalRead(pin_IN_HDO));                // indikace HDO sepnutim rele 
    digitalWrite(pin_OUT_Motor_ATREA, !digitalRead(pin_IN_Motor_ATREA));        // indikace motor ATREA sepnutim rele 

    sauna_control();                // rizeni sauny ve smycce  kazdou 1 sec 
    heating_2NP_control();          // rizeni heating 2NP ve smycce  kazdou 1 sec 
    read_sensor_data_DS18B20();     // nacteni DS1820 kazdych  30 sec
    read_sensor_data_DS2438();      // nacteni DS2438 kazdych  1 min
    checking_counters();            // kontrola counteru a nastaveni statusu 1 min
    readcommandline();
    readHTTPline();
}
