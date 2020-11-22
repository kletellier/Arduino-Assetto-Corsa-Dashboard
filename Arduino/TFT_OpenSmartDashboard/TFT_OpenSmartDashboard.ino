// Specific for ACC 3.2" inch TFT works with Arduino Mega

#include <Adafruit_GFX.h>    // Core graphics library
#include <OPENSMART_kbv.h> 
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define TIME_STRING "--:--.---" 

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20

#define ORANGE_TRIGGER  192
#define RED_TRIGGER  256
 

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
byte rotation = 3; 
long diffLess;
bool bForceDisplay = true;

 
Display stDisplay = {0,"0","N",0,0,"0","","","",0,"",""}; 
LastDisplay stLast = {0,"0","N",0,0,"0","","","",0,"",""}; 

OPENSMART_kbv tft; 

void setup(void) {
  Serial.begin(115200);   
  tft.reset();  
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481; // Fix for openSmart TFT 
  tft.begin(ID);
  ResetDisplay(); 
  pinMode(13, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
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

void ResetDisplay()
{
  tft.fillScreen(BLACK);
  tft.setRotation(rotation);
  DisplayBorders();
}

void RefreshDisplay()
{
  long actual = millis();
  diffLess = actual - lastRefreshLess;

  if(actual-lastRefresh>=90) //more time between refresh for less remanence
  {
    // Send message to server to stop sending during refreshing display
    Serial.print("O"); 
    DisplayMode1();  
    // refresh display
    lastRefresh = actual;     
  }  
  long display = millis();
  long displaytime = display - actual;
  bForceDisplay = false;
  // enable server to send message again
  Serial.print("I");
}

void DisplayMode1()
{
  // Display Speed,Gear,Rpm , Fuel , Fuel remaining estimated laps, Best Time and LastTime
  if(bForceDisplay || stLast.Speed!=stDisplay.Speed)
  {
    char last[10];
    char actual[10];

    padLeft(stDisplay.Speed," ",3).toCharArray(actual,sizeof(actual));
    padLeft(stLast.Speed," ",3).toCharArray(last,sizeof(last));

    for (int i = 0; i < 3; i++)
    {
        char act_char = actual[i];
        char last_char = last[i];

        if(act_char!=last_char)
        {
          int iOffset = 0 + (i * (11 * charWidth));
          tft.drawChar(iOffset,0, act_char, WHITE, BLACK, 9);
        }
    }  
    stLast.Speed = stDisplay.Speed;
  }

  int iDelta = abs(stDisplay.Percent - stLast.Percent);

  if(bForceDisplay || (stLast.Percent!=stDisplay.Percent && iDelta>3))
  {      
    int iPix = (int) stDisplay.Percent * 3.2;       
      if(stLast.Percent>stDisplay.Percent)
      {
        // draw black offset
        tft.fillRect(iPix,81,iLastPix-iPix,38,BLACK);
      }
      else
      {
        // draw green offset
        tft.fillRect(iLastPix,81,iPix-iLastPix,38,GREEN);
      }
      
     if(iPix>RED_TRIGGER)
     {
       tft.fillRect(RED_TRIGGER,81, iPix - RED_TRIGGER, 38,RED);
       tft.fillRect(ORANGE_TRIGGER,81,RED_TRIGGER-ORANGE_TRIGGER,38,YELLOW);
     }
     else
     {
       if(iPix>ORANGE_TRIGGER)
       {
         tft.fillRect(ORANGE_TRIGGER,81,iPix - ORANGE_TRIGGER,38,YELLOW);
       }              
     }
    int iGreen = (iPix > ORANGE_TRIGGER) ? ORANGE_TRIGGER : iPix;
    tft.fillRect(0,81,iGreen,38,GREEN);     
    
    iLastPix = iPix;
    stLast.Percent = stDisplay.Percent;      
  }

  if(bForceDisplay || stLast.Gear!=stDisplay.Gear)
  {          
    char cSpeed[2];
    stDisplay.Gear.toCharArray(cSpeed,sizeof(cSpeed));
    tft.drawChar(260,0, cSpeed[0], GREEN, BLACK, 10);         
    stLast.Gear = stDisplay.Gear;
  }     

  if(bForceDisplay || stLast.BestTime!=stDisplay.BestTime)
  {        
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(2);
    tft.setCursor(15,200);
    char pStr[10];
    FormatTime(pStr,stDisplay.BestTime) ;
    tft.println(pStr);
    stLast.BestTime = stDisplay.BestTime;
  }
        
  if(bForceDisplay || stLast.LastTime!=stDisplay.LastTime)
  {       
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(2);
    char pStr2[10];
    tft.setCursor(185,200);
    FormatTime(pStr2,stDisplay.LastTime) ;
    tft.println(pStr2);
    stLast.LastTime = stDisplay.LastTime;
  } 

  if(diffLess>1000)
  {
      if(bForceDisplay || stLast.Fuel!=stDisplay.Fuel)
      {         
        tft.setTextSize(3);
        tft.setCursor(10,135);
        tft.setTextColor(WHITE,BLACK);
        tft.println(padLeft(stDisplay.Fuel," ",6));
        stLast.Fuel = stDisplay.Fuel;
      }

      if(bForceDisplay || stLast.Lap!=stDisplay.Lap)
      {
        tft.setTextColor(WHITE,BLACK);
        tft.setCursor(175,135);
        tft.println(padLeft(stDisplay.Lap," ",6));
        stLast.Lap = stDisplay.Lap;
      }
    
    
  }   

  if(stDisplay.Flag!=0 && stLast.Flag!=stDisplay.Flag)
  {
    // Display Flag
    DisplayFlag(stDisplay.Flag); 
  }
  
  if(stLast.Flag!=0 && stDisplay.Flag==0)
  {
    // Remove Flag
    DisplayFlag(stDisplay.Flag); 
  }
}

void DisplayFlag(int iFlag)
{
  stLast.Flag = iFlag;
  // define color
  uint16_t iColor = BLACK; // empty flag
  switch (iFlag)
  {     
   case 1:
      iColor = BLUE;      
   break;
   case 2:
    iColor = YELLOW;
   break;     
    case 4:
    iColor = WHITE;
   break;
    case 0:
    case 3:
    case 5:
    case 6:
    iColor = BLACK;
   break;
   case 7: 
    iColor = GREEN;
   break;
    case 8: 
    iColor = ORANGE;
   break;
  default:
    break;
  }
  tft.fillRect(321,0,79,80 ,iColor);
}

void DisplayBorders()
{
   
    tft.drawFastHLine(0,80, 320, WHITE);
    tft.drawFastHLine(0,120, 320, WHITE);
    tft.drawFastHLine(0,190, 320, WHITE);
    tft.drawFastVLine(200, 0, 80, WHITE);
    tft.drawFastVLine(160, 120, 140, WHITE);
    tft.drawFastVLine(320, 0, 240, WHITE);
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
