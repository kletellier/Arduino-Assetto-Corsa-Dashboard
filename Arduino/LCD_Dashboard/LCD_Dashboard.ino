#include <KeyPad.h>
#include "LiquidCrystal.h"
 
#define LCD_PIN_RS          8   //!< LCD RS pin
#define LCD_PIN_EN          9   //!< LCD EN pin
#define LCD_PIN_D0          4   //!< LCD D0 pin
#define LCD_PIN_D1          5   //!< LCD D1 pin
#define LCD_PIN_D2          6   //!< LCD D2 pin
#define LCD_PIN_D3          7   //!< LCD D3 pin

// Backlight pin
#define LCD_BACK_LIGHT_PIN  10  //!< LCD backlight pin

#define LCD_ROW 2
#define LCD_COLUMN 16

#define TIME_STRING "--:--.---"

#define ALLOW_TEST    true

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
  PERFORMANCE_METER
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
  String FrontLeft;
  String FrontRight;
  String RearLeft;
  String RearRight;  
} Display_4;

typedef struct {
  String FrontLeft_Status;
  String FrontRight_Status;
  String RearLeft_Status;
  String RearRight_Status;
} Display_5;

typedef struct {
  String PerformanceMeter;
} Display_6;


typedef struct {
  String Command;
  String Value;
} Command;
 

int iMode = 1;
int iModeMax = 5;
int iLum = 255;
int iProgress = 0;
bool bNewMode = true;
long lastRefresh = 0;
int iLap = 0;
float fFuel = 0;
float fLastConsumption = 0.0;

Display_1 stDisplay1 = {"0",0,"0","N"};
Display_2 stDisplay2 = {"0",0,""};
Display_3 stDisplay3 = {0,0,"0","_"};
Display_4 stDisplay4 = {"0","0","0","0"};
Display_5 stDisplay5 = {"0","0","0","0"};
Display_6 stDisplay6 = {"0"};

LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_EN,  LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3);
KeyPad KeyPad(0,10, &lcd);  

void setup() {
  Serial.begin(115200); 
  KeyPad.Display(0, 0,"AC Dashboard");   
  KeyPad.Display(0, 1,"<- or ->"); 
  delay(1000);
  RefreshDisplay();
}
void loop() {
  LCDButton pButton = KeyPad.GetButton();
 
  if (pButton == ButtonUp) {
    iMode++;
    if (iMode > iModeMax)
    {
      iMode = iModeMax;
    }
    KeyPad.Clear();
    bNewMode = true;    
    RefreshDisplay();
  }
  else if (pButton == ButtonRight) { 
    iLum += 25;
    if (iLum > 255) {
      iLum = 255;
    }
    KeyPad.ChangeBackLight(iLum);
     
  }
  else if (pButton == ButtonLeft ) { 
    iLum -= 25;
    if (iLum < 10) {
      iLum = 10;
    }
    KeyPad.ChangeBackLight(iLum);
  }
  else if (pButton == ButtonDown) {
    iMode--;
    if (iMode < 1)
    {
      iMode = 1;
    }
    KeyPad.Clear();
    bNewMode = true;
    RefreshDisplay();
  }
  else if (pButton == ButtonSelect) {     
    if(ALLOW_TEST)
    {
      FillTestValue();
      RefreshDisplay();
    }    
  }

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('|');
    int iPosDollar = command.indexOf("$");
    if (command != "" && (iPosDollar==1 || iPosDollar==2))
    {
      ParseCommand(command);
      RefreshDisplay();
    }
  }
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
      if(iLap>=2 && fLastConsumption > 0.0)
      {         
          float fNbLap = iValue.toFloat() / fLastConsumption;
          char cLaps[10]; 
          int iNbDec = 1;
          if(fNbLap>100)
          {
            iNbDec = 0;            
          }        
          stDisplay2.Lap = padLeft(String(fNbLap,iNbDec)," ",4) + "Laps";
      }
      break; 
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
        float fActualFuel = stDisplay2.Fuel.toFloat();
        if(iValue.toInt() > 1)
        {               
          float fConsumption = fFuel - fActualFuel;
          fLastConsumption = fConsumption;
          float fNbLap = fActualFuel / fConsumption;
          char cLaps[10];
          //dtostrf(fNbLap 2, 1, cLaps); 
          int iNbDec = 1;
          if(fNbLap>100)
          {
            iNbDec = 0;            
          }        
          stDisplay2.Lap = padLeft(String(fNbLap,iNbDec)," ",4) + "Laps";
          
        }
        iLap = iValue.toInt();
        fFuel = fActualFuel;
      }
      break;
    case TIME:
      char *timeArray[2];
      SplitToArray(iValue,timeArray);
      stDisplay3.BestTime = StringToLong(timeArray[0]);
      stDisplay3.LastTime = StringToLong(timeArray[1]);
      break; 
    case TYRE_TEMP:
      char *tempArray[4];
      SplitToArray(iValue,tempArray);
      stDisplay4.FrontLeft = tempArray[0]; 
      stDisplay4.FrontRight = tempArray[1]; 
      stDisplay4.RearLeft = tempArray[2]; 
      stDisplay4.RearRight = tempArray[3]; 
      break;   
    case TYRE_STATUS:
      char *statusArray[4];
      SplitToArray(iValue,statusArray);
      stDisplay5.FrontLeft_Status = statusArray[0]; 
      stDisplay5.FrontRight_Status = statusArray[1]; 
      stDisplay5.RearLeft_Status = statusArray[2]; 
      stDisplay5.RearRight_Status = statusArray[3]; 
      break;  
    case PERFORMANCE_METER:
      stDisplay6.PerformanceMeter = iValue;
      break;        
    default:
      break;
  }       
}

void RefreshDisplay()
{
  long actual = millis();
  if(actual-lastRefresh>=100)
  {
    if(bNewMode){ KeyPad.Clear(); bNewMode = false;}
    if(iMode==1)
    {
      DisplayMode1();
    }
    if(iMode==2)
    {
      DisplayMode2();
    }
    if(iMode==3)
    {
      DisplayMode3();
    }
    if(iMode==4)
    {
      DisplayMode4();
    }
    if(iMode==5)
    {
      DisplayMode5();
    }
    lastRefresh = actual;
  }  
}

void DisplayMode1()
{   
   // Display Speed Rpm and Gear
   printDisplayField(stDisplay1.Rpm,"0","-----",0,0);
   printDisplayField(stDisplay1.Speed,"0","---",7,0);
   printDisplayField(stDisplay1.Gear,"N","-",15,0);
   if(iProgress != stDisplay1.Percent)
   {      
     KeyPad.DrawProgressBar(1,stDisplay1.Percent);
     iProgress = stDisplay1.Percent;
   }   
}

void DisplayMode2()
{
  // Display Fuel Level (ProgressBar and value) and laps number to ran out of fuel
  printDisplayField(stDisplay2.Fuel,"0","----",0,0);
  printDisplayField(stDisplay2.Lap,"0","----",12,0);
  KeyPad.DrawProgressBar(1,stDisplay2.Percent);
}

void DisplayMode3()
{
  // Display Last Time, Best Time, Actual Time and lap completed
  printDisplayField(FormatTime(stDisplay3.BestTime),TIME_STRING,TIME_STRING,0,0);
  printDisplayField(FormatTime(stDisplay3.LastTime),TIME_STRING,TIME_STRING,0,1);
  printDisplayField(stDisplay3.Lap,"0","--",14,0);
  printDisplayField(stDisplay3.Position,"_","--",14,1);
}

void DisplayMode4()
{
  // Display Tyre temp
  printDisplayField(stDisplay4.FrontLeft,"0","---",0,0);
  printDisplayField(stDisplay4.FrontRight,"0","---",12,0);
  printDisplayField(stDisplay4.RearLeft,"0","---",0,1);
  printDisplayField(stDisplay4.RearRight,"0","---",12,1);
  // Draw ° character
  KeyPad.SpecialChar(3,0,6);
  KeyPad.SpecialChar(3,1,6);
  KeyPad.SpecialChar(15,0,6);
  KeyPad.SpecialChar(15,1,6);
}

void DisplayMode5()
{
  printDisplayField(stDisplay5.FrontLeft_Status,"0","---",0,0);
  printDisplayField(stDisplay5.FrontRight_Status,"0","---",12,0);
  printDisplayField(stDisplay5.RearLeft_Status,"0","---",0,1);
  printDisplayField(stDisplay5.RearRight_Status,"0","---",12,1);
  // Draw % character
  KeyPad.SpecialChar(3,0,7);
  KeyPad.SpecialChar(3,1,7);
  KeyPad.SpecialChar(15,0,7);
  KeyPad.SpecialChar(15,1,7);
}

void FillTestValue()
{
  // Fill all structure with randomvalue
  stDisplay1.Gear = random(1,6);
  stDisplay1.Rpm = random(2000,20000);
  stDisplay1.Percent = random(10,85);
  stDisplay1.Speed = random(0,320);

  stDisplay2.Fuel = random(0,120);
  stDisplay2.Percent = random(10,85);

  long lBest = random(62000,89000);
  long lLast = lBest + random(0,5000);

  stDisplay3.BestTime = lBest;
  stDisplay3.Lap = random(0,10);
  stDisplay3.LastTime = lLast;

  long lTemp = random(50,95);
  stDisplay4.FrontLeft = lTemp + random(0,10);
  stDisplay4.FrontRight = lTemp + random(0,10);
  stDisplay4.RearLeft = lTemp + random(0,10);
  stDisplay4.RearRight = lTemp + random(0,10);

  long lStatus = random(50,99);
  stDisplay5.FrontLeft_Status = lStatus + random(0,10);
  stDisplay5.FrontRight_Status = lStatus + random(0,10);
  stDisplay5.RearLeft_Status = lStatus + random(0,10);
  stDisplay5.RearRight_Status = lStatus + random(0,10);

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

String FormatTime(long ms)
{  
  long mins = 0;
  long secs = 0;
  long millisec = 0;

  millisec = ms % 1000;
  long tsec = ms / 1000;
  mins = tsec / 60;
  secs = tsec % 60;

  char buffer[25];
  sprintf(buffer, "%02i:%02i.%03i", (int)mins, (int)secs ,(int) millisec);
  return String(buffer);
}

static void printDisplayField(String value, String default_value, String zerostr,   int col, int row)
{ 
  int default_len = zerostr.length();
  int diff = default_len - value.length();
  int coll_offset = col;
  String newvalue = value;
  if(diff>0){
    coll_offset+=diff;
    for (int i = 0; i < diff; i++)
    {
      newvalue = " " + newvalue;
    }
    
  }

  if (default_value == value) {
    KeyPad.Display( col, row, zerostr);       
  } else {
    KeyPad.Display( col, row, newvalue);   
  }  
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
 
