#include "lvgl.h"
#include "ui.h"
#include "Arduino.h"
#include "ArduinoLog.h"

#include "calculator.h"

void CalcHandle(lv_event_t *e)
{
  if (lv_scr_act() == ui_Calculator)
  {
    const char *calcbutton = lv_btnmatrix_get_btn_text(ui_Calculator_Keyboard, lv_btnmatrix_get_selected_btn(ui_Calculator_Keyboard));
    if (calcbutton == "C")
    {
      lv_textarea_set_text(ui_Calculator_textarea, "");
      Log.verboseln("Calc C pressed %i", (int)calcbutton);
      lv_btnmatrix_set_selected_btn(ui_Calculator_Keyboard, 16);
    }

    if (calcbutton == "=")
    {
      String expr = lv_textarea_get_text(ui_Calculator_textarea);
      char buffer[64];
      if (CalculateExpression(expr) == NULL)
        sprintf(buffer, "Undefined");
      else
        sprintf(buffer, "%1.6g", CalculateExpression(expr)); // 0.000000

      Log.verboseln("Calculating Expression: %s %s", expr, buffer);
      lv_textarea_set_text(ui_Calculator_textarea, buffer);
      lv_btnmatrix_set_selected_btn(ui_Calculator_Keyboard, 16);
    }
  }
}

float CalculateExpression(String expression)
{
  float num1 = 0;
  float num2 = 0;
  char operatorChar = '+';
  float result = 0;

  sscanf(expression.c_str(), "%f%c%f", &num1, &operatorChar, &num2);

  switch (operatorChar)
  {
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
    else
    {
      Log.verboseln("Error: Division by zero");
      return NULL;
    }
    break;
  default:
    Log.verboseln("Error: Invalid operator");
    return num1;
  }

  return result;
}