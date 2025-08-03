#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "SensorDataManager.h"
#include <WiFi.h>
#include <PubSubClient.h>

// 硬件引脚定义
#define TFT_CS    5
#define TFT_RST   4  
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_SCLK  18

#define EC11_A    12
#define EC11_B    14
#define EC11_SW   27
#define BTN_BACK  26

// 屏幕尺寸
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

// 颜色定义
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_GRAY      0x8410

// UI状态枚举
enum UIState {
    STATE_OVERVIEW,     // 概览页
    STATE_BROWSE,       // 房间浏览模式
    STATE_EDIT,         // 房间编辑模式
    STATE_SETTINGS      // 系统设置页
};

// 传感器项目枚举
enum SensorItem {
    ITEM_TEMPERATURE = 0,   // 温度
    ITEM_HUMIDITY = 1       // 湿度
};

// 房间名称映射
const char* ROOM_NAMES[] = {"客厅", "卧室", "厨房", "浴室", "室外"};

class UIController {
private:
    Adafruit_ST7735 tft;
    
    // UI状态变量
    UIState currentState;
    int selectedRoom;
    SensorItem selectedItem;
    bool editMode;
    
    // 旋钮状态
    volatile bool encoderPressed;
    volatile bool backButtonPressed;
    volatile int encoderDirection;
    int lastEncoderA;
    
    // 显示刷新
    unsigned long lastUpdate;
    bool needRedraw;
    
    // 闪烁控制
    unsigned long blinkTimer;
    bool blinkState;

public:
    UIController();
    void begin();
    void update();
    void handleInput();
    
    // 中断处理函数（需要设为static并绑定实例）
    void IRAM_ATTR handleEncoderInterrupt();
    void IRAM_ATTR handleEncoderSwitchInterrupt();
    void IRAM_ATTR handleBackButtonInterrupt();

private:
    // 页面绘制函数
    void drawOverviewPage();
    void drawRoomPage();
    void drawSettingsPage();
    void drawHeader(const char* title);
    void drawProgressBar(int x, int y, int width, int height, float value, float maxValue);
    void drawWiFiIcon(int x, int y);
    void drawMQTTIcon(int x, int y);
    
    // 输入处理
    void handleEncoderRotation(int direction);
    void handleEncoderPress();
    void handleBackButton();
    
    // 数据调节
    void adjustSensorValue(int direction);
    
    // 工具函数
    void setRedraw() { needRedraw = true; }
    const char* getRoomName(int index);
    SensorData getCurrentRoomData();
    void updateBlinkState();
};

// 全局实例（用于中断处理）
extern UIController* g_uiController;

#endif // UI_CONTROLLER_H