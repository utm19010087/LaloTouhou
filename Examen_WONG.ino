#include <ESP8266WiFi.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_MQTT_Client.h>
#include <SimpleDHT.h>

#define SSID "RED"
#define PASSWD "12345678"

#define IO_USERNAME  "UTM19010087"
#define IO_KEY "aio_Kmvb67Hy41xvWLyPpq7f0flkscJj"

int pinDHT = D1;
int err;

byte temperature = 0;
byte humidity = 0;

AdafruitIO_WiFi ioFeed(IO_USERNAME, IO_KEY, SSID, PASSWD);
AdafruitIO_Feed *temperatureFeed = ioFeed.feed("temperature");
AdafruitIO_Feed *humidityFeed = ioFeed.feed("humidity");
AdafruitIO_Feed *ledFeed = ioFeed.feed("led");

SimpleDHT11 dht(pinDHT);

void setup()
{
  Serial.begin(9600);
  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);
  ioFeed.connect();
  ledFeed -> onMessage(ledController);
  while(ioFeed.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  ledFeed -> get();
}

void loop()
{
  ioFeed.run();
  DHTController();
  Serial.println("*");
  delay(5000);
}

void DHTController(){
  temperature = 0;
  humidity = 0;
  err = SimpleDHTErrSuccess;
  if((err = dht.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT 11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  sendTemperature((int) temperature);
  sendHumidity((int) humidity);
}

void sendTemperature(int temperature)
{
  temperatureFeed -> save(temperature);
  Serial.print((int) temperature); Serial.print(" *C, "); 
}

void sendHumidity(int humidity)
{
  humidityFeed -> save(humidity);
  Serial.print((int) humidity); Serial.println(" H");
}

void ledController(AdafruitIO_Data *ledInfo) {
  digitalWrite(D2, !ledInfo->toPinLevel());
}