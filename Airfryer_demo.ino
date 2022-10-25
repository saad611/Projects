// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#define PERIOD_MS 1000000
#define TEXT_SIZE 1

byte AInput = 33;
byte BInput = 32;
byte ButtonInput = 35;
byte Buzzer = 25;
byte Fan = 12;
byte Heat = 14;

byte lastState = 0;
int  cw = 0;
byte AState = 0;
byte BState = 0;
byte State = 0;

volatile int minutes = 0;
volatile int seconds = 0;
char screen[10];
volatile bool TimerActive = false;
volatile byte machinestate = 0;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounter = 0;
int numberOfInterrupts = 0;
hw_timer_t * timer = NULL;

byte A_temp = 26;
byte B_temp = 27;
byte set_temp = 22;

int  cw_temp = 0;
byte AState_temp = 0;
byte BState_temp = 0;
byte lastState_temp = 0;
volatile int temperature_ = 0;
int programmed_temp =0;

// Replace with your network credentials
const char* ssid = "WLAN-536119";
const char* password = "17748575802450706113";

/* Declare LCD object for SPI
 Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
Adafruit_PCD8544 display = Adafruit_PCD8544(18, 23, 4, 15, 2);
int contrastValue = 60; // Default Contrast Value

// Measure Temperature using NTC Thermistor
// 0- General
 
int decimalPrecision = 2;
 
// 1- Temperature Measurement
 
float voltageDividerR_1 = 10000;
float BValue = 3470;
float R_1 = 5000;
float T_1 = 298.15;
float R_2 ;
float T_2 ;
 
float a ;
float b ;
float c ;
float d ;
float e = 2.718281828 ;
 
float ntc_val = 0;
float tempLastSample = 0;
float tempSampleSum = 0;
float tempSampleCount = 0;
float tempMean ;
 

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int ntc_pin = 34;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String Temperature()
{
 // 1- Temperature Measurement
 
    if(millis() >= tempLastSample + 1)
    {
     ntc_val = analogRead(ntc_pin);
     tempSampleSum = tempSampleSum+ntc_val;
     tempSampleCount = tempSampleCount+1;
     tempLastSample = millis();
    }
 
    if(tempSampleCount == 1000)
    {
     tempMean = tempSampleSum / tempSampleCount;
     R_2 = (voltageDividerR_1*tempMean)/(1023-tempMean);
     
    a = 1/T_1;
    b = log10(R_1/R_2);
    c = b / log10(e);
    d = c / BValue ;
    T_2 = 1 / (a- d);
    Serial.print(T_2 - 273.15,decimalPrecision);
    Serial.println(" °C");
     
    tempSampleSum = 0;
    tempSampleCount = 0;
    }
 
  return String(T_2);
}

void display_temperature()
{
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 1);
  display.setTextSize(1);
  display.print("Current Temp: ");
  display.setCursor(0,14);
  display.print(Temperature().c_str());
  display.display();
}

String readtimer() {
    return String(String(minutes)+":"+String(seconds));
  }
String Set_temp_() {
    return String(programmed_temp);
  }
String Fan_status_() {
    return String(Fan);
  }
String Heat_status_() {
    return String(Heat);
  }



// Read encoder function
// return rotation direction 
int readEncoder_temperatrure()
{
    AState_temp = digitalRead(A_temp);
    BState_temp = digitalRead(B_temp) << 1;
    State = AState_temp | BState_temp;
    cw_temp = 0;
    if (lastState_temp != State)
    {
        switch (State)
        {
            case 0:
            if (lastState_temp == 2)
            {
                cw_temp = 1;
            }
            else if (lastState_temp == 1)
            {
                cw_temp = -1;
            }
            break;
            case 1:
            if (lastState_temp == 0)
            {
                cw_temp = 1;
            }
            else if(lastState_temp == 3)
            {
                cw_temp = -1;
            }
            break;
            case 2:
            if (lastState_temp == 3)
            {
                cw_temp = 1;
            }
            else if(lastState_temp == 0)
            {
                cw_temp = -1;
            }
            break;
            case 3:
            if (lastState_temp == 1)
            {
                cw_temp = 1;
            }
            else if(lastState_temp == 2)
            {
                cw_temp = -1;
            }
            break;
        }
    }
    lastState_temp = State;
   
    delay(1);
    return cw_temp;
}

// Set temperature_ function
int settemperature_(int st) 
{
    if (st == 1)          // up count
    {
        temperature_ = (temperature_ == 232) ? 0 : temperature_+1;
    }
    else if (st == -1)    // down count
    {
        temperature_ = (temperature_ == 0) ? 232 : temperature_-1;
    }

    int temperature_;
}


// Button interrupt function

void IRAM_ATTR handleInterrupt() {

    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    if (interrupt_time - last_interrupt_time > 200)
    {
            if (machinestate == 1 || machinestate == 2)
              machinestate = 0;
            else if (machinestate == 1 && minutes == 0 && seconds == 0)
              machinestate = 0;
            else
            {
                machinestate++;
            }
    }
    last_interrupt_time = interrupt_time;
}

// Timer callback function
void IRAM_ATTR action()
{
    if (seconds == 0)
    {
        if (minutes == 0)
        { 
            machinestate = 2;
            // stop counter
            TimerActive = false;
            portENTER_CRITICAL(&timerMux);
            interruptCounter = 0;
            portEXIT_CRITICAL(&timerMux);        
        }
        else
        {
          seconds = 59;
          minutes--;
        }
    }
    else{
        seconds--;
        }
}

void Timer_handling()
{
    switch (machinestate)
    {
        case 0:                                                             // Initial state (SET minutes)
        TimerActive = false;
        minutes =0;
        seconds =0;
        portENTER_CRITICAL(&timerMux);
        interruptCounter = 0;
        portEXIT_CRITICAL(&timerMux);  
        digitalWrite (Fan, LOW); //turn Fan off
        digitalWrite (Heat, LOW); //turn heat off
        digitalWrite (Buzzer, LOW); //turn buzzer on
        setTime(readEncoder());
        draw();
        break;
 
        case 1:                                                             // Running
        draw();
        if (TimerActive == false)
        {
           timer = timerBegin(0, 80, true);
           timerAttachInterrupt(timer, &action, true);
           timerAlarmWrite(timer, PERIOD_MS, true);
           timerAlarmEnable(timer);
           digitalWrite (Fan, HIGH); //turn Fan on
           digitalWrite (Heat, HIGH); //turn heat on
 
           TimerActive = true;
        }
        break;
        case 2:                                                             // Alarm!!
         draw();
        digitalWrite (Fan, LOW); //turn Fan off
        digitalWrite (Heat, LOW); //turn heat off

         
         digitalWrite (Buzzer, HIGH); //turn buzzer on
         delay(100);
         digitalWrite (Buzzer, LOW);  //turn buzzer off
         delay(100);
         digitalWrite (Buzzer, HIGH); //turn buzzer on
         delay(100);
         digitalWrite (Buzzer, LOW);  //turn buzzer off
         delay(100);
         digitalWrite (Buzzer, HIGH); //turn buzzer on
         delay(100);
         digitalWrite (Buzzer, LOW);  //turn buzzer off
         portENTER_CRITICAL(&timerMux);
         interruptCounter = 0;
         portEXIT_CRITICAL(&timerMux);        
         machinestate = 0;        
         break;
    }
}



// Read encoder function
// return rotation direction 
int readEncoder()
{
    AState = digitalRead(AInput);
    BState = digitalRead(BInput) << 1;
    State = AState | BState;
    cw = 0;
    if (lastState != State)
    {
        switch (State)
        {
            case 0:
            if (lastState == 2)
            {
                cw = 1;
            }
            else if (lastState == 1)
            {
                cw = -1;
            }
            break;
            case 1:
            if (lastState == 0)
            {
                cw = 1;
            }
            else if(lastState == 3)
            {
                cw = -1;
            }
            break;
            case 2:
            if (lastState == 3)
            {
                cw = 1;
            }
            else if(lastState == 0)
            {
                cw = -1;
            }
            break;
            case 3:
            if (lastState == 1)
            {
                cw = 1;
            }
            else if(lastState == 2)
            {
                cw = -1;
            }
            break;
        }
    }
    lastState = State;
   
    delay(1);
    return cw;
}


// Set time function
void setTime(int st)
{
    if (st == 1)          // up count
    {
        if((seconds==60)&&(minutes<=10))
        {
          seconds =0;
          minutes = minutes+1;
        }
        else 
        {
          seconds =seconds+30;
        }
    }
    else if (st == -1)    // down count
    {
        if((seconds==0)&&(minutes>=0))
        {
          seconds =0;
          minutes = minutes-1;
        }
        else 
        {
          seconds =seconds-30;
        }
    }
}

// Display function
void draw(void) 
{
    display.clearDisplay();
    display.setTextSize(TEXT_SIZE);                   
    display.setTextColor(WHITE, BLACK);       
    display.setCursor(2,0);                     
    switch (machinestate)
    {
        case 0:                                                                 // SET Minutes
            display.println(F("SET Minutes"));
            display.setTextSize(TEXT_SIZE);                                             
            display.setTextColor(WHITE, BLACK); 
            display.print("   ");
            display.setTextColor(WHITE, BLACK);                 
            snprintf(screen, sizeof(screen), "%.2d",minutes);
            display.print(screen);
            display.setTextColor(WHITE, BLACK); 
            snprintf(screen, sizeof(screen), ":%.2d",seconds);
            display.println(screen);
            break;
        case 1:                                                                 // Running
            display.println(F("Running"));
            display.setTextSize(TEXT_SIZE);                                             
            display.setTextColor(WHITE, BLACK);            
            snprintf(screen, sizeof(screen), "   %.2d:%.2d",minutes,seconds);
            display.println(screen);
            break;
        case 2:                                                                 // Alarm
            display.println();
            display.setTextSize(TEXT_SIZE);                                             
            display.setTextColor(WHITE, BLACK); 
            display.print(F("  "));
            display.println(F("ALARM!!"));
            break;
    }
    display.display();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 Temperature Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
    <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Set_Temperature</span> 
    <span id="set_temperature">%SET_TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>

  <p>
    <span class="dht-labels">Timer</span>
    <span id="timer">%TIMER%</span>
  </p>
  <p>
    <span class="dht-labels">Fan_status</span> 
    <span id="fan_status">%FAN_STATUS%</span>
  </p>
  <p>
    <span class="dht-labels">Heat_status</span> 
    <span id="heat_status">%HEAT_STATUS%</span>
  </p>
  </body>
<script>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("set_temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/set_temperature", true);
  xhttp.send();
}, 50) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("timer").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/timer", true);
  xhttp.send();
}, 50) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("fan_status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/fan_status", true);
  xhttp.send();
}, 50) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("heat_status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/heat_status", true);
  xhttp.send();
}, 50) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return Temperature();
  }
  else if(var == "TIMER"){
    return readtimer();
  }
   else if(var == "SET_TEMPERATURE"){
    return Set_temp_();
  }
  else if(var == "FAN_STATUS"){
    return Fan_status_();
  }
   else if(var == "HEAT_STATUS"){
    return Heat_status_();
  }
  
  return String();
}


void setup() {
   /* Initialize the Display*/
   display.begin();

   /* Change the contrast using the following API*/
   display.setContrast(contrastValue);

   /* Clear the buffer */
   display.clearDisplay();
   display.display();
 
    pinMode(AInput, INPUT_PULLUP);
    pinMode(BInput, INPUT_PULLUP);
    pinMode(ButtonInput, INPUT_PULLUP);
    pinMode (Buzzer, OUTPUT);
    pinMode (Fan, OUTPUT);
    pinMode (Heat, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(ButtonInput), handleInterrupt, RISING  );

    pinMode(A_temp, INPUT_PULLUP);
    pinMode(B_temp, INPUT_PULLUP);
    pinMode(set_temp, INPUT);
 
    Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");}

    // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", Temperature().c_str());
  });

  server.on("/timer", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readtimer().c_str());
  });

  server.on("/fan_status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", Set_temp_().c_str());
  });

  server.on("/heat_status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", Fan_status_().c_str());
  });

  server.on("/set_temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", Heat_status_().c_str());
  });
  
  // Start server
  server.begin();
}

void loop()
{
    display_temperature();

    Timer_handling();

    int val_programmed = settemperature_(readEncoder_temperatrure());    
    
    if(digitalRead(set_temp)==HIGH)
    {
      programmed_temp = val_programmed; 

        display.setTextColor(WHITE, BLACK);
        display.setCursor(4, 0);
        display.setTextSize(1);
        display.print("Set Temp: ");
        display.setCursor(4,10);
        display.print((String)programmed_temp);
        display.display();     
    } 
    else 
    {
        display.setTextColor(WHITE, BLACK);
        display.setCursor(4, 0);
        display.setTextSize(1);
        display.print("Set Temp: ");
        display.setCursor(4,10);
        display.print((String)programmed_temp);
        display.display();                
    }  

 

 
}
