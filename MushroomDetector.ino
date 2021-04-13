#include <NewPing.h>
#include <TridentTD_LineNotify.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define SONAR_NUM 5
#define MAX_DISTANCE 100
#define DHTPIN D8
#define DHTTYPE DHT22

NewPing sonar[SONAR_NUM] = {       // Sensor object array.
  NewPing(D1, D1, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(D2, D2, MAX_DISTANCE),
  NewPing(D5, D5, MAX_DISTANCE),
  NewPing(D6, D6, MAX_DISTANCE),
  NewPing(D7, D7, MAX_DISTANCE)
};

int ul, TotalGrownMushroom, LastTotalGrownMushroom;
int freq = 1000;
int ref = 10;
float Humid, Temp;

char auth[] = "API_KEY"; //Blynk api key
char ssid[] = "SSID"; //ชื่อ wifi
char pass[] = "PASS"; //Wifi password
char IP[] = "oasiskit.com";

BLYNK_WRITE(V1)
{
  ref = param.asInt();
}
BLYNK_WRITE(V2)
{
  freq = param.asInt();
}

DHT dht(DHTPIN, DHTTYPE);

void IOTbegin() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println(WiFi.localIP());
  LINE.setToken("LINE_TOKEN");//Line token
  Blynk.begin(auth, ssid, pass, IP, 8080);
}
void CheckMushroom() {
  TotalGrownMushroom = 0;
  for (int i = 0; i < SONAR_NUM; i++)
  {
    ul = sonar[i].ping_cm();
    Serial.print(i);
    Serial.print("=");
    Serial.print(ul);
    Serial.print("cm ");

    if (ul < ref)
    {
      TotalGrownMushroom++;
    }
  }
  Serial.println();
  if (TotalGrownMushroom != LastTotalGrownMushroom)
  {
    Serial.print("GrownMushroom =");
    Serial.println(TotalGrownMushroom);
    LINE.notify(TotalGrownMushroom);
    Blynk.virtualWrite(V0, TotalGrownMushroom);
    LastTotalGrownMushroom = TotalGrownMushroom;
  }
}
void TestDHT () {
  Serial.print(F("Humidity: "));
  Serial.print(Humid);
  Serial.print(F("%  Temperature: "));
  Serial.print(Temp);
  Serial.println(F("°C "));
}

void setup()
{
  Serial.begin(115200);
  IOTbegin();
  dht.begin();
}

void loop()
{
  Blynk.run();
  Humid = dht.readHumidity();
  Temp = dht.readTemperature();
  CheckMushroom();
  Blynk.virtualWrite(V3, Temp);
  Blynk.virtualWrite(V4, Humid);
  delay(freq);
}
