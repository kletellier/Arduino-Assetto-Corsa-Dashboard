#include "Arduino.h"
#include "KeyPad.h"
#include "LiquidCrystal.h"

KeyPad::KeyPad(int pin,long trigger, LiquidCrystal* lcd)
{   
  _pin = pin;
  _triggerPressed = trigger;
  _lcd = lcd; 
  _lastPressed = ButtonNone;
  InitLcd();
}

LCDButton KeyPad::GetButton()
{
    LCDButton pState = GetState();
    unsigned long pTimer = millis();

    if(pState==ButtonNone)
    {
      if(_lastPressed!=ButtonNone)
      {
        if(pTimer - _startPressed >= _triggerPressed)
        {
          LCDButton returnKey = _lastPressed;
          _lastPressed = ButtonNone;
          return returnKey;
        }
        else
        {
           _lastPressed = ButtonNone;
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
       if(_lastPressed != pState)
       {
          _startPressed = millis();
          _lastPressed = pState;
       } 
       return ButtonNone;       
    }    
}

LCDButton KeyPad::GetState()
{
  int x;
  x = analogRead(_pin);
  if (x < 60) {
     return LCDButton::ButtonRight;
  }
  else if (x < 200) {
    return LCDButton::ButtonUp;
  }
  else if (x < 400) {
    return LCDButton::ButtonDown;
  }
  else if (x < 600) {
    return LCDButton::ButtonLeft;
  }
  else if (x < 800) {
    return LCDButton::ButtonSelect;
  }
    return LCDButton::ButtonNone;
}

void KeyPad::InitLcd()
{  
  _lcd->begin(16,2);
  RegisterSpecialCharacter();
}

void KeyPad::SetCursor(int x,int y)
{
  _lcd->setCursor(x,y);
}

void KeyPad::Clear()
{
  _lcd->clear();
}

void KeyPad::ChangeBackLight(int value)
{
  analogWrite(10,value);
}

void KeyPad::Display(int x,int y ,String message)
{
  _lcd->setCursor(x,y);
  _lcd->print(message);
}

void KeyPad::SpecialChar(int x,int y,char charac)
{
  _lcd->setCursor(x,y);
  _lcd->print(charac);
}

void KeyPad::RegisterSpecialCharacter()
{
  
 
byte DIV_0_OF_5[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};  

byte DIV_1_OF_5[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};  

byte DIV_2_OF_5[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};  

byte DIV_3_OF_5[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};  

byte DIV_4_OF_5[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};  

byte DIV_5_OF_5[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};  
 

 byte percent_char[] = {
  B11000,
  B11001,
  B00010,
  B00110,
  B00100,
  B01000,
  B01011,
  B10011
};

byte degree_char[] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000
};

   
  _lcd->createChar(0, DIV_0_OF_5);
  _lcd->createChar(1, DIV_1_OF_5);
  _lcd->createChar(2, DIV_2_OF_5);
  _lcd->createChar(3, DIV_3_OF_5);
  _lcd->createChar(4, DIV_4_OF_5);
  _lcd->createChar(5, DIV_5_OF_5);
  _lcd->createChar(6, degree_char); 
  _lcd->createChar(7, percent_char); 
}

void KeyPad::ClearLine(int y)
{
  SetCursor(0, y);
  _lcd->print("                ");
  SetCursor(0, y);
}

/* https://www.carnetdumaker.net/articles/faire-une-barre-de-progression-avec-arduino-et-liquidcrystal/ */
void KeyPad::DrawProgressBar(int y,byte percent)
{
   SetCursor(0, y);   
  byte nb_columns = map(percent, 0, 100, 0, 16 * 5);
 
  for (byte i = 0; i < 16; ++i) {
 
    if (nb_columns == 0) { 
      _lcd->write((byte) 0);

    } else if (nb_columns >= 5) {  
      _lcd->write(5);
      nb_columns -= 5;

    } else {  
      _lcd->write(nb_columns);
      nb_columns = 0;
    }
  }
}