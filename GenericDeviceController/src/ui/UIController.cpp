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

#if ENABLE_UI_DISPLAY
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
      lastUpdate(0),
      needRedraw(true),
      blinkTimer(0),
      blinkState(false) {
    g_uiController = this;
}
#else
UIController::UIController() {
    g_uiController = this;
}
#endif

#if ENABLE_UI_DISPLAY
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
    attachInterrupt(digitalPinToInterrupt(EC11_SW), encoderSwitchISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_OK), backButtonISR, FALLING);
    
    // 显示启动界面
    displayStartupScreen();
    
    Serial.println("[UI] UIController initialized");
}

void UIController::update() {
    unsigned long currentTime = millis();
    
    // 处理输入
    handleInput();
    
    // 更新闪烁状态
    updateBlinkState();
    
    // 定期刷新显示或需要重绘时
    if (needRedraw || (currentTime - lastUpdate > 1000)) {
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
    }
}

void UIController::handleInput() {
    // 处理编码器旋转
    if (encoderDirection != 0) {
        handleEncoderRotation(encoderDirection);
        encoderDirection = 0;
    }
    
    // 处理编码器按下
    if (encoderPressed) {
        encoderPressed = false;
        handleEncoderPress();
        delay(200); // 防抖
    }
    
    // 处理返回按键
    if (backButtonPressed) {
        backButtonPressed = false;
        handleBackButton();
        delay(200); // 防抖
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
            setRedraw();
            break;
            
        case STATE_BROWSE:
            // 浏览模式：进入编辑模式
            currentState = STATE_EDIT;
            editMode = true;
            setRedraw();
            break;
            
        case STATE_EDIT:
            // 编辑模式：退出编辑，回到浏览模式
            currentState = STATE_BROWSE;
            editMode = false;
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
            // 概览页：进入系统设置页
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
        data.temperature = constrain(data.temperature, 0.0f, 50.0f);
    } else {
        data.humidity += direction * 1.0f;
        data.humidity = constrain(data.humidity, 0.0f, 100.0f);
    }
    
    setSensorData((RoomIndex)selectedRoom, data.temperature, data.humidity);
}

void UIController::drawOverviewPage() {
    tft.fillScreen(COLOR_BLACK);
    
    // 绘制标题
    drawHeader("环境监控");
    
    // 绘制所有房间数据
    int y = 25;
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
        
        // 温度数据（英文数字）
        tft.setCursor(50, y);
        tft.print(data.temperature, 1);
        tft.print("C");
        
        // 湿度（下一行）
        y += 12;
        tft.setCursor(50, y);
        tft.print(data.humidity, 1);
        tft.print("%");
        
        y += 16;
    }
    
    // 底部状态栏
    y = SCREEN_HEIGHT - 20;
    tft.setTextColor(COLOR_CYAN);
    tft.setTextSize(1);
    
    // WiFi和MQTT状态图标
    drawWiFiIcon(0, y);
    drawMQTTIcon(20, y);
    
    // 操作提示
    tft.setCursor(40, y);
    tft.print(" <>");
    
    printChineseSmall(0, y + 18, "转动选择 按下进入", COLOR_CYAN);
}

void UIController::drawRoomPage() {
    tft.fillScreen(COLOR_BLACK);
    
    // 绘制标题
    String title = String("🏠") + getRoomName(selectedRoom);
    drawHeader(title.c_str());
    
    SensorData data = getSensorData((RoomIndex)selectedRoom);
    
    int y = 30;
    
    // 温度行
    bool tempSelected = (selectedItem == ITEM_TEMPERATURE);
    bool tempEditing = (currentState == STATE_EDIT && tempSelected);
    
    uint16_t tempColor = COLOR_WHITE;
    if (tempSelected) {
        tempColor = tempEditing ? (blinkState ? COLOR_YELLOW : COLOR_RED) : COLOR_YELLOW;
    }
    
    tft.setTextColor(tempColor);
    tft.setTextSize(1);
    
    if (tempSelected) {
        tft.setCursor(0, y);
        tft.print(">");
    }
    
    // 使用U8g2显示emoji + 中文
    u8g2.setForegroundColor(tempColor);
    u8g2.setCursor(8, y + 8);
    u8g2.print("🌡️温度:");
    
    // 使用原生库显示数字
    tft.setTextColor(tempColor);
    tft.setCursor(65, y);
    tft.print(data.temperature, 1);
    tft.print("C");
    
    y += 16;
    
    // 湿度行
    bool humSelected = (selectedItem == ITEM_HUMIDITY);
    bool humEditing = (currentState == STATE_EDIT && humSelected);
    
    uint16_t humColor = COLOR_WHITE;
    if (humSelected) {
        humColor = humEditing ? (blinkState ? COLOR_CYAN : COLOR_BLUE) : COLOR_CYAN;
    }
    
    tft.setTextColor(humColor);
    
    if (humSelected) {
        tft.setCursor(0, y);
        tft.print(">");
    }
    
    // 使用U8g2显示emoji + 中文
    u8g2.setForegroundColor(humColor);
    u8g2.setCursor(8, y + 8);
    u8g2.print("💧湿度:");
    
    // 使用原生库显示数字
    tft.setTextColor(humColor);
    tft.setCursor(65, y);
    tft.print(data.humidity, 1);
    tft.print("%");
    
    y += 25;
    
    // 进度条
    if (selectedItem == ITEM_TEMPERATURE) {
        drawProgressBar(8, y, 100, 8, data.temperature, 50.0f);
    } else {
        drawProgressBar(8, y, 100, 8, data.humidity, 100.0f);
    }
    
    // 底部操作提示
    y = SCREEN_HEIGHT - 30;
    tft.setTextColor(COLOR_GREEN);
    tft.setTextSize(1);
    
    if (currentState == STATE_BROWSE) {
        printChineseSmall(0, y + 8, "转动选择", COLOR_GREEN);
        printChineseSmall(0, y + 18, "按下编辑", COLOR_GREEN);
    } else {
        printChineseSmall(0, y + 8, "转动调节", COLOR_GREEN);
        printChineseSmall(0, y + 18, "按下确认", COLOR_GREEN);
    }
    
    printChineseSmall(0, y + 28, "按键返回", COLOR_GREEN);
}

void UIController::drawSettingsPage() {
    tft.fillScreen(COLOR_BLACK);
    
    drawHeader("系统设置");
    
    int y = 30;
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    
    // WiFi状态
    u8g2.setForegroundColor(COLOR_WHITE);
    u8g2.setCursor(0, y + 8);
    u8g2.print("📶 WiFi:");
    if (WiFi.status() == WL_CONNECTED) {
        printChineseSmall(50, y + 8, "已连接", COLOR_GREEN);
        y += 12;
        tft.setCursor(0, y);
        tft.print("   ");
        tft.print(WiFi.SSID());
        y += 12;
        tft.setCursor(0, y);
        tft.print("   ");
        tft.print(WiFi.localIP());
    } else {
        printChineseSmall(50, y + 8, "未连接", COLOR_RED);
    }
    
    y += 20;
    
    // MQTT状态
    extern PubSubClient client; // 引用外部MQTT客户端
    u8g2.setForegroundColor(COLOR_WHITE);
    u8g2.setCursor(0, y + 8);
    u8g2.print("🔗 MQTT:");
    if (client.connected()) {
        printChineseSmall(50, y + 8, "在线", COLOR_GREEN);
    } else {
        printChineseSmall(50, y + 8, "离线", COLOR_RED);
    }
    
    // 底部操作提示
    y = SCREEN_HEIGHT - 20;
    printChineseSmall(0, y + 8, "按键返回", COLOR_GREEN);
}

void UIController::drawHeader(const char* title) {
    // 使用U8g2显示中文标题
    printChinese((SCREEN_WIDTH - strlen(title) * 6) / 2, 15, title, COLOR_WHITE);
    
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
    if (WiFi.status() == WL_CONNECTED) {
        u8g2.setForegroundColor(COLOR_GREEN);
        u8g2.setCursor(x, y + 8);
        u8g2.print("📶");
    } else {
        u8g2.setForegroundColor(COLOR_RED);
        u8g2.setCursor(x, y + 8);
        u8g2.print("📶");
    }
}

void UIController::drawMQTTIcon(int x, int y) {
    extern PubSubClient client;
    if (client.connected()) {
        u8g2.setForegroundColor(COLOR_GREEN);
        u8g2.setCursor(x, y + 8);
        u8g2.print("🔗");
    } else {
        u8g2.setForegroundColor(COLOR_RED);
        u8g2.setCursor(x, y + 8);
        u8g2.print("🔗");
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

void UIController::updateBlinkState() {
    if (millis() - blinkTimer > 500) {
        blinkState = !blinkState;
        blinkTimer = millis();
        if (currentState == STATE_EDIT) {
            setRedraw();
        }
    }
}

// 中断处理函数实现
void IRAM_ATTR UIController::handleEncoderInterrupt() {
    int currentA = digitalRead(EC11_A);
    int currentB = digitalRead(EC11_B);
    
    if (currentA != lastEncoderA) {
        if (currentA == currentB) {
            encoderDirection = 1; // 顺时针
        } else {
            encoderDirection = -1; // 逆时针
        }
        lastEncoderA = currentA;
    }
}

void IRAM_ATTR UIController::handleEncoderSwitchInterrupt() {
    static unsigned long lastPress = 0;
    unsigned long now = millis();
    if (now - lastPress > 200) { // 防抖
        encoderPressed = true;
        lastPress = now;
    }
}

void IRAM_ATTR UIController::handleBackButtonInterrupt() {
    static unsigned long lastPress = 0;
    unsigned long now = millis();
    if (now - lastPress > 200) { // 防抖
        backButtonPressed = true;
        lastPress = now;
    }
}

void UIController::displayStartupScreen() {
    // 清屏
    tft.fillScreen(COLOR_BLACK);
    
    // 显示欢迎信息
    u8g2.setForegroundColor(COLOR_WHITE);
    u8g2.setFont(u8g2_font_wqy14_t_gb2312);
    u8g2.setCursor(20, 40);
    u8g2.println("智能家居");
    
    u8g2.setCursor(20, 60);
    u8g2.println("控制中心");
    
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 80);
    tft.println("Node1 - UI Module");
    
    // 显示状态信息
    printChineseSmall(10, 110, "正在初始化...", COLOR_YELLOW);
    printChineseSmall(10, 130, "连接WiFi中...", COLOR_CYAN);
    
    // 显示操作提示
    printChineseSmall(10, 150, "使用编码器操作", COLOR_GREEN);
}

#endif // ENABLE_UI_DISPLAY