#ifndef CONFIG_H
#define CONFIG_H

// =================== 编译配置 ===================
// 决定此次编译使用哪个节点配置
#define CURRENT_NODE 2  // 1=Node1, 2=Node2

// 根据节点选择包含对应配置并设置UI显示
#if CURRENT_NODE == 1
    // #include "Node1Config.h"
    #define ENABLE_UI_DISPLAY 0
#elif CURRENT_NODE == 2  
    // #include "Node2Config.h"
    #define ENABLE_UI_DISPLAY 1
#else
    #error "CURRENT_NODE must be 1 or 2"
#endif

#endif // CONFIG_H 