// Specific for ACC 4.3 Nextion Screen works with Arduino Mega
#include "EasyNextionLibrary.h" 

typedef enum {
  RPM,
  RPM_TICK,
  SPEED,
  GEAR,
  TYRE_STATUS,
  TYRE_TEMP,
  TIME,
  INIT,
  FUEL,
  LAP,
  CURRENT_TIME,
  FUEL_TICK,
  CURRENT_POSITION,
  PERFORMANCE_METER,
  FUELXLAP,
  TC,
  ABS,
  ENGINE_MAP,
  FUEL_AUTONOMY,
  FLAG
} MessageType;


typedef struct {
  String Command;
  String Value;
} Command;

typedef struct {
    int Percent;
    String Speed;
    String Gear;
    long BestTime;
    long LastTime;
    String Fuel;
    String Lap;
    String Rpm;
    String Position;
    int Flag;
    String Consumption;
    String Autonomy;
   } Display;
 

typedef struct {
  int Percent;
  String Speed;
  String Gear;
  long BestTime;
  long LastTime;
  String Fuel;
  String Lap;
  String Rpm;
  String Position;
  int Flag;
  String Consumption;
  String Autonomy;
} LastDisplay;
 
  
int iProgress = 0;
long lastRefresh = 0;
long lastRefreshLess = 0;
int iLap = 0; 
int iLastPix = 0;
int charWidth = 5; 
long diffLess;
bool bForceDisplay = true;
int REFRESH_TIME = 100;

EasyNex myNex(Serial2); // Serial2 for Mega2560

Display stDisplay = {0,"0","N",0,0,"0","","","",0,"",""}; 
LastDisplay stLast = {0,"0","N",0,0,"0","","","",0,"",""}; 
 
void setup(void) {
  Serial.begin(115200);   
  myNex.begin(9600);
}

void loop()
{      
 
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('|');
    // prevent bad message
    int iPosDollar = command.indexOf("$");
    if (command != "" && (iPosDollar==1 || iPosDollar==2))
    { 
      ParseCommand(command);      
    }
  }
  RefreshDisplay();
  myNex.NextionListen();
}

Command ExtractCommand(String pStr)
{
  Command cCmd;

  char *array[5] = {"","","","",""};
  char *sep = "$";

  char buf[64];
  pStr.toCharArray(buf,sizeof(buf));
  split(array,buf,sep); 
  
  cCmd.Command = String(array[0]);
  cCmd.Value = String(array[1]);    

  return cCmd;
}

void ParseCommand(String pStr)
{
  Command cmd = ExtractCommand(pStr); 
  String iValue = cmd.Value; 
  switch (cmd.Command.toInt())
  {
    case RPM:
      stDisplay.Rpm = iValue;
      break;
    case RPM_TICK:
      stDisplay.Percent = iValue.toInt();
      break;
    case SPEED:
      stDisplay.Speed = iValue;
      break;
    case GEAR:
      stDisplay.Gear = iValue;
      break;  
    case FUEL:
      stDisplay.Fuel = iValue;      
    case FUEL_TICK:
      stDisplay.Percent = iValue.toInt();
      break; 
    case CURRENT_POSITION:
      stDisplay.Position = iValue;
      break; 
    case LAP:
      stDisplay.Lap = iValue;
      if(iLap!=iValue.toInt())
      {          
        iLap = iValue.toInt(); 
      }
      break;
    case TIME:
      char *timeArray[2];
      SplitToArray(iValue,timeArray);
      stDisplay.BestTime = StringToLong(timeArray[0]);
      stDisplay.LastTime = StringToLong(timeArray[1]);
      break; 
    case FLAG: 
      stDisplay.Flag = iValue.toInt();
      break;
    case FUELXLAP:
      stDisplay.Consumption = iValue;
      break;
    case FUEL_AUTONOMY:
      stDisplay.Autonomy = iValue;
    break;
    default:
      break;
  }       
}
 

void RefreshDisplay()
{
  long actual = millis();
  diffLess = actual - lastRefreshLess;

  if(actual-lastRefresh>=REFRESH_TIME) //more time between refresh for less remanence
  {
    // Send message to server to stop sending during refreshing display
    Serial.print("O"); 
    DisplayScreen();  
    // refresh display
    lastRefresh = actual;     
  }  
  long display = millis();
  long displaytime = display - actual;
  bForceDisplay = false;
  // enable server to send message again
  Serial.print("I");
}

void DisplayScreen()
{
   
    
}
  

void SplitToArray(String pVal,char **array)
{  
  char *sep = ";";
  char buf[64];
  pVal.toCharArray(buf,sizeof(buf));
  split(array,buf,sep); 
}

long StringToLong(String pVal)
{ 
  char buf[64];
  pVal.toCharArray(buf, sizeof(buf));
  return atol(buf);  
}

void FormatTime(char *buf, long ms)
{  
  long mins = 0;
  long secs = 0;
  long millisec = 0;

  millisec = ms % 1000;
  long tsec = ms / 1000;
  mins = tsec / 60;
  secs = tsec % 60;
 
  sprintf(buf, "%02i:%02i.%03i", (int)mins, (int)secs ,(int) millisec); 
}

String FormatTyre(String value,String pRightChar)
{
  return padLeft(value," ",3) + pRightChar;
}

String padLeft(String value, String padStr, int nbLen)
{ 
   
  int diff = nbLen - value.length(); 
  String newvalue = value;
  if(diff>0){     
    for (int i = 0; i < diff; i++)
    {
      newvalue = padStr + newvalue;
    }    
  }

  return newvalue;
}

void split(char *array[],char *pStr,char *pSep)
{
    int i = 0;
    char *p = strtok (pStr, pSep);
    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, pSep);
    }
}
