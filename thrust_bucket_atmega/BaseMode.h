#include "rgb_lcd.h"
#include "HX711.h"
#include "BaseModeFunction.h"

#ifndef BASE_MODE
#define BASE_MODE

class BaseMode {

protected:
  void showFunctionName();

  BaseModeFunction **modeFunctions;
  int FUNCTION_COUNT;
  int function_index;

  HX711 *scale;
  rgb_lcd *lcd;
  

public:
  BaseMode(HX711 *_scale, rgb_lcd *_lcd);
  virtual void startMode();
  virtual void updateMode() = 0;
  void handleWheelRotation(int wheelRotation);
  void handleButtonDown();
  void handleButtonUp();
};

#endif
