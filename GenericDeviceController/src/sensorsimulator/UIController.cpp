#include "UIController.h"

// 全局实例指针
UIController* g_uiController = nullptr;

// 房间名称映射（定义）
const char* ROOM_NAMES[] = {"客厅", "卧室", "厨房", "浴室", "室外"};

// 中断服务程序包装函数
void IRAM_ATTR encoderISR() {
    if (g_uiController) g_uiController->handleEncoderInterrupt();
}

void IRAM_ATTR encoderSwitchISR() {
    if (g_uiController) g_uiController->handleEncoderSwitchInterrupt();
}

void IRAM_ATTR backButtonISR() {
    if (g_uiController) g_uiController->handleBackButtonInterrupt();
}

UIController::UIController() 
    : tft(TFT_CS, TFT_DC, TFT_RST),
      currentState(STATE_OVERVIEW),
      selectedRoom(0),
      selectedItem(ITEM_TEMPERATURE),
      editMode(false),
      encoderPressed(false),
      backButtonPressed(false),
      encoderDirection(0),
      lastEncoderA(HIGH),
      encoderStepAccumulator(0),
      lastEncoderTime(0),
      lastEncoderSwitchState(HIGH),
      lastBackButtonState(HIGH),
      lastUpdate(0),
      needRedraw(true) {
    g_uiController = this;
}

void UIController::begin() {
    // 初始化背光控制
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH); // 打开背光
    
    // 初始化SPI和TFT
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(0); // 竖屏模式
    tft.fillScreen(COLOR_BLACK);
    
    // 初始化U8g2库，用于中文显示
    u8g2.begin(tft);
    u8g2.setFontMode(1);  // 透明背景
    u8g2.setFontDirection(0);  // 从左到右
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);  // 设置中文字体
    
    // 初始化GPIO引脚
    pinMode(EC11_A, INPUT_PULLUP);
    pinMode(EC11_B, INPUT_PULLUP);
    pinMode(EC11_SW, INPUT_PULLUP);
    pinMode(BTN_OK, INPUT_PULLUP);
    
    // 读取初始编码器状态
    lastEncoderA = digitalRead(EC11_A);
    
    // 设置中断
    attachInterrupt(digitalPinToInterrupt(EC11_A), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(EC11_SW), encoderSwitchISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BTN_OK), backButtonISR, CHANGE);
    
    // 显示启动界面
    displayStartupScreen();
    
    delay(2000);
}

void UIController::update() {
    unsigned long currentTime = millis();
    
    handleInput();
    
    // 设置页定时刷新，反映网络状态变化
    if (currentState == STATE_SETTINGS) {
        static unsigned long lastSettingsRefresh = 0;
        if (currentTime - lastSettingsRefresh >= 1000) {
            setRedraw();
            lastSettingsRefresh = currentTime;
        }
    }
    
    if (needRedraw) {
        switch (currentState) {
            case STATE_OVERVIEW:
                drawOverviewPage();
                break;
            case STATE_BROWSE:
            case STATE_EDIT:
                drawRoomPage();
                break;
            case STATE_SETTINGS:
                drawSettingsPage();
                break;
        }
        needRedraw = false;
        lastUpdate = currentTime;
        // Serial.println("[UI] 屏幕已刷新");
    }
}

void UIController::handleInput() {
    unsigned long currentTime = millis();
    
    // 处理编码器旋转 - 两个咔嗒对应一个步长
    if (encoderDirection != 0) {
        encoderStepAccumulator += encoderDirection;
        lastEncoderTime = currentTime;
        encoderDirection = 0;
        
        // 当累积器达到±2时，执行一次旋转操作
        if (abs(encoderStepAccumulator) >= 2) {
            int logicalDirection = (encoderStepAccumulator > 0) ? 1 : -1;
            handleEncoderRotation(logicalDirection);
            encoderStepAccumulator = 0;  // 重置累积器
            // Serial.print("[UI] 编码器累积步长达到，执行旋转: ");
            // Serial.println(logicalDirection > 0 ? "正向" : "反向");
        }
    }
    
    // 超时重置累积器
    if (encoderStepAccumulator != 0 && (currentTime - lastEncoderTime) > 500) {
        encoderStepAccumulator = 0;
    }
    
    if (encoderPressed) {
        encoderPressed = false;
        // Serial.println("[UI] 编码器按键触发");
        handleEncoderPress();
        delay(200);
    }
    
    // 处理返回按键
    if (backButtonPressed) {
        backButtonPressed = false;
        // Serial.println("[UI] 返回按键触发");
        handleBackButton();
        delay(200);
    }
}

void UIController::handleEncoderRotation(int direction) {
    switch (currentState) {
        case STATE_OVERVIEW:
            // 概览页：选择房间
            selectedRoom += direction;
            if (selectedRoom < 0) selectedRoom = MAX_ROOMS - 1;
            if (selectedRoom >= MAX_ROOMS) selectedRoom = 0;
            setRedraw();
            break;
            
        case STATE_BROWSE:
            // 浏览模式：选择传感器项目
            selectedItem = (selectedItem == ITEM_TEMPERATURE) ? ITEM_HUMIDITY : ITEM_TEMPERATURE;
            Serial.print("[UI] 浏览模式切换到: ");
            Serial.println(selectedItem == ITEM_TEMPERATURE ? "温度" : "湿度");
            setRedraw();
            break;
            
        case STATE_EDIT:
            // 编辑模式：调节数值
            adjustSensorValue(direction);
            setRedraw();
            break;
            
        case STATE_SETTINGS:
            // 设置页面暂时无操作
            break;
    }
}

void UIController::handleEncoderPress() {
    switch (currentState) {
        case STATE_OVERVIEW:
            // 概览页：进入选中房间的浏览模式
            currentState = STATE_BROWSE;
            selectedItem = ITEM_TEMPERATURE;
            editMode = false;
            Serial.print("[UI] 进入房间浏览模式，初始选择: ");
            Serial.println(selectedItem == ITEM_TEMPERATURE ? "温度" : "湿度");
            setRedraw();
            break;
            
        case STATE_BROWSE:
            // 浏览模式：进入编辑模式
            currentState = STATE_EDIT;
            editMode = true;
            Serial.print("[UI] 进入编辑模式，编辑项目: ");
            Serial.println(selectedItem == ITEM_TEMPERATURE ? "温度" : "湿度");
            setRedraw();
            break;
            
        case STATE_EDIT:
            // 编辑模式：退出编辑，回到浏览模式
            currentState = STATE_BROWSE;
            editMode = false;
            Serial.print("[UI] 退出编辑模式，当前选择: ");
            Serial.println(selectedItem == ITEM_TEMPERATURE ? "温度" : "湿度");
            setRedraw();
            break;
            
        case STATE_SETTINGS:
            // 设置页面暂时无操作
            break;
    }
}

void UIController::handleBackButton() {
    switch (currentState) {
        case STATE_OVERVIEW:
            // 概览页：进入系统信息页
            currentState = STATE_SETTINGS;
            setRedraw();
            break;
            
        case STATE_BROWSE:
        case STATE_EDIT:
            // 房间页面：返回概览页
            currentState = STATE_OVERVIEW;
            editMode = false;
            setRedraw();
            break;
            
        case STATE_SETTINGS:
            // 设置页面：返回概览页
            currentState = STATE_OVERVIEW;
            setRedraw();
            break;
    }
}

void UIController::adjustSensorValue(int direction) {
    SensorData data = getSensorData((RoomIndex)selectedRoom);
    
    if (selectedItem == ITEM_TEMPERATURE) {
        data.temperature += direction * 0.5f;
        data.temperature = constrain(data.temperature, -10.0f, 40.0f);
    } else {
        data.humidity += direction * 0.5f;
        data.humidity = constrain(data.humidity, 0.0f, 100.0f);
    }
    
    setSensorData((RoomIndex)selectedRoom, data.temperature, data.humidity);
}

void UIController::drawOverviewPage() {
    tft.fillScreen(COLOR_BLACK);
    
    // 绘制标题
    drawHeader("环境监控");
    
    // 绘制列标题
    int y = 25;
    tft.setTextColor(COLOR_GRAY);
    tft.setTextSize(1);
    printChineseSmall(8, y + 8, "房间", COLOR_GRAY);
    printChineseSmall(50, y + 8, "温度", COLOR_GRAY);
    printChineseSmall(95, y + 8, "湿度", COLOR_GRAY);
    
    y += 15;
    
    // 绘制所有房间数据
    for (int i = 0; i < MAX_ROOMS; i++) {
        SensorData data = getSensorData((RoomIndex)i);
        
        // 选中房间用不同颜色显示
        uint16_t textColor = (i == selectedRoom) ? COLOR_YELLOW : COLOR_WHITE;
        tft.setTextColor(textColor);
        tft.setTextSize(1);
        
        // 显示选中指示符
        if (i == selectedRoom) {
            tft.setCursor(0, y);
            tft.print(">");
        }
        
        // 房间名（中文）
        printChineseSmall(8, y + 8, getRoomName(i), textColor);
        
        tft.setTextColor(textColor);
        
        // 温度数据
        tft.setCursor(45, y);
        tft.print(data.temperature, 1);
        tft.print("C");
        
        // 湿度数据
        tft.setCursor(90, y);
        tft.print(data.humidity, 1);
        tft.print("%");
        
        y += 20;
    }
    
    // 底部操作提示
    int bottomY = SCREEN_HEIGHT - 20;
    
    // 左下角：系统信息
    printChineseSmall(0, bottomY + 18, "系统信息", COLOR_GREEN);
    
    // 右下角：转动选择 按下确认
    printChineseSmall(SCREEN_WIDTH - 50, bottomY + 8, "转动选择", COLOR_GREEN);
    printChineseSmall(SCREEN_WIDTH - 50, bottomY + 18, "按下确认", COLOR_GREEN);
}

void UIController::drawRoomPage() {
    tft.fillScreen(COLOR_BLACK);
    
    // 绘制标题
    String title = getRoomName(selectedRoom);
    drawHeader(title.c_str());
    
    SensorData data = getSensorData((RoomIndex)selectedRoom);
    
    int y = 30;
    
    // 温度行
    bool tempSelected = (selectedItem == ITEM_TEMPERATURE);
    bool tempEditing = (currentState == STATE_EDIT && tempSelected);
    
    uint16_t tempColor = COLOR_WHITE;
    if (tempSelected) {
        tempColor = tempEditing ? COLOR_RED : COLOR_YELLOW;
    }
    
    tft.setTextColor(tempColor);
    tft.setTextSize(1);
    
    if (tempSelected) {
        tft.setCursor(0, y);
        tft.print(">");
    }
    
    // 使用U8g2显示中文
    u8g2.setForegroundColor(tempColor);
    u8g2.setCursor(8, y + 8);
    u8g2.print("温度:");
    
    // 使用原生库显示数字
    tft.setTextColor(tempColor);
    tft.setCursor(65, y);
    tft.print(data.temperature, 1);
    tft.print("C");
    
    y += 12;
    
    // 温度进度条 (范围: -10°C ~ 40°C，总共50°C)
    // 将温度值映射到0-50范围用于进度条显示
    float tempForProgress = data.temperature + 10.0f;  // 将-10~40映射到0~50
    drawProgressBar(8, y, 100, 6, tempForProgress, 50.0f);
    
    y += 12;
    
    // 湿度行
    bool humSelected = (selectedItem == ITEM_HUMIDITY);
    bool humEditing = (currentState == STATE_EDIT && humSelected);
    
    uint16_t humColor = COLOR_WHITE;
    if (humSelected) {
        humColor = humEditing ? COLOR_BLUE : COLOR_CYAN;
    }
    
    tft.setTextColor(humColor);
    
    if (humSelected) {
        tft.setCursor(0, y);
        tft.print(">");
    }
    
    // 使用U8g2显示中文
    u8g2.setForegroundColor(humColor);
    u8g2.setCursor(8, y + 8);
    u8g2.print("湿度:");
    
    // 使用原生库显示数字
    tft.setTextColor(humColor);
    tft.setCursor(65, y);
    tft.print(data.humidity, 1);
    tft.print("%");
    
    y += 12;
    
    // 湿度进度条
    drawProgressBar(8, y, 100, 6, data.humidity, 100.0f);
    
    y += 20;
    
    // 底部操作提示
    int bottomY = SCREEN_HEIGHT - 20;
    
    // 左下角：返回
    printChineseSmall(0, bottomY + 18, "返回", COLOR_GREEN);
    
    // 右下角：根据状态显示不同提示
    if (currentState == STATE_BROWSE) {
        printChineseSmall(SCREEN_WIDTH - 50, bottomY + 8, "转动选择", COLOR_GREEN);
        printChineseSmall(SCREEN_WIDTH - 50, bottomY + 18, "按下编辑", COLOR_GREEN);
    } else {
        printChineseSmall(SCREEN_WIDTH - 50, bottomY + 8, "转动调节", COLOR_GREEN);
        printChineseSmall(SCREEN_WIDTH - 50, bottomY + 18, "按下确认", COLOR_GREEN);
    }
}

void UIController::drawSettingsPage() {
    tft.fillScreen(COLOR_BLACK);
    
    drawHeader("系统信息");
    
    int y = 30;
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    
    // WiFi状态
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(0, y);
    tft.print("WiFi:");
    if (WiFi.isConnected()) {
        printChineseSmall(35, y + 8, "已连接", COLOR_GREEN);
        y += 12;
        tft.setCursor(0, y);
        tft.print("   ");
        tft.print(WiFi.SSID());
        y += 12;
        tft.setCursor(0, y);
        tft.print("   ");
        tft.print(WiFi.localIP());
    } else {
        printChineseSmall(35, y + 8, "未连接", COLOR_RED);
    }
    
    y += 20;
    
    // MQTT状态
    extern PubSubClient client; // 引用外部MQTT客户端
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(0, y);
    tft.print("MQTT:");
    if (client.connected()) {
        printChineseSmall(40, y + 8, "在线", COLOR_GREEN);
    } else {
        printChineseSmall(40, y + 8, "离线", COLOR_RED);
    }
    
    // 底部操作提示
    int bottomY = SCREEN_HEIGHT - 20;
    
    // 左下角：返回
    printChineseSmall(0, bottomY + 18, "返回", COLOR_GREEN);
    
}

void UIController::drawHeader(const char* title) {
    // 计算中文字符串的显示宽度
    int titleWidth = 0;
    const char* p = title;
    while (*p) {
        if ((*p & 0x80) == 0) {
            // ASCII字符
            titleWidth += 6;
            p++;
        } else {
            // 中文字符（UTF-8编码）
            titleWidth += 12;  // U8g2中文字体宽度约12像素
            // 跳过UTF-8多字节序列
            if ((*p & 0xE0) == 0xC0) p += 2;      // 2字节
            else if ((*p & 0xF0) == 0xE0) p += 3; // 3字节（中文）
            else if ((*p & 0xF8) == 0xF0) p += 4; // 4字节
            else p++;
        }
    }
    
    // 居中显示
    int x = (SCREEN_WIDTH - titleWidth) / 2;
    printChinese(x, 15, title, COLOR_WHITE);
    
    // 绘制分隔线
    tft.drawLine(0, 18, SCREEN_WIDTH, 18, COLOR_GRAY);
}

// U8g2中文显示辅助函数
void UIController::printChinese(int x, int y, const char* text, uint16_t color) {
    u8g2.setForegroundColor(color);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(x, y);
    u8g2.print(text);
}

void UIController::printChineseSmall(int x, int y, const char* text, uint16_t color) {
    u8g2.setForegroundColor(color);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);  // 可以换成更小的字体
    u8g2.setCursor(x, y);
    u8g2.print(text);
}

void UIController::drawProgressBar(int x, int y, int width, int height, float value, float maxValue) {
    // 背景
    tft.drawRect(x, y, width, height, COLOR_GRAY);
    
    // 填充
    int fillWidth = (int)(width * value / maxValue);
    if (fillWidth > 0) {
        tft.fillRect(x + 1, y + 1, fillWidth - 1, height - 2, COLOR_GREEN);
    }
}

void UIController::drawWiFiIcon(int x, int y) {
    if (WiFi.isConnected()) {
        tft.setTextColor(COLOR_GREEN);
        tft.setCursor(x, y);
        tft.print("W");
    } else {
        tft.setTextColor(COLOR_RED);
        tft.setCursor(x, y);
        tft.print("W");
    }
}

void UIController::drawMQTTIcon(int x, int y) {
    extern PubSubClient client;
    if (client.connected()) {
        tft.setTextColor(COLOR_GREEN);
        tft.setCursor(x, y);
        tft.print("M");
    } else {
        tft.setTextColor(COLOR_RED);
        tft.setCursor(x, y);
        tft.print("M");
    }
}

const char* UIController::getRoomName(int index) {
    if (index >= 0 && index < MAX_ROOMS) {
        return ROOM_NAMES[index];
    }
    return "未知";
}

SensorData UIController::getCurrentRoomData() {
    return getSensorData((RoomIndex)selectedRoom);
}



// 中断处理函数实现
void IRAM_ATTR UIController::handleEncoderInterrupt() {
    int currentA = digitalRead(EC11_A);
    int currentB = digitalRead(EC11_B);
    
    if (currentA != lastEncoderA) {
        if (currentA == currentB) {
            encoderDirection = -1; // 逆时针
        } else {
            encoderDirection = 1; // 顺时针
        }
        lastEncoderA = currentA;
    }
}

void IRAM_ATTR UIController::handleEncoderSwitchInterrupt() {
    static unsigned long lastChange = 0;
    unsigned long now = millis();
    
    // 基本防抖：忽略80ms内的重复变化
    if (now - lastChange < 80) {
        return;
    }
    
    bool currentState = digitalRead(EC11_SW);
    
    // 检测从高到低的变化（按下事件）
    if (lastEncoderSwitchState == HIGH && currentState == LOW) {
        encoderPressed = true;
        lastChange = now;
    }
    
    // 更新状态（无论是按下还是松开）
    lastEncoderSwitchState = currentState;
}

void IRAM_ATTR UIController::handleBackButtonInterrupt() {
    static unsigned long lastChange = 0;
    unsigned long now = millis();
    
    // 基本防抖：忽略50ms内的重复变化
    if (now - lastChange < 50) {
        return;
    }
    
    bool currentState = digitalRead(BTN_OK);
    
    // 检测从高到低的变化（按下事件）
    if (lastBackButtonState == HIGH && currentState == LOW) {
        backButtonPressed = true;
        lastChange = now;
    }
    
    // 更新状态（无论是按下还是松开）
    lastBackButtonState = currentState;
}

void UIController::displayStartupScreen() {
    tft.fillScreen(COLOR_BLACK);

    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(2);

    int title1Width = strlen("SmartHome") * 6 * 2;
    int title1X = (SCREEN_WIDTH - title1Width) / 2;
    tft.setCursor(title1X, 35);
    tft.println("SmartHome");
    
    tft.setTextSize(1);
    int title2Width = strlen("Sensor Simulator") * 6 * 1;
    int title2X = (SCREEN_WIDTH - title2Width) / 2;
    tft.setCursor(title2X, 55);
    tft.println("Sensor Simulator");

    // 节点信息（居中显示）
    printChineseSmall((SCREEN_WIDTH - 5*12)/2, 100, "Node2-节点", COLOR_CYAN);
    
    // 状态信息（居中显示）
    printChineseSmall((SCREEN_WIDTH - 6*12)/2, 130, "正在初始化...", COLOR_YELLOW);
    
    // 版本信息（居中显示）
    tft.setTextColor(COLOR_GRAY);
    tft.setTextSize(1);
    int versionWidth = strlen("v1.0  2024.01") * 6;
    int versionX = (SCREEN_WIDTH - versionWidth) / 2;
    tft.setCursor(versionX, 150);
    tft.print("v1.0  2024.01");
}