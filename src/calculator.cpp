#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"
#include "ArduinoLog.h"

#include "calculator.h"

void CalcHandle()
{
    if (lv_scr_act() == ui_Calculator)
    {
        const char *calcbutton = lv_btnmatrix_get_btn_text(ui_Calculator_Keyboard, lv_btnmatrix_get_selected_btn(ui_Calculator_Keyboard));
        if (calcbutton == "C")
        {
            lv_textarea_set_text(ui_Calculator_textarea, "");
            Log.verboseln("Calc C pressed %i",(int)calcbutton);
        }

        if (calcbutton == "=")
        {
            String expr = lv_textarea_get_text(ui_Calculator_textarea);
            lv_textarea_set_text(ui_Calculator_textarea, String(calculateExpression(expr)).c_str());
        }
    }
}

int calculateExpression(String expression) {
  int num1 = 0;
  int num2 = 0;
  char operatorChar = '+';
  int result = 0;

  sscanf(expression.c_str(), "%d%c%d", &num1, &operatorChar, &num2);

  switch (operatorChar) {
    case '+':
      result = num1 + num2;
      break;
    case '-':
      result = num1 - num2;
      break;
    case 'x':
      result = num1 * num2;
      break;
    case '/':
      if (num2 != 0)
        result = num1 / num2;
      else {
        Serial.println("Error: Division by zero");
        return 0;
      }
      break;
    default:
      Serial.println("Error: Invalid operator");
      return 0;
  }

  return result;
}