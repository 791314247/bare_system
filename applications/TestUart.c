#include "TestUart.h"

void Uartx_Init(UART_Type *UARTx)
{

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_UART_InitTypeDef UART_InitStruct = {0};

    switch ((uint32_t)UARTx) {
    case UART0_BASE:
        //PA13:UART0-RX   PA14:UART0-TX
        GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = DISABLE;
        GPIO_InitStruct.RemapPin = DISABLE;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        //PA2:UART0-RX   PA3:UART0-TX
//          GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
//          GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
//          GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//          GPIO_InitStruct.Pull = DISABLE;
//          GPIO_InitStruct.RemapPin = DISABLE;
//          LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        UART_InitStruct.ClockSrc = LL_RCC_UART_OPERATION_CLOCK_SOURCE_APBCLK1;
        break;

    case UART1_BASE:
        //PB13:UART1-RX   PB14:UART1-TX
        GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = DISABLE;
        GPIO_InitStruct.RemapPin = DISABLE;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        //PC2:UART1-RX   PC3:UART1-TX
//          GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
//          GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
//          GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//          GPIO_InitStruct.Pull = DISABLE;
//          GPIO_InitStruct.RemapPin = DISABLE;
//          LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        UART_InitStruct.ClockSrc = LL_RCC_UART_OPERATION_CLOCK_SOURCE_APBCLK1;
        break;

    case UART4_BASE:
        //PB2:UART4-RX   PB3:UART4-TX
        GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = DISABLE;
        GPIO_InitStruct.RemapPin = DISABLE;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        //PA0:UART4-RX   PA1:UART4-TX
//          GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
//          GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
//          GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//          GPIO_InitStruct.Pull = DISABLE;
//          GPIO_InitStruct.RemapPin = DISABLE;
//          LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        break;

    case UART5_BASE:
        //PD0:UART5-RX   PD1:UART5-TX
        GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = DISABLE;
        GPIO_InitStruct.RemapPin = DISABLE;
        LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        //PC4:UART5-RX   PC5:UART5-TX
//          GPIO_InitStruct.Pin = LL_GPIO_PIN_4|LL_GPIO_PIN_5;
//          GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
//          GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//          GPIO_InitStruct.Pull = DISABLE;
//          GPIO_InitStruct.RemapPin = DISABLE;
//          LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        break;

    default:
        break;
    }
    UART_InitStruct.BaudRate = 115200;                              //������
    UART_InitStruct.DataWidth = LL_UART_DATAWIDTH_8B;               //����λ��
    UART_InitStruct.StopBits = LL_UART_STOPBITS_1;                  //ֹͣλ
    UART_InitStruct.Parity = LL_UART_PARITY_EVEN;                   //��żУ��
    UART_InitStruct.TransferDirection = LL_UART_DIRECTION_TX_RX;    //����-����ʹ��
    UART_InitStruct.InfraredModulation = DISABLE;
    LL_UART_Init(UARTx, &UART_InitStruct);
}


void Test_Uartx(UART_Type *UARTx)
{

    uint8_t TestTxData[11] = "TestUart-TX";
    uint8_t i;
    volatile uint8_t tmp08;

    Uartx_Init(UARTx);//��ʼ��uart����


    //��ѯ��������
    for (i = 0; i < 11; i++) {
        LL_UART_TransmitData(UARTx, TestTxData[i]);     //����������д�뷢�ͼĴ���
        while (SET != LL_UART_IsActiveFlag_TXSE(UARTx));    //�ȴ��������

    }

    //ת�����յ�������
    while (1) {
        if (SET == LL_UART_IsActiveFlag_RXBF(UARTx)) {  //�ȴ����յ�һ���ֽ�
            tmp08 = LL_UART_ReceiveData(UARTx);         //uart�����жϱ�־��ͨ����ȡrxreg���
            LL_UART_TransmitData(UARTx, tmp08);         //���յ������ݷ��ͻ�ȥ

        }
    }
}
