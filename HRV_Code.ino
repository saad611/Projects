//Variable declaration
const int ecgInput = 11; //Input Pin of arduino

byte oldSample; // variable for comparision with next samples

byte sample; // collecting samples

bool alreadyPeaked = false; //Flag for successive peak detection

unsigned long firstPeakTime = 0; //Gets the time for first peak detected

unsigned long secondPeakTime = 0; //Gets the time for second peak detected

unsigned long rrInterval = 0; //Variable used for calculating time between two successive occurence

int numRRDetected = 0; //successive peaks count

bool hrvStarted = false; //Flag to initiate HRV calculation

bool hrvUpdate = false;  //Flag to update HRV calculation

bool hrvComplete = false; //Flag to finish HRV calculation

unsigned long hrvStartTime = 0; //Variable to initiate HRV calculation time

unsigned long rrIntervalPrevious = 0; //Variable to store last time of peak occurence 

float rrDiff = 0.0; //Variable for calculating difference between collected samples

float rrDiffSquaredTotal = 0.0; //Variable for calculating square of collected samples

float diffCount = 0.0; //Variable for counting no. of samples of interval difference

float rmssd = -1.0; //intial value of rsmssd

//Initialize the System
void setup()
{
    // Setup Serial Monitor
    Serial.begin(9600);
    pinMode (ecgInput, INPUT);  //Signal pin to input
    pinMode(LED_BUILTIN, OUTPUT);//Arduino builtin pin used for display 

    // Wait until a heart beat is detected  
  while (!digitalRead(ecgInput))
  {
    //Wait in this loop until beats don't get detected
  };
  
  Serial.println ("Heart beat detected!"); //Successful print on serial port upon beat detection
}

//Here main function works 
void loop()
{
    //Collect digital sample from PT34 reciever at Arduino pin 11  (0/1)
    sample  = digitalRead(ecgInput);

    //Comparision with old sample and currenct sample for further calculation
    if (sample && (oldSample != sample)) 
    {
        //Condition for proceeding to HRV calculation
        //Calculation shall only be performed when arduino recieves HIGH from receiver module
        if ((firstPeakTime == 0)&&(sample==(int)1)) 
        {
        // If this is the very first peak, set the first peak time
        firstPeakTime = millis();
        }
        
        else if((firstPeakTime != 0)&&(sample==(int)1))
        {
          secondPeakTime = millis();
          // Store the previous R-to-R interval for use in HRV calculations
          rrIntervalPrevious = rrInterval;
          // Calculate new R-to-R interval and set HRV update flag
          rrInterval = secondPeakTime - firstPeakTime;
          firstPeakTime = secondPeakTime;
          hrvUpdate = true;
          numRRDetected = numRRDetected + 1;
      }
      alreadyPeaked = true;
    }

    else 
   {
     alreadyPeaked = false;
   }
   oldSample = sample;           //Store last signal received

   

   // Once two consecutive R-to-R intervals have been detected, 
  // start the 2 minute HRV window
  if (!hrvStarted && numRRDetected >= 2) {
    hrvStarted = true;
    hrvStartTime = millis();
  }

  // If a new R-to-R interval has been detected, update the HRV measure
  if (hrvUpdate && hrvStarted) {
    // Add the square of successive differences between 
    // R-to-R intervals to the running total
    rrDiff = float(rrInterval) - float(rrIntervalPrevious);
    rrDiffSquaredTotal = rrDiffSquaredTotal + sq(rrDiff);
    // Count the number of successive differences for averaging
    diffCount = diffCount + 1.0;
    // Reset the hrvUpdate flag
    hrvUpdate = false;
  }

  // Once two minute window has elapsed, calculate the RMSSD
  if (millis() - hrvStartTime >= 120000 && !hrvComplete) {
    rmssd = sqrt(rrDiffSquaredTotal/diffCount);
    hrvComplete = true;
  }

   Serial.println(rmssd);
}
    
