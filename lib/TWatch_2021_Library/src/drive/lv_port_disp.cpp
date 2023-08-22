// #include "Input.h"
#include "./TWatch_hal.h"

#if CONFIG_TWATCH_APP_LVGL && defined(CONFIG_TWATCH_HAS_DISPLAY)

lv_disp_drv_t *disp_drv_p;

void TWatchClass::lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  lv_disp_flush_ready(disp);
  TFT_eSPI *screen = (TFT_eSPI *)disp->user_data;
  // if( !framebuffer_drawing )
  //         lv_disp_flush_ready( disp_drv );

  // get buffer sizs
  uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
  /**
   * start data trnsmission
   * set the working window
   * and start DMA transfer if enabled
   * stop transmission
   */
  screen->startWrite();
  screen->pushImageDMA(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1), (uint16_t *)color_p);
  screen->endWrite();
}
#endif

void display_wait_cb(_lv_disp_drv_t *disp_drv) {
  delay(1);
}

void TWatchClass::lv_port_disp_init(SCREEN_CLASS *scr) {
  static lv_disp_draw_buf_t draw_buf;

  tft->init();
  //tft->fillScreen(TFT_BLACK);
  tft->initDMA();

  // lv_disp_buf_p = (lv_color_t*)calloc( sizeof(lv_color_t), DISP_BUF_SIZE, MALLOC_CAP_DMA);
  // lv_disp_buf_p2 = (lv_color_t*)calloc( sizeof(lv_color_t), DISP_BUF_SIZE, MALLOC_CAP_DMA);

  if (lv_disp_buf_p == nullptr) {
    lv_disp_buf_p = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * DISP_BUF_SIZE, MALLOC_CAP_DMA);
    //lv_disp_buf_p2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * DISP_BUF_SIZE, MALLOC_CAP_DMA);
  }

   lv_disp_draw_buf_init(&draw_buf, lv_disp_buf_p, NULL, DISP_BUF_SIZE );
  // lv_disp_draw_buf_init(&draw_buf, lv_disp_buf_p, lv_disp_buf_p2, DISP_BUF_SIZE);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = lv_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.wait_cb = display_wait_cb;
  disp_drv.user_data = scr;
  disp_drv.sw_rotate = 0;
  disp_drv.rotated = LV_DISP_ROT_NONE;

  lv_disp_drv_register(&disp_drv);
}
