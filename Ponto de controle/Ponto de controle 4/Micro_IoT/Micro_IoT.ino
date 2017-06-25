#include <msp430g2533.h>
#include <legacymsp430.h>
#include "BasicsFuncs.h"
#include "Serial_UART.h"
#include "Serial_SPI.h"
#include "Rc522.h"

void SendCommand(const char* command, const int timeout)
{
	// Envio dos comandos AT para o modulo
	Serial_UART.print(command);
	delayms(timeout);

}

#define CS BIT0
#define NRSTDP BIT3
Rc522 rc522(CS, NRSTDP);
unsigned char serNum[5];

void setup()
{

	Serial_UART.init(19200);
	delayms(200);

	SendCommand("AT+RST\r\n", 2000);
	delayms(1000);
	SendCommand("AT+CIOBAUD=19200\r\n", 2000);

	delayms(1000);
	SendCommand("AT+CWJAP=\"VIVO-0100\",\"5760500100\"\r\n", 2000);
	delayms(1000);
	SendCommand("AT+CWMODE=1\r\n", 1000);
	// Mostra o endereco IP
	delayms(300);
	SendCommand("AT+CIFSR\r\n", 1000);
	// Configura para multiplas conexoes
	SendCommand("AT+CIPMUX=1\r\n", 1000);
	// Inicia o web server na porta 80
	SendCommand("AT+CIPSERVER=1,8090\r\n", 1000);

	Serial_SPI.init();
	digitalWriteA(1, CS, LOW);

	rc522.Init();

	while (1)
	{



		unsigned char status;
		unsigned char str[MAX_LEN];

		status = rc522.Request(PICC_REQIDL, str);
		if (status == MI_OK)  // Cartão detectado
		{
			Serial_UART.print("RFID detectado: ");
			Serial_UART.print(str[0], 16);
			Serial_UART.print(" , ");
			Serial_UART.print(str[1], 16);
			Serial_UART.print("\r\n");
		}

		status = rc522.Anticoll(str);
		memcpy(serNum, str, 5);
		if (status == MI_OK)
		{
       
			Serial_UART.print("ID: ");
			Serial_UART.print(serNum[0], 16);
			Serial_UART.print(" , ");
			Serial_UART.print(serNum[1], 16);
			Serial_UART.print(" , ");
			Serial_UART.print(serNum[2], 16);
			Serial_UART.print(" , ");
			Serial_UART.print(serNum[3], 16);
			Serial_UART.print(" , ");
			Serial_UART.print(serNum[4], 16);
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
		}

		rc522.Halt();
	}

}


interrupt(USCIAB0TX_VECTOR) USCI0TX_ISR(void)
{

	if (UC0IFG & UCA0TXIFG)
	{
		//Pega os valores passados nos parâmetros da função
		volatile byte* buf = Serial_UART.TxBuffer;
		volatile int* i = &Serial_UART.iTx;
		volatile byte* lock = &Serial_UART.lockTX;

		(*i)++;

		if (*i < Serial_UART.lenghtTX)
			UCA0TXBUF = buf[*i];


		else	//Coloca o valor para ser enviado no BUF do TX
		{
			while (UCA0STAT & UCBUSY);
			UC0IE &= ~UCA0TXIE;

			*lock = 0;

		}
	}
}


interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR(void)
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
