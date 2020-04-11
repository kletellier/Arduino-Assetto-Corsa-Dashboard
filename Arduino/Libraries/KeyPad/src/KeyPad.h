#ifndef KeyPad_h
#define KeyPad_h

#include "Arduino.h"
#include "LiquidCrystal.h"

typedef enum {
    ButtonNone = 0,
    ButtonRight = 1,
    ButtonUp = 2,
    ButtonDown = 3,
    ButtonLeft = 4,
    ButtonSelect = 5
} LCDButton;


class KeyPad
{
  public:
    KeyPad(int pin,long trigger,LiquidCrystal* lcd);
    LCDButton GetButton();
    void Clear();
    void SetCursor(int x,int y);
    void DrawProgressBar(int y, byte percent);
    void Display(int x,int y, String message);
    void SpecialChar(int x,int y,char charac);
    void ClearLine(int y);
    void ChangeBackLight(int value);
  private:
    int _pin;
    long _triggerPressed;
    unsigned long _startPressed;
    LiquidCrystal* _lcd;
    LCDButton GetState();
    LCDButton _lastPressed;
    void InitLcd();
    void RegisterSpecialCharacter();
};

#endif