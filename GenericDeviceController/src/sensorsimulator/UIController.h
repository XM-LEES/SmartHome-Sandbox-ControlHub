#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include <Arduino.h>
#include "../Config.h"
#include "SensorDataManager.h"

// UI相关库（UIController只在传感器模拟器启用时被包含，无需条件判断）
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <U8g2_for_Adafruit_GFX.h>

// UI硬件引脚定义（内置在UIController中）
// TFT屏幕引脚 (ST7735s)
#define TFT_SCLK  18   // SCL - SPI时钟线 (D18, VSPI_CLK原生)
#define TFT_MOSI  23   // SDA - SPI数据线 (D23, VSPI_MOSI原生)
#define TFT_RST   15    // RES - 复位 (D15)
#define TFT_DC    2    // DC  - 数据/命令选择 (D2)
#define TFT_CS    5    // CS  - 片选 (D5, VSPI_CS原生)
#define TFT_BLK   4   // BLK - 背光控制 (D4)

// EC11旋钮编码器引脚
#define EC11_A    14   // A   - 编码器A相
#define EC11_B    27   // B   - 编码器B相
#define EC11_SW   26   // PUSH- 编码器按键

// 独立按键引脚
#define BTN_OK    25   // KO  - OK确认按键

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

// UI状态枚举（总是需要，用于接口）
enum UIState {
    STATE_OVERVIEW,     // 概览页
    STATE_BROWSE,       // 房间浏览模式
    STATE_EDIT,         // 房间编辑模式
    STATE_SETTINGS      // 系统设置页
};

// 传感器项目枚举（总是需要，用于接口）
enum SensorItem {
    ITEM_TEMPERATURE = 0,   // 温度
    ITEM_HUMIDITY = 1       // 湿度
};

// 房间名称映射（声明）
extern const char* ROOM_NAMES[];

// UI控制器实现
class UIController {
private:
    Adafruit_ST7735 tft;
    U8G2_FOR_ADAFRUIT_GFX u8g2;
    
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
    int encoderStepAccumulator;  // 步长累积器：两个咔嗒对应一个步长
    unsigned long lastEncoderTime;  // 上次编码器活动时间
    
    // 按键状态记录（用于CHANGE中断的电平检测）
    bool lastEncoderSwitchState;  // 编码器按键上次状态
    bool lastBackButtonState;     // 返回按键上次状态
    
    // 显示刷新
    unsigned long lastUpdate;
    bool needRedraw;
    


public:
    UIController();
    void begin();
    void update();
    void handleInput();
    
    // 中断处理函数（需要设为static并绑定实例）
    void IRAM_ATTR handleEncoderInterrupt();
    void IRAM_ATTR handleEncoderSwitchInterrupt();
    void IRAM_ATTR handleBackButtonInterrupt();

    // 工具函数
    void setRedraw() { needRedraw = true; }

private:
    // 页面绘制函数
    void drawOverviewPage();
    void drawRoomPage();
    void drawSettingsPage();
    void drawHeader(const char* title);
    void drawProgressBar(int x, int y, int width, int height, float value, float maxValue);
    void drawWiFiIcon(int x, int y);
    void drawMQTTIcon(int x, int y);
    void displayStartupScreen();
    
    // U8g2中文显示辅助函数
    void printChinese(int x, int y, const char* text, uint16_t color);
    void printChineseSmall(int x, int y, const char* text, uint16_t color);
    
    // 输入处理
    void handleEncoderRotation(int direction);
    void handleEncoderPress();
    void handleBackButton();
    
    // 数据调节
    void adjustSensorValue(int direction);
    
    // 工具函数
    const char* getRoomName(int index);
    SensorData getCurrentRoomData();

};

// 全局实例（用于中断处理）
extern UIController* g_uiController;

#endif // UI_CONTROLLER_H