# Z-stack2.4.0-1.4.0
# 口袋实验室魔改版协议栈

## 魔改部分
- 板上 **D1** 和 **D2** （LED）对应宏定义 **HAL_LED_1** 和 **HAL_LED_2** （其中HAL_LED_2和HAL_LED_3都是指向D2这盏灯）

- 按键 **S1** 和 **S2** 分别对应 **HAL_KEY_SW_1** 和 **HAL_KEY_SW_2**

    如果想让按键具有shift功能，只需修改 **hal_key.c中HalKeyPoll**函数下两个button判断，任意一个按键宏改为6，或更改onboard.c 下 **OnBoard_KeyCallback ( uint8 keys, uint8 state )** 函数中 shift 指定的宏

- OLED采用板上IIC接口，基本函数Hal_Oled_WriteString(u8 x,u8 y, u8 *p,u8 Char_Size)  
  前两个参数控制字符串在屏幕中的位置，x 0~128 可用宏定义，y 0到7 可用宏定义，第三个参数是放置字符串的位置，可采用指针形式，最后一个指定字号大小有8x16和6x8 两种可用宏定义。
    宏定义的详情参考release

## 基础deemo 是广播的helloworld互传，可根据需要自行更改

- 详细修改情况请自行查看commit记录
 
# 使用方法

- 下载所有文件，或仅修改部分文件，替换即可。
- release压缩包开箱即用，环境是IAR7.6（学校机房版本），高版本打开会出现编译配置问题。


## Tips
- 样例中定时任务如果注册，且没有具体执行函数，就会组网失败（表象，内部原因尚未明晰）
- 基于协议栈裸机编程，使用按键外部中断会和协议栈内部按键回调函数冲突，请注意！！
- 若使用IAR8.3 会出现编译和debug的错误，请善用搜索引擎解决

### PS：
- 由于板上LED的设计，UART0的CTS功能，不能使用了，如果要开启此功能的话，将_hal_uart_dma.c 下的HAL_UART_Px_CTS宏定义修改为0x10即可（可以参照based版本）

- 寻找问题建议使用watch 窗口，在调试的时候定位相关寄存器和变量，快速定位问题。

- 本仓库只包含工程中的C文件和头文件，其他没有。![emj](https://i0.hdslb.com/bfs/vip/86ccf6d0b5480169bf80f3582fae09d7ed455c06.png@50w_50h.webp)


### 如何定做适合自己的HAL处理的程序
1. 修改原文件的方式：
    1. HAL\include下的头文件应该保留一样。
    2. 在HAL\Target\hal_xxx.c修改相应的驱动函数，hal_adc.c, hal_key.c, hal_lcd.c, hal_led.c, hal_timer.c, and hal_uart.c
    3. 硬件驱动配置可以被修改在hal_board_cfg.h
2. 增加用户自己的目标驱动
    1. 增加新的头文件，在hal\include 
    2. 在hal\Target\hal_xxx.c添加自己运行函数，xxx为自己的目标
    3. 如果GPIO有冲突或者没用到，应该保证驱动不被编译，否则后果严重。
    4. 检查GPIO有没有正确设置或冲突，通过hal_board_cfg.h
    5. 不想被编译，或者是老的文件，没用到的文件，可以通过选择options->"Exclude form build"

