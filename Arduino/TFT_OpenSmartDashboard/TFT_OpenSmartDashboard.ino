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

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

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

#define ORANGE_TRIGGER  192
#define RED_TRIGGER  256

#define MINPRESSURE 100
#define MAXPRESSURE 1000

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
  FUEL_AUTONOMY
} MessageType;

typedef struct {
   String Rpm;
   int Percent;
   String Speed;
   String Gear;
   } Display_1;

typedef struct {
  String Fuel;
  int Percent;
  String Lap;
} Display_2;

typedef struct {
  long BestTime;
  long LastTime;
  String Lap;
  String Position;
} Display_3;


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
} LastDisplay;
 
  
int iProgress = 0;
long lastRefresh = 0;
long lastRefreshLess = 0;
int iLap = 0; 
int iLastPix = 0;
int charWidth = 5;
byte rotation = 3; 
long diffLess;
bool bForceDisplay = false;

Display_1 stDisplay1 = {"0",0,"0","N"};
Display_2 stDisplay2 = {"0",0,""};
Display_3 stDisplay3 = {0,0,"0","_"};
 
LastDisplay stLast = {0,"0","N",0,0,"0",""};
 

OPENSMART_kbv tft;
//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup(void) {
  Serial.begin(115200);   
  tft.reset();  
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481;  
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
      stDisplay1.Rpm = iValue;
      break;
    case RPM_TICK:
      stDisplay1.Percent = iValue.toInt();
      break;
    case SPEED:
      stDisplay1.Speed = iValue;
      break;
    case GEAR:
      stDisplay1.Gear = iValue;
      break;  
    case FUEL:
      stDisplay2.Fuel = iValue;      
    case FUEL_TICK:
      stDisplay2.Percent = iValue.toInt();
      break; 
    case CURRENT_POSITION:
      stDisplay3.Position = iValue;
      break; 
    case LAP:
      stDisplay3.Lap = iValue;
      if(iLap!=iValue.toInt())
      {          
        iLap = iValue.toInt(); 
      }
      break;
    case TIME:
      char *timeArray[2];
      SplitToArray(iValue,timeArray);
      stDisplay3.BestTime = StringToLong(timeArray[0]);
      stDisplay3.LastTime = StringToLong(timeArray[1]);
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

  if(actual-lastRefresh>=50)
  {
    Serial.print("O"); 
    DisplayMode1();  
    // refresh display
    lastRefresh = actual;     
  }  
  long display = millis();
  long displaytime = display - actual;
  bForceDisplay = false;
  
  Serial.print("I");
}

void DisplayMode1()
{
  // Display Speed,Gear,Rpm , Fuel , Fuel remaining estimated laps, Best Time and LastTime
  if(bForceDisplay || stLast.Speed!=stDisplay1.Speed)
  {
    char last[10];
    char actual[10];

    padLeft(stDisplay1.Speed," ",3).toCharArray(actual,sizeof(actual));
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
    stLast.Speed = stDisplay1.Speed;
  }

  if(bForceDisplay || stLast.Percent!=stDisplay1.Percent)
  {      
    int iPix = (int) stDisplay1.Percent * 3.2;       
      if(stLast.Percent>stDisplay1.Percent)
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
    stLast.Percent = stDisplay1.Percent;      
  }

  if(bForceDisplay || stLast.Gear!=stDisplay1.Gear)
  {          
    char cSpeed[2];
    stDisplay1.Gear.toCharArray(cSpeed,sizeof(cSpeed));
    tft.drawChar(260,0, cSpeed[0], GREEN, BLACK, 10);         
    stLast.Gear = stDisplay1.Gear;
  }     

  if(bForceDisplay || stLast.BestTime!=stDisplay3.BestTime)
  {        
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(2);
    tft.setCursor(15,200);
    char pStr[10];
    FormatTime(pStr,stDisplay3.BestTime) ;
    tft.println(pStr);
    stLast.BestTime = stDisplay3.BestTime;
  }
        
  if(bForceDisplay || stLast.LastTime!=stDisplay3.LastTime)
  {       
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(2);
    char pStr2[10];
    tft.setCursor(185,200);
    FormatTime(pStr2,stDisplay3.LastTime) ;
    tft.println(pStr2);
    stLast.LastTime = stDisplay3.LastTime;
  } 

  if(diffLess>1000)
  {
      if(bForceDisplay || stLast.Fuel!=stDisplay2.Fuel)
      {         
        tft.setTextSize(3);
        tft.setCursor(10,135);
        tft.setTextColor(WHITE,BLACK);
        tft.println(padLeft(stDisplay2.Fuel," ",6));
        stLast.Fuel = stDisplay2.Fuel;
      }

      if(bForceDisplay || stLast.Lap!=stDisplay2.Lap)
      {
        tft.setTextColor(WHITE,BLACK);
        tft.setCursor(175,135);
        tft.println(padLeft(stDisplay2.Lap," ",6));
        stLast.Lap = stDisplay2.Lap;
      }
    
    
  }   
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