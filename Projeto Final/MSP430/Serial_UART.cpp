#include "Serial_UART.h"


void Serial_UARTClass::init(unsigned long BaudRate)
{
	this->baudRate = BaudRate;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;		// CLK = 1 MHz
	P1SEL = RXD + TXD;			// Habilitar RX e TX
	P1SEL2 = RXD + TXD;			// Habilitar RX e TX
	UCA0CTL1 |= UCSSEL_2;		// SMCLK
	UCA0BR0 = (unsigned char)(1000000 / BaudRate); // 1MHz / BaudRate
	UCA0BR1 = 0;				// Fazer depois
	UCA0MCTL = UCBRS0; // Ajuste do rate para diminuir o erro na comunicação
	UCA0CTL1 &= ~UCSWRST;		// **Initialize USCI state machine**  - ??????
}


void Serial_UARTClass::read(byte * buff, int offset, int length)
{
	this->iRx = offset - 1;
	this->lockRX = 1;
	this->RxBuffer = buff;
	this->offsetRX = offset;
	this->lenghtRX = length;
	UC0IE |= UCA0RXIE;

	while (lockRX);
	UC0IFG &= ~UCA0RXIFG;
}



void Serial_UARTClass::write(byte* buff, int offset, int length)
{
	this->iTx = offset - 1;
	this->lockTX = 1;
	this->TxBuffer = buff;
	this->offsetTX = offset;
	this->lenghtTX = length;
	UC0IE |= UCA0TXIE;
	//__bis_SR_register(GIE);
	while (lockTX);
}

void Serial_UARTClass::Send_Data(unsigned char c)
{
	while ((IFG2&UCA0TXIFG) == 0);
	UCA0TXBUF = c;
}

void Serial_UARTClass::print(const char *str)
{
	/*int len = strsize(str);
	this->write((byte*)str, 0, len);*/
		int i;
		for (i = 0; str[i] != '\0'; i++)
			Send_Data(str[i]);
}


void Serial_UARTClass::print(unsigned char str)
{
	char* v = new char[4];
	itoa(str, v, 16);
	this->write((byte*)v, 0, 4);
	delete[] v;
}

void Serial_UARTClass::print(unsigned char str, unsigned char Base)
{
	char* v = new char[4];
	itoa(str, v, Base);
	this->write((byte*)v, 0, strsize(v));
	delete[] v;
}

int Serial_UARTClass::readInt()
{
	byte buff[2] = { 0 };
	this->read(buff, 0, 2);
	return (buff[1] << 8 + buff[0]);
}

void Serial_UARTClass::readStr(char* buff, int offset, int length)
{
	//this->read((byte*)buff, offset, length);
	while ((IFG2 & UCA0RXIFG) == 0);
	/*while (true)
	{
		char c = UCA0RXBUF;
		if (c == '+') break;
	}*/
	for (int i = 0; i < length - 1; i++)
		buff[i] = UCA0RXBUF;
	//buff[0] = '+';
}


unsigned char Serial_UARTClass::readByte()
{
	unsigned char value = 0;
	this->read((byte*)(&value), 0, 1);
	return value;
}

Serial_UARTClass Serial_UART;
