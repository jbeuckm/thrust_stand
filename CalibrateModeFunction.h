#include "BaseModeFunction.h"

class CalibrateModeFunction : public BaseModeFunction {
public:
  CalibrateModeFunction(HX711 _scale, rgb_lcd _lcd);
  char * getLabel();

  void handleWheelRotation(int wheelRotation);
  void handleButtonDown();
  void handleButtonUp();
};

