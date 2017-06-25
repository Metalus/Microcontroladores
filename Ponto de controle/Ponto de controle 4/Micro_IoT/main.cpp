


#include <msp430g2533.h>
#include <Serial_UART.h>
#include <Serial_SPI.h>
#include <Rc522.h>



#define CS BIT0
#define NRSTDP BIT3
Rc522 rc522(CS, NRSTDP);
unsigned char serNum[5];

int main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT


    Serial_UART.init(9600);
    delayms(200);

    Serial_UART.print("Start\r\n");
    Serial_SPI.init();
    digitalWriteA(1, CS, LOW);
    Serial_UART.print("WOW\r\n");
    while(!(Serial_UART.available()));
    char a[10] = {0};
    Serial_UART.readStr(a,0,10);
    Serial_UART.print(a);
    rc522.Init();

    while (1)
    {


        unsigned char status;
        unsigned char str[MAX_LEN];

        status = rc522.Request(PICC_REQIDL, str);
        if (status == MI_OK)  // Cartão detectado
        {
            Serial_UART.print("RFID detectado: ");
            Serial_UART.print(str[0]);
            Serial_UART.print(" , ");
            Serial_UART.print(str[1]);
            Serial_UART.print("\r\n");
        }

        status = rc522.Anticoll(str);
        memcopy(serNum, str, 5);
        if (status == MI_OK)
        {
            //digitalWrite(RED_LED, HIGH);
            Serial_UART.print("ID: ");
            Serial_UART.print(serNum[0]);
            Serial_UART.print(" , ");
            Serial_UART.print(serNum[1]);
            Serial_UART.print(" , ");
            Serial_UART.print(serNum[2]);
            Serial_UART.print(" , ");
            Serial_UART.print(serNum[3]);
            Serial_UART.print(" , ");
            Serial_UART.print(serNum[4]);
            Serial_UART.print("\r\n");


            if (serNum[0] == 0x6B && serNum[1] == 0xAB && serNum[2] == 0x8D && serNum[3] == 0x41 && serNum[4] == 0x0C)
            {
                Serial_UART.print("Acesso do bilhete unico autorizado.\r\n");
            }
            else
            {
                Serial_UART.print("Acesso negado!!!\r\n");
            }
            delayms(1000);

            // digitalWrite(RED_LED, LOW);
        }
        //Serial_UART.print("Aguardando\r\n");
        rc522.Halt();
    }

}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{


    if (UC0IFG & UCA0TXIFG)
    {
        //Pega os valores passados nos parâmetros da função
        byte* buf = Serial_UART.TxBuffer;
        volatile int* i = &Serial_UART.iTx;
        volatile byte* lock = &Serial_UART.lockTX;

        (*i)++;

        if (*i < Serial_UART.lenghtTX)
            UCA0TXBUF = buf[*i];


        else    //Coloca o valor para ser enviado no BUF do TX
        {
            while (UCA0STAT & UCBUSY);
            UC0IE &= ~UCA0TXIE;

            *lock = 0;

        }
    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    if (UC0IFG & UCA0RXIFG)
    {

        //Pega os valores passados nos parâmetros da função
        byte* buf = Serial_UART.RxBuffer;
        volatile int* i = &Serial_UART.iRx;
        volatile byte* lock = &Serial_UART.lockRX;
        (*i)++;

        unsigned char rx = UCA0RXBUF;
        if(rx == '\0' || *i > Serial_UART.lenghtRX)
        {
            while (UCA0STAT & UCBUSY);
            UC0IE &= ~UCA0RXIE;
            *lock = 0;
        }

        else//(*i < Serial_UART.lenghtTX && buf[*(i - 1)] != '\0')
            buf[*i] = rx;
    }
}


//typedef struct
//{
//  uint32_t baud;
//} uart_config_t;
//
//struct baud_value
//{
//  uint32_t baud;
//  uint16_t UCAxBR0;
//  uint16_t UCAxBR1;
//  uint16_t UCAxMCTL;
//};
//
//const struct baud_value baud_tbl[] = {
//  { 9600, 104, 0, 0x2 }
//};
//
//int uart_getchar(void)
//{
//  int chr = -1;
//
//  if (IFG2 & UCA0RXIFG) {
//      chr = UCA0RXBUF;
//  }
//
//  return chr;
//}
//
//int uart_putchar(int c)
//{
//  /* Wait for the transmit buffer to be ready */
//  while (!(IFG2 & UCA0TXIFG));
//
//  /* Transmit data */
//  UCA0TXBUF = (char)c;
//
//  return 0;
//}
//
//int uart_puts(const char *str)
//{
//  int status = -1;
//
//  if (str != NULL) {
//      status = 0;
//
//      while (*str != '\0') {
//          /* Wait for the transmit buffer to be ready */
//          while (!(IFG2 & UCA0TXIFG));
//
//          /* Transmit data */
//          UCA0TXBUF = *str;
//
//          /* If there is a line-feed, add a carriage return */
//          if (*str == '\n') {
//              /* Wait for the transmit buffer to be ready */
//              while (!(IFG2 & UCA0TXIFG));
//              UCA0TXBUF = '\r';
//          }
//
//          str++;
//      }
//  }
//
//  return status;
//}
//
//
//
//
//int uart_init(uart_config_t *config)
//{
//  int status = -1;
//
//  /* USCI should be in reset before configuring - only configure once */
//  if (UCA0CTL1 & UCSWRST) {
//      size_t i;
//
//      /* Set clock source to SMCLK */
//      UCA0CTL1 |= UCSSEL_2;
//
//      /* Find the settings from the baud rate table */
//      for (i = 0; i < 4; i++) {
//          if (baud_tbl[i].baud == config->baud) {
//              break;
//          }
//      }
//
//      if (i < 4) {
//          /* Set the baud rate */
//          UCA0BR0 = baud_tbl[i].UCAxBR0;
//          UCA0BR1 = baud_tbl[i].UCAxBR1;
//          UCA0MCTL = baud_tbl[i].UCAxMCTL;
//
//          /* Enable the USCI peripheral (take it out of reset) */
//          UCA0CTL1 &= ~UCSWRST;
//          status = 0;
//      }
//  }
//
//  return status;
//}
//
//int main(int argc, char *argv[])
//{
//  (void)argc;
//  (void)argv;
//
//  if (board_init() == 0) {
//      int timer_handle = -1;
//
//      uart_puts("\n**********************************************");
//      uart_puts("\nSimply Embedded tutorials for MSP430 Launchpad");
//      uart_puts("\nsimplyembedded.org");
//      uart_puts("\nVersion: 0.9");
//      uart_puts("\n"__DATE__);
//      uart_puts("\n**********************************************");
//
//      menu_init(main_menu, ARRAY_SIZE(main_menu));
//
//      while (1) {
//          watchdog_pet();
//          menu_run();
//
//          /**
//          * If blinking is enabled and the timer handle is
//          * negative (invalid) create a periodic timer
//          */
//          if (_blink_enable != 0) {
//              if (timer_handle < 0) {
//                  timer_handle = timer_create(_timer_ms, 1, blink_led, NULL);
//              }
//          }
//          else {
//              if (timer_handle != -1) {
//                  timer_delete(timer_handle);
//                  timer_handle = -1;
//              }
//          }
//      }
//  }
//
//  return 0;
//}
