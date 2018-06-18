# Z-stack2.4.0-1.4.0
# 口袋实验室魔改版协议栈

## 魔改部分
- 板上 **D1** 和 **D2** （LED）对应宏定义 **HAL_LED_1** 和 **HAL_LED_2** （其中HAL_LED_2和HAL_LED_3都是指向D2这盏灯）

- 按键 **S1** 和 **S2** 分别对应 **HAL_KEY_SW_1** 和 **HAL_KEY_SW_2**

    如果想让按键具有shift功能，只需修改 **hal_key.c中HalKeyPoll**函数下两个button判断，任意一个按键宏改为6，或更改onboard.c 下 **OnBoard_KeyCallback ( uint8 keys, uint8 state )** 函数中 shift 指定的宏

## 基础deemo 是广播的helloworld互传，可根据需要自行更改

- 详细修改情况请自行查看commit记录
 
### PS：
- 由于板上LED的设计，UART0的CTS功能，不能使用了，如果要开启此功能的话，将_hal_uart_dma.c 下的HAL_UART_Px_CTS宏定义修改为0x10即可（可以参照based版本）

- 寻找问题建议使用watch 窗口，在调试的时候定位相关寄存器和变量，快速定位问题。

