// Specific for ACC 4.3 Nextion Screen works with Arduino Mega
#include "EasyNextionLibrary.h" 
 
  const int RPM  = 0;
  const int RPM_TICK  = 1;
  const int SPEED  = 2;
  const int GEAR  = 3;
  const int TYRE_STATUS  = 4;
  const int TYRE_TEMP  = 5;
  const int TIME  = 6;
  const int INIT  = 7;
  const int FUEL  = 8;
  const int LAP  = 9;
  const int CURRENT_TIME  = 10;
  const int FUEL_TICK = 11;
  const int CURRENT_POSITION =  12;
  const int PERFORMANCE_METER  = 13;
  const int FUELXLAP  = 14;
  const int TCTRL  = 15;
  const int ABRS  = 16;
  const int ENGINE_MAP =  17;
  const int FUEL_AUTONOMY =  18;
  const int FLAG =  19;
  const int TYRE_PRESSURE =  20;

  const int COLOR_YELLOW = 2;
  const int COLOR_ORANGE = 3;
  const int COLOR_GREEN = 4;
  const int COLOR_BLACK = 1;


typedef struct {
  String Command;
  String Value;
} Command;

typedef struct {
    int Percent;
    String Speed;
    String Gear;
    String BestTime;
    String LastTime;
    String Fuel;
    String Lap;
    String Rpm;
    String Position;
    int Flag;
    String Consumption;
    String Autonomy;
    String ABS;
    String TC;
    String EM;
    String FrontLeft_Pressure ;
    String FrontRight_Pressure;
    String RearLeft_Pressure ;
    String RearRight_Pressure ;
   } Display;
 

typedef struct {
  int Percent;
  String Speed;
  String Gear;
  String BestTime;
  String LastTime;
  String Fuel;
  String Lap;
  String Rpm;
  String Position;
  int Flag;
  String Consumption;
  String Autonomy;
  String ABS;
  String TC;
  String EM;
  String FrontLeft_Pressure;
  String FrontRight_Pressure;
  String RearLeft_Pressure;
  String RearRight_Pressure;
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
bool bSending = true;
bool bNewCommand = false;
int iLastLed = 0;

EasyNex myNex(Serial2); // Serial2 for Mega2560

Display stDisplay = {0,"0","N","","","0","","","",0,"","","","","","","","",""}; 
LastDisplay stLast = {0,"0","N","","","0","","","",0,"","","","","","","","",""}; 
 
void setup(void) {
  Serial.begin(115200);   
  myNex.begin(9600);

  myNex.writeNum("p2.pic",COLOR_BLACK);
  myNex.writeNum("p3.pic",COLOR_BLACK);
  myNex.writeNum("p4.pic",COLOR_BLACK);
  myNex.writeNum("p5.pic",COLOR_BLACK);
  myNex.writeNum("p6.pic",COLOR_BLACK);
  myNex.writeNum("p7.pic",COLOR_BLACK);
  myNex.writeNum("p8.pic",COLOR_BLACK);
  myNex.writeNum("p9.pic",COLOR_BLACK);
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
      {
      stDisplay.Rpm = iValue;
      break;
      }
    case RPM_TICK:
      {
      stDisplay.Percent = iValue.toInt();
      break;
      }
    case SPEED:
      {
      stDisplay.Speed = iValue;
      break;
      }
    case GEAR:
      {
      stDisplay.Gear = iValue;
      break;  
      } 
    case TIME:
    {
      char *timeArray[2];
      char bufbt[25] = "";
      char buflt[25] = "";
      SplitToArray(iValue,timeArray);
      FormatTime(bufbt,StringToLong(timeArray[0]));
      FormatTime(buflt,StringToLong(timeArray[1]));
      stDisplay.BestTime = bufbt;
      stDisplay.LastTime = buflt; 
      break;
    }    
    case FUEL: 
    {    
      stDisplay.Fuel = iValue;  
      break;
    }
    case LAP:
    {
      stDisplay.Lap = iValue;
      if(iLap!=iValue.toInt())
      {          
        iLap = iValue.toInt(); 
      }
      break; 
    } 
   
    case FUEL_TICK:
    {
      stDisplay.Percent = iValue.toInt();
      break; 
    }
    case CURRENT_POSITION:
    {
      stDisplay.Position = iValue;
      break; 
    }     
    case FUELXLAP:
    {
      stDisplay.Consumption = iValue;
      break;    
    }
    case TCTRL:
    { 
     stDisplay.TC = iValue;
     break;
    }
    case ABRS:
    {
     stDisplay.ABS = iValue;
     break;    
    }
    case ENGINE_MAP: 
    {
     stDisplay.EM = iValue;
     break;
    }
    case FUEL_AUTONOMY:
    {
      stDisplay.Autonomy = iValue;
      break;
    }
    case FLAG: 
    {
      stDisplay.Flag = iValue.toInt();
      break;
    }
    case TYRE_PRESSURE:
    {
      char *pressureArray[4];
      SplitToArray(iValue,pressureArray);
      stDisplay.FrontLeft_Pressure = pressureArray[0]; 
      stDisplay.FrontRight_Pressure = pressureArray[1]; 
      stDisplay.RearLeft_Pressure = pressureArray[2]; 
      stDisplay.RearRight_Pressure = pressureArray[3];    
     break; 
    }
    default:
      break;
  }    
  bNewCommand = true;
}
 

void RefreshDisplay()
{
  long actual = millis();
  
  if(actual-lastRefresh>=REFRESH_TIME) //more time between refresh for less remanence
  {
    // Send message to server to stop sending during refreshing display
    Serial.print("O"); 
    bNewCommand = false;
    bSending = false;
    DisplayScreen();  
    // refresh display
    lastRefresh = actual;     
  }    
  bForceDisplay = false;
  // enable server to send message again
  if(!bSending) { Serial.print("I"); bSending = true; }
}

void DisplayScreen()
{
  if(stDisplay.Speed!=stLast.Speed)
  {
    myNex.writeStr("speed.txt", stDisplay.Speed);
    stLast.Speed = stDisplay.Speed;
  }
  if(stDisplay.Gear!=stLast.Gear)
  {
    myNex.writeStr("gear.txt", stDisplay.Gear);
    stLast.Gear = stDisplay.Gear;
  }
  if(stDisplay.Rpm!=stLast.Rpm)
  {
    myNex.writeStr("rpm.txt", stDisplay.Rpm); 
    stLast.Rpm = stDisplay.Rpm;
  }
  if(stDisplay.Fuel!=stLast.Fuel)
  {
    myNex.writeStr("fuell.txt", stDisplay.Fuel); 
    stLast.Fuel = stDisplay.Fuel;
  }
  if(stDisplay.Autonomy!=stLast.Autonomy)
  {
    myNex.writeStr("fuela.txt", stDisplay.Autonomy); 
    stLast.Autonomy = stDisplay.Autonomy;
  }
  if(stDisplay.TC!=stLast.TC)
  {
    myNex.writeStr("tc.txt", stDisplay.TC); 
    stLast.TC = stDisplay.TC;
  }
  if(stDisplay.ABS!=stLast.ABS)
  {
    myNex.writeStr("abs.txt", stDisplay.ABS); 
    stLast.ABS = stDisplay.ABS;
  }
  if(stDisplay.EM!=stLast.EM)
  {
    myNex.writeStr("em.txt", stDisplay.EM); 
    stLast.EM = stDisplay.EM;
  }
  if(stDisplay.LastTime!=stLast.LastTime)
  {
    myNex.writeStr("laptime.txt", stDisplay.LastTime); 
    stLast.LastTime = stDisplay.LastTime;
  }

  if(stDisplay.FrontLeft_Pressure!=stLast.FrontLeft_Pressure)
  {
    myNex.writeStr("flp.txt", stDisplay.FrontLeft_Pressure); 
    stLast.FrontLeft_Pressure = stDisplay.FrontLeft_Pressure;
  }
  if(stDisplay.FrontRight_Pressure!=stLast.FrontRight_Pressure)
  {
    myNex.writeStr("frp.txt", stDisplay.FrontRight_Pressure); 
    stLast.FrontRight_Pressure = stDisplay.FrontRight_Pressure;
  }
  if(stDisplay.RearLeft_Pressure!=stLast.RearLeft_Pressure)
  {
    myNex.writeStr("rlp.txt", stDisplay.RearLeft_Pressure); 
    stLast.RearLeft_Pressure = stDisplay.RearLeft_Pressure;
  }
  if(stDisplay.RearRight_Pressure!=stLast.RearRight_Pressure)
  {
    myNex.writeStr("rrp.txt", stDisplay.RearRight_Pressure); 
    stLast.RearRight_Pressure = stDisplay.RearRight_Pressure;
  }

  // Display led rpm
  DisplayProgressBar();
   
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

void DisplayProgressBar()
{
  // progressbar since 50 %
  float fPercent = (float)stDisplay.Percent * 1.0;
  float fNum = fPercent - 50.0; 
  if( fNum < 0.0 ){ fNum=0.0; }
  float fLedsIncr = 50.0 / 8.0;
  float fLeds = fNum / fLedsIncr;
  int iLeds = (int) floor(fLeds);

  if(iLastLed!=iLeds)
  {
    int iMinLed = 9;
    int iMaxLed = 0;

    if(iLeds<iMinLed) { iMinLed = iLeds; }
    if(iLastLed<iMinLed) { iMinLed = iLastLed; }

    if(iLeds>iMaxLed) { iMaxLed = iLeds;}
    if(iLastLed>iMaxLed) { iMaxLed = iLastLed;}

    if(iMinLed<=1 && iMaxLed>=1)  {if(iLeds>=1){myNex.writeNum("p2.pic",COLOR_GREEN);} else {myNex.writeNum("p2.pic",COLOR_BLACK);} }
    if(iMinLed<=2 && iMaxLed>=2)  {if(iLeds>=2){myNex.writeNum("p3.pic",COLOR_GREEN);} else {myNex.writeNum("p3.pic",COLOR_BLACK);} }
    if(iMinLed<=3 && iMaxLed>=3)  {if(iLeds>=3){myNex.writeNum("p4.pic",COLOR_GREEN);} else {myNex.writeNum("p4.pic",COLOR_BLACK);} }
    if(iMinLed<=4 && iMaxLed>=4)  {if(iLeds>=4){myNex.writeNum("p5.pic",COLOR_GREEN);} else {myNex.writeNum("p5.pic",COLOR_BLACK);} }
    if(iMinLed<=5 && iMaxLed>=5)  {if(iLeds>=5){myNex.writeNum("p6.pic",COLOR_YELLOW);} else {myNex.writeNum("p6.pic",COLOR_BLACK);} }
    if(iMinLed<=6 && iMaxLed>=6)  {if(iLeds>=6){myNex.writeNum("p7.pic",COLOR_YELLOW);} else {myNex.writeNum("p7.pic",COLOR_BLACK);} }
    if(iMinLed<=7 && iMaxLed>=7)  {if(iLeds>=7){myNex.writeNum("p8.pic",COLOR_YELLOW);} else {myNex.writeNum("p8.pic",COLOR_BLACK);} }
    if(iMinLed<=8 && iMaxLed>=8)  {if(iLeds>=8){myNex.writeNum("p9.pic",COLOR_ORANGE);} else {myNex.writeNum("p9.pic",COLOR_BLACK);} }
  }

  iLastLed = iLeds;
  
   
}


void DisplayFlag(int iFlag)
{
  stLast.Flag = iFlag;
  // define color
  uint32_t iColor = 0; // empty flag
  switch (iFlag)
  {     
   case 1:
   {
      iColor = 31;      
   break;
   }
   case 2:
    { 
    iColor = 65504;
   break; 
    }    
    case 4:
    {
    iColor = 65535;
   break;}
    case 0:
    case 3:
    case 5:
    case 6:
    {
    iColor = 0;
   break;
   }
   case 7: 
   {
    iColor = 2016;
   break;
   }
    case 8: 
    {
    iColor = 64800;
   break;
    }
  default:
    break;
  } 
  myNex.writeNum("flag.bco", iColor);
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
