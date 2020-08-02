/*
 * Author: Guillermo O. «Tordek» Freschi
 * License: GNU GPL v3.0
 */

#define _XTAL_FREQ 24000000

#include <xc.h>

// CONFIG1L
#pragma config PLLDIV = 6       // PLL Prescaler Selection bits (Divide by 6 (24 MHz oscillator input))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 2       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = ON      // USB Voltage Regulator Enable bit (USB voltage regulator enabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#include <stdint.h>
#include <usb_device.h>
#include <usb_device_hid.h>

#include "n64_communications.h"
#include "cheat.h"

// The joystick can only be read so often; this variable is 1 whenever it can be.
volatile char read_enable = 0;
volatile int gamepad_connected = 0;

// Located in USB accessible memory

struct {
    char usb_report_id;

    union {
        char raw[4];
        struct n64_poll_response n64_response;
    };
} n64_usb_message @ 0x500;

// Handle to last transmission. Limits communication speed.
USB_VOLATILE USB_HANDLE last_transmission = NULL;

/**
 * Interrupt function. Triggered on TIMER1 overflow.
 *
 * TIMER1 triggers approximately every 1ms.
 */
void interrupt isr()
{
    if (PIR1bits.TMR1IF == 1) {
        read_enable++;
        PIR1bits.TMR1IF = 0;
        TMR1 = 5600; // 6MHz/60k = 100Hz.
    }
}

void main(void)
{
    // bool status_led = 0;
    int led_blink_timer = 0;

    // Enable TIMER1 interrupt.
    // TIMER1 2^16/6MHz = ~1ms
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
    T1CONbits.RD16 = 1;
    T1CONbits.TMR1ON = 1;

    // Status LEDs.
    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    LATA = 0b00000001;

    // Joystick ports.
    LATB = 0;
    ADCON1 = 0x0F;
    TRISB = 0x0F;

    LATA2 = 1;
    GIE = 0;
    USBDeviceInit();
    USBDeviceAttach();
    GIE = 1;
    LATA2 = 1;

    for (;;) {
        GIE = 0;
        USBDeviceTasks();
        GIE = 1;

        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                USBIsDeviceSuspended()) {
            //Either the device is not configured or we are suspended,
            // so we don't want to execute any USB related application code
            continue;
        }

        if (read_enable < 1) {
            continue;
        }

        if (HIDTxHandleBusy(last_transmission)) {
            continue;
        }

        read_enable = 0;

        if (!gamepad_connected) {
            // Query command. Should return 05 00 01 or 05 00 02.
            char command[] = {0x00};
            char response[3];
            char rc = tdk64_command(0, command, 1, 0, response, 3);

            if (rc) {
                led_blink_timer -= rc;
                if (led_blink_timer <= 0) {
                    LATA2 = !LATA2;
                    led_blink_timer = 500;
                }
            } else {
                gamepad_connected = 1;
                LATA2 = 0;
            }
        } else {
            char command[1] = {0x01};
            char rc = tdk64_command(0, command, 1, 0, n64_usb_message.raw, 4);

            if (rc) {
                gamepad_connected = 0;
                memset(n64_usb_message.raw, 0, 5);
            } else {
                cheat(&n64_usb_message.n64_response);
                n64_usb_message.n64_response.analog_y = -n64_usb_message.n64_response.analog_y;
            }

            n64_usb_message.usb_report_id = 1;

            last_transmission = HIDTxPacket(HID_EP, (void *) &n64_usb_message, 5);
        }
    }
}

bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch (event) {
    case EVENT_EP0_REQUEST:
        USBCheckHIDRequest();
        break;
    case EVENT_CONFIGURED:
        USBEnableEndpoint(HID_EP, USB_IN_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
        break;
    }

    return true;
}
