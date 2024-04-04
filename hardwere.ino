#include <ESP8266WiFi.h>
#include <DHT.h>
#include <U8g2lib.h>


const char* ssid = "ESP8266_AP"; // Change this to the desired AP name
const char* password = "urpass"; // Change this to the desired password

#define DHTPIN 0 // D4 - Connect DHT sensor to this pin
#define DHTTYPE DHT11



#define SOIL_PIN A0 // Connect soil moisture sensor to this pin
#define RELAY_PIN_1 14 // D5 GPIO pin connected to relay 1
#define RELAY_PIN_2 12 // D6 GPIO pin connected to relay 2
#define LDR_PIN 13 // D7 Connect LDR to this pin

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D1, /* data=*/ D2, /* reset=*/ U8X8_PIN_NONE); // OLED constructor

void setup() {
  Serial.begin(115200);
  delay(10);

    u8g2.begin(); // Initialize the OLED display
    


  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  digitalWrite(RELAY_PIN_1, HIGH); // Relay 1 off
  digitalWrite(RELAY_PIN_2, HIGH); // Relay 2 off

 

  // Start access point
  WiFi.softAP(ssid, password);
  
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  Serial.println("Server started");

  dht.begin();
  // Display funtions 
  welcome();
  delay(5000);
 displaySSID_Pass_IP();
 
}

void loop() {


 static int thresholdValue;
 static int N;
 int ldrValue = digitalRead(LDR_PIN);
char ldr[4]; // Declare a character array to hold "YES" or "NO"

//If the LDR reading is HIGH, consider it bright and display "Yes"
if (ldrValue == LOW) {
  strcpy(ldr, "YES"); // Copy "YES" into the ldr array
} 
// If the LDR reading is LOW, consider it dark and display "No"
else {
  strcpy(ldr, "NO"); // Copy "NO" into the ldr array
} 

   delay(2000);

  

 

  WiFiClient client = server.available();
  if (!client) {
    return;
  }

 

  while (!client.available()) {
    delay(1);
  }
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  float raw_value = analogRead(SOIL_PIN);
  float output_value = map(raw_value, 1024, 1, 0, 100);

/*
  Serial.print("Raw Soil Moisture Value: ");
  Serial.println(raw_value);

  // You can add a delay here to control how often you log the value
  delay(1000); 
*/

  String request = client.readStringUntil('\r');
  client.flush();


if (request.indexOf("threshold") != -1) {
        // Extract threshold value from the request
        int startIndex = request.indexOf("threshold=") + 10;
        int endIndex = request.indexOf("&", startIndex);
        if (endIndex == -1) {
            endIndex = request.length();
        }
        String thresholdStr = request.substring(startIndex, endIndex);
        thresholdValue = thresholdStr.toInt();
        Serial.print("Threshold value extracted: ");
        Serial.println(thresholdValue); // Print the extracted threshold value for debugging
        processThreshold(thresholdValue, &N);
    }

    Serial.println(thresholdValue); // Print the extracted threshold value for debugging
            Serial.println("n1="); // Print the extracted threshold value for debugging

        Serial.println(N); // Print the extracted threshold value for debugging





if (request.indexOf("/?relay1=on") != -1) {
    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, LOW);

    delay(N);
     Serial.println("N="); // Print the extracted threshold value for debuggin
    Serial.println(N); // Print the extracted threshold value for debugging
    digitalWrite(RELAY_PIN_1, HIGH);
    digitalWrite(RELAY_PIN_2, HIGH);

}

if (request.indexOf("/?relay2=on") != -1) {
  delay(5000);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_1, LOW);
    delay(N);
    digitalWrite(RELAY_PIN_2, HIGH);
    digitalWrite(RELAY_PIN_1, HIGH);

}


 

String html = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Smart Irrigation Website</title> <style> body { margin: 0; padding: 0; font-family: 'Roboto', sans-serif; background: linear-gradient(120deg, #D8B5FF, #1EAE98); /* Adjust gradient colors as desired */ } .header { background-color: rgba(0, 0, 0, 0.289); /* Semi-transparent black background for better readability */ color: white; text-align: center; padding: 20px 0; width: 100%; box-sizing: border-box; } .container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; margin-top: 20px; position: relative; } .box { position: relative; width: 200px; height: 150px; background-color: white; /* White background color for the divs */ border: 2px solid #ddd; margin: 10px; display: flex; align-items: center; justify-content: center; border-radius: 10px; box-shadow: 0px 10px 20px rgba(0, 0, 0, 0.3); /* Adding a shadow to create a 3D effect */ transform: translateY(-10px); /* Move the box up to create a floating effect */ font-weight: bold; text-align: center; } .label { font-size: 18px; font-weight: bold; margin-bottom: 10px; text-align: center; color: #333; /* Dark gray text color */ } .text { font-size: 25px; font-weight: bold; color: #333; /* Dark gray text color */ } .btn-container { text-align: center; } .btn { padding: 10px 20px; background-color: #78fc47; /* Red button */ color: white; border: none; cursor: pointer; font-size: 16px; font-weight: bold; border-radius: 5px; transition: background-color 0.3s, color 0.3s; border: 1px solid black; } .btn:hover { background-color: rgb(0, 176, 9); /* Darker red on hover */ } #airt { background-color: #29bf12; } #airh { background-color: #9ef01a; } #soilm { background-color: #29bf12; } #additional { background-color: #9ef01a; color: #333; /* Dark gray text color */ } #water-dosage { background-color: #edede9; /* Light gray */ } #fertilizer-dosage { background-color: #edede9; /* Light gray */ } .manual-text { font-size: 24px; margin: 20px 0; text-align: center; color: white; /* White text color */ } .btns { padding: 5px 10px; background-color: #78fc47; /* Red button */ color: white; border: none; cursor: pointer; font-size: 16px; font-weight: bold; border-radius: 5px; transition: background-color 0.3s, color 0.3s; margin-top: 10px; border: 1px solid black; } .btns:hover { background-color: rgb(0, 176, 9); /* Darker red on hover */ } /* Custom style for input box */ input[type=\"number\"] { width: 100%; padding: 8px; margin-top: 4px; box-sizing: border-box; border: 1px solid #666; /* Darker border color */ border-radius: 4px; outline: none; } #mn-text { color:#333; text-decoration: underline; } </style>    </head> <body> <div class=\"header\"> <h1>Smart Irrigation</h1> </div> <div class=\"container\"> <div> <div class=\"label\">Air Temperature</div> <div class=\"box\" id=\"airt\"><div class=\"text\">29C</div></div> </div> <div> <div class=\"label\">Air Humidity</div> <div class=\"box\" id=\"airh\"><div class=\"text\">42%</div></div> </div> <div> <div> <div class=\"label\">Soil Moisture</div> <div class=\"box\" id=\"soilm\"><div class=\"text\">" + String(output_value) + "</div></div> </div> <div> <div class=\"label\">Sunlight present in the vicinity</div> <div class=\"box\" id=\"additional\"><div class=\"text\">" + String(ldr) + "</div></div> </div> </div> <h1 class=\"manual-text\">Manual Controls</h1> <div class=\"container\"> <div class=\"box\"> <form action=\"/\" method=\"GET\"> <label for=\"threshold\">Dosage Quantity(LTR):</label> <input type=\"number\" id=\"threshold\" name=\"threshold\" min=\"0\" max=\"100\"> <input type=\"submit\" value=\"Set Threshold\"> </form> </div> </div> <div class=\"container\"> <div class=\"box\"> <div class=\"btn-container\"> <a href=\"/?relay1=on\" class=\"btn\">Water Dosage</a> </div> </div> <div class=\"box\"> <div class=\"btn-container\"> <a href=\"/?relay2=on\" class=\"btn\">Fertilizer Dosage</a> </div> </div> </div> </body> </html>";

  client.print(html);

  delay(2000);
}

void displaySSID_Pass_IP() {

  u8g2.clearBuffer(); // Clear the display buffer
  u8g2.setFont(u8g2_font_ncenB14_tr); // Choose a font
  u8g2.setCursor(0, 20); // Set cursor position
  u8g2.print("SSID"); // Print text
  u8g2.setCursor(0, 40); // Set cursor position
  u8g2.print(ssid); // Print IP address
  u8g2.sendBuffer(); // Send the buffer to the display

  delay(5000);

  
  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_ncenB14_tr); 
  u8g2.setCursor(0, 20); 
  u8g2.print("Password"); 
  u8g2.setCursor(0, 40); 
  u8g2.print(password); 
  u8g2.sendBuffer(); 

  delay(5000);


  u8g2.clearBuffer(); // Clear the display buffer
  u8g2.setFont(u8g2_font_ncenB14_tr); // Choose a font
  u8g2.setCursor(0, 20); // Set cursor position
  u8g2.print("IP Address:"); // Print text
  u8g2.setCursor(0, 40); // Set cursor position
  u8g2.print(WiFi.softAPIP()); // Print IP address
  u8g2.sendBuffer(); // Send the buffer to the display
  delay(10000);
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}



void welcome() {
  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_ncenB14_tr); // Choose a font
  u8g2.setCursor(0, 20); 
  u8g2.drawStr(0, 10, "Hi Sakura");
  u8g2.drawStr(0, 30, "Bio Tech");
  u8g2.setCursor(0, 40); 
  u8g2.sendBuffer(); 
  delay(5000); // Delay for 5 seconds
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Welcome to");
  u8g2.drawStr(0, 30, "Smart irrgation");
  u8g2.sendBuffer(); 
}
void processThreshold(int thresholdValue, int *N) {
    *N = thresholdValue * 1000;
}

/* 
3.3V = DHT11,OLED,SOIL,LIGHT
VIN/5V = RELAY
Soil = A0
Light SENSOR= D5
DHT11 SENSOR =D6
Relay1=D3
Relay2=DD4
*/








// KUNAL'S CODE AHAHAH
