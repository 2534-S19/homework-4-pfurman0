#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"
#include <ti/devices/msp432p4xx/driverlib/gpio.h>

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool finished = false;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.

    eUSCI_UART_ConfigV1 uartconfig =
    {
             EUSCI_A_UART_CLOCKSOURCE_SMCLK,              // Onboard clock
             65,                                          // 10MHz/ 9600 = 1041, 1041mod16 = 1
             1,
             0xD6,
             EUSCI_A_UART_NO_PARITY,                       // No Parity
             EUSCI_A_UART_LSB_FIRST,                       // LSB First
             EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
             EUSCI_A_UART_MODE,                            // UART mode
             EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,// Oversampling
             EUSCI_A_UART_8_BIT_LEN

    };


    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // TODO: Initialize EUSCI_A0

    UART_initModule(EUSCI_A0_BASE, &uartconfig); //Not sure why this is an invalid argument?

    // TODO: Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
           rChar = UART_receiveData(EUSCI_A0_BASE);
        }
        else
        {
            rChar = 0xFF;
        }
        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.

        if(rChar != 0xFF)
        {
            charFSM(rChar);
            if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) != EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
            {
                UART_transmitData(EUSCI_A0_BASE, rChar);
            }
        }

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if(finished == 1)
        {
            UART_transmitData(EUSCI_A0_BASE, rChar);
            finished = 0;
        }

    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    typedef enum
    {SX, S1, S2, S3} hw4;
    char test;
    bool finished = false;
    static hw4 currentstate = SX;
    switch(currentstate)
    {
    case SX:
        if(finished == 0)
        {
          test=rChar;
          currentstate=S1;
          break;
        }
    case S1:
        if(finished == 0)
        {
          currentstate=S2;
          break;
        }
    case S2:
        if(finished == 0)
        {
          currentstate=S3;
          finished = 1;
          break;
        }
    }

    return finished;
}
