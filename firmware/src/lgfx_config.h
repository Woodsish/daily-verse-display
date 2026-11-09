// lgfx_config.h
#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Light_PWM _light;

    public:
        LGFX() {
            {
                auto cfg = _bus.config();
                cfg.spi_host        = SPI2_HOST;
                cfg.spi_mode        = 0;
                cfg.freq_write      = 40000000;
                cfg.freq_read       = 16000000;
                cfg.dma_channel     = SPI_DMA_CH_AUTO;
                cfg.pin_sclk        = 7;    // D8
                cfg.pin_mosi        = 9;    // D10
                cfg.pin_miso        = -1;   // not wired
                cfg.pin_dc          = 2;    // D1
                _bus.config(cfg);
                _panel.setBus(&_bus);
            }
            {
                auto cfg = _panel.config();
                cfg.pin_cs          = 1;    // D0
                cfg.pin_rst         = 4;    // D3
                cfg.pin_busy        = -1;
                cfg.panel_width     = 320;
                cfg.panel_height    = 480;
                cfg.offset_x        = 0;
                cfg.offset_y        = 0;
                cfg.readable        = false;
                cfg.invert          = false;    // flip if colors look inverted
                cfg.rgb_order       = false;    // flip if red and blue are swapped
                cfg.bus_shared      = false;
                _panel.config(cfg);
            }
            {
                auto cfg = _light.config();
                cfg.pin_bl          = 5;    // D4
                cfg.invert          = false;
                cfg.freq            = 44100;
                cfg.pwm_channel     = 7;
                _light.config(cfg);
                _panel.setLight(&_light);
            }
            setPanel(&_panel);
}
};