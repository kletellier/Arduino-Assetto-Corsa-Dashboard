#include <TM1638plus.h>
#include <TM1638plus_font.h>
#include <TM1638plus_Model2.h>

#define TIME_STRING "--:--.---"
#define ALLOW_TEST    true
#define  STROBE_TM 7
#define  CLOCK_TM 9
#define  DIO_TM 8

typedef enum {
    ButtonNone = 0,
    Button1 = 1,
    Button2 = 2,
    Button3 = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,
    Button8 = 8
} TMButton;

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
 

TMButton iLastPressed = ButtonNone;
int iTrigger = 25;
int iStartPressed = 0;
int iMode = 1;
int iModeMax = 5;
int iLum = 255;
int iProgress = 0;
bool bNewMode = true;
long lastRefresh = 0;
int iLap = 0;
float fFuel = 0;
int iAlternate = 0;
bool bModeAlternate = false;
float fLastConsumption = 0.0;

Display_1 stDisplay1 = {"0",0,"0","N"};
Display_2 stDisplay2 = {"0",0,""};
Display_3 stDisplay3 = {0,0,"0","_"};
Display_4 stDisplay4 = {"0","0","0","0"};
Display_5 stDisplay5 = {"0","0","0","0"};
Display_6 stDisplay6 = {"0"};

TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM);

void setup() {
  Serial.begin(115200); 
  tm.displayBegin();   
  RefreshDisplay();
}
void loop() {
   
  TMButton btn = GetButton(); 
  
  switch (btn)
  {
    case Button1:
      iMode = 1;
      bNewMode = true;
      break;
    case Button2:
      iMode = 2;
      bNewMode = true;
      break;
    case Button3:
      iMode = 3;
      bNewMode = true;
      break;
    case Button4:
      iMode = 4;
      bNewMode = true;
      break;
    case Button5:
      iMode = 5;
      bNewMode = true;
      break;
    case Button6:
      iMode = 6;
      bNewMode = true;
    case Button8:
      FillTestValue();
      RefreshDisplay();
      break;
    default:
      break;
  }
  

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('|');
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
      if(iLap>=2 && fLastConsumption > 0.0)
      {         
          float fNbLap = iValue.toFloat() / fLastConsumption;
          char cLaps[10]; 
          int iNbDec = 1;
          if(fNbLap>100)
          {
            iNbDec = 0;            
          }        
          stDisplay2.Lap = String(fNbLap,iNbDec);
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
          stDisplay2.Lap = String(fNbLap,iNbDec);
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
  if(actual-lastRefresh>=30)
  {
    iAlternate++;
    if(bNewMode){ iAlternate=0; tm.reset(); bNewMode = false;}
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
    if(iMode==6)
    {
      DisplayMode6();
    }
    lastRefresh = actual;
    if(iAlternate>100)
    {
      bModeAlternate = (bModeAlternate==true) ? false : true;
      iAlternate = 0;
    }
  }  
}

void DisplayMode1()
{    
   // Display Speed Rpm and Gear
   tm.displayHex(0,  stDisplay1.Gear.toInt()); 

   // progressbar since 50 %
   float fPercent = (float)stDisplay1.Percent * 1.0;
   float fNum = fPercent - 50.0; 
   if( fNum < 0.0 ){ fNum=0.0; }
   float fLedsIncr = 50.0 / 8.0;
   float fLeds = fNum / fLedsIncr;
   int iLeds = (int) floor(fLeds);

   for (int i = 0; i < 8; i++)
   {
     int iDis = (iLeds >= i) ? 1 : 0;
     tm.setLED(i,iDis);
   }
   
   DisplayAt(stDisplay1.Speed,5,3);
}

void DisplayMode2()
{
  // Display Fuel Level (ProgressBar and value) and laps number to ran out of fuel
  if(bModeAlternate)
  {
    stDisplay2.Lap.trim();
    String sLap = "Lp" + padLeft(stDisplay2.Lap," ",6);
    tm.displayText(sLap.c_str());
    //DisplayString(sLap);    
  }
  else
  {
    stDisplay2.Fuel.trim();
    String sFuel = "L" + padLeft(stDisplay2.Fuel," ",7);
    //DisplayString(sFuel);
    tm.displayText(sFuel.c_str());
  }
}

void DisplayMode3()
{
  // Display Last Time
  char buf[25] = "";
  FormatTime(buf,stDisplay3.LastTime);
  for (int i = 0; i < 8; i++)
  {   
    tm.displayASCII(i, (int)buf[i]);
  } 
}

void DisplayMode4()
{
  // Display Best Time
  char buf[25] = "";
  FormatTime(buf,stDisplay3.BestTime);
  for (int i = 0; i < 8; i++)
  {   
    tm.displayASCII(i, (int)buf[i]);
  } 
}

void DisplayMode5()
{
   // Display Lap count and Position
   String sDisplay;
   String sPosition = "P";
   String sLapCount = "Lp";

   sPosition = sPosition + padLeft(stDisplay3.Position," ",3);
   sLapCount = sLapCount + padLeft(stDisplay3.Lap," ",2);
   sDisplay = sPosition + sLapCount;
   tm.displayText(sDisplay.c_str());

}

void DisplayMode6()
{
  // display PerformanceMeter
  String sDisplay = padLeft(stDisplay6.PerformanceMeter," ",8);
  tm.displayText(sDisplay.c_str());
}

void DisplayAt(String pDisplay,int pPos, int pLen)
{
  int diff = pLen - pDisplay.length();   
  String newvalue = pDisplay;
  if(diff>0){   
    for (int i = 0; i < diff; i++)
    {
      newvalue = " " + newvalue;
    }    
  }
  int iLen = newvalue.length();
  int iStart = pPos;
  char aChar[8];
  newvalue.toCharArray(aChar,sizeof(aChar));
  for (int i = 0; i < iLen; i++)
  {
    int iOffset = i + iStart;   
    tm.displayASCII(iOffset, (int)aChar[i]);
  }  
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
  stDisplay3.Position = random(1,24);
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

void DisplayString(String pStr)
{
  char buf[8];
  pStr.toCharArray(buf, sizeof(buf));
  tm.displayText(buf);
}

long StringToLong(String pVal)
{ 
  char buf[64];
  pVal.toCharArray(buf, sizeof(buf));
  return atol(buf);  
}

TMButton GetButton()
{
    TMButton pState = GetState();
    unsigned long pTimer = millis();

    if(pState==ButtonNone)
    {
      if(iLastPressed!=ButtonNone)
      {
        if(pTimer - iStartPressed >= iTrigger)
        {
          TMButton returnKey = iLastPressed;
          iLastPressed = ButtonNone;
          return returnKey;
        }
        else
        {
           iLastPressed = ButtonNone;
           return ButtonNone;
        }        
      }
      else
      {
         return ButtonNone;
      }      
    } 
    else
    {
       if(iLastPressed != pState)
       {
          iStartPressed = millis();
          iLastPressed = pState;
       } 
       return ButtonNone;       
    }    
}

TMButton GetState()
{
   uint8_t btn = tm.readButtons();
   switch (btn)
   {
   case 1:
     return Button1;
     break;
   case 2:
     return Button2;
     break;
   case 4:
     return Button3;
     break;
   case 8:
     return Button4;
     break;
    case 16:
     return Button5;
     break;
    case 32:
     return Button6;
     break;
    case 64:
     return Button7;
     break;
    case 128:
     return Button8;
     break;

   default:
   return ButtonNone;
     break;
   }
  //return tm.readButtons();
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

String padLeft(String value, String padStr, int nbLen)
{ 
  String sValue = value;
  sValue.replace(".",""); 
  int diff = nbLen - sValue.length(); 
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

void SplitToArray(String pVal,char **array)
{  
  char *sep = ";";
  char buf[64];
  pVal.toCharArray(buf,sizeof(buf));
  split(array,buf,sep); 
}
