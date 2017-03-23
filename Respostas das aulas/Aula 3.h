Data: 22/03/2017

1. Dada uma variável 'a' do tipo 'char' (um byte), escreva os trechos de código em C para:
	(a) Somente setar o bit menos significativo de 'a'.
		a |= 0x01;

	(b) Somente setar dois bits de 'a': o menos significativo e o segundo menos significativo.
		a |= 0x03;
	
	(c) Somente zerar o terceiro bit menos significativo de 'a'.
		a &= ~(0x04);
	
	(d) Somente zerar o terceiro e o quarto bits menos significativo de 'a'.
		a &= ~(0x0C);
	
	(e) Somente inverter o bit mais significativo de 'a'.
		a ^= 0x80;
	
	(f) Inverter o nibble mais significativo de 'a', e setar o nibble menos significativo de 'a'. 
		a = (~(a) & 0xF0) | 0x0F;

2. Considerando a placa Launchpad do MSP430, escreva o código em C para piscar os dois LEDs ininterruptamente.

	#include <msp430g2553.h>
	#define LED1 BIT0
	#define LED2 BIT6
	
	void main(void)
	{
		WDTCTL = WDTPW | WDTHOLD;
		P1DIR = LED1 + LED2;
		P1OUT = 0;
		for(;;)
		{
			P1OUT |= LED1 + LED2;
			P1OUT &= ~(LED1 + LED2);
		}
	};

3. Considerando a placa Launchpad do MSP430, escreva o código em C para piscar duas vezes os dois LEDs sempre que o usuário pressionar o botão.

	#include <msp430g2553.h>
	#define BTN BIT2
	#define LED1 BIT0
	#define LED2 BIT6
	
	void main(void)
	{
		WDTCTL = WDTPW | WDTHOLD;
		P1DIR = LED1 + LED2;
		P1OUT = 0;
		for(;;)
		{
			if(P1IN & BTN == 0) // Botão pressionado
			{
				P1OUT |= LED1 + LED2;
				P1OUT &= ~(LED1 + LED2); //1 vez
				
				P1OUT |= LED1 + LED2;
				P1OUT &= ~(LED1 + LED2); //2 vezes
				
				while(P1IN & BTN == 0) {} // Aguardar o botão parar de ser pressionado para continuar.
			}
			else
				P1OUT &= ~(LED1 + LED2); // Apagar os leds
		}
	};

4. Considerando a placa Launchpad do MSP430, faça uma função em C que pisca os dois LEDs uma vez.

	void Pisca2()
	{
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //1 vez
		
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //2 vezes
	}

5. Reescreva o código da questão 2 usando a função da questão 4.

	#include <msp430g2553.h>
	#define LED1 BIT0
	#define LED2 BIT6
	
	void Pisca2()
	{
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //1 vez
		
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //2 vezes
	}
	
	void main(void)
	{
		WDTCTL = WDTPW | WDTHOLD;
		P1DIR = LED1 + LED2;
		P1OUT = 0;
		for(;;)
		{
			Pisca2();
		}
	};

6. Reescreva o código da questão 3 usando a função da questão 4.

	#include <msp430g2553.h>
	#define BTN BIT2
	#define LED1 BIT0
	#define LED2 BIT6
	
	void Pisca2()
	{
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //1 vez
		
		P1OUT |= LED1 + LED2;
		P1OUT &= ~(LED1 + LED2); //2 vezes
	}
	
	void main(void)
	{
		WDTCTL = WDTPW | WDTHOLD;
		P1DIR = LED1 + LED2;
		P1OUT = 0;
		for(;;)
		{
			if(P1IN & BTN == 0) // Botão pressionado
			{
				Pisca2();
				
				while(P1IN & BTN == 0) {} // Aguardar o botão parar de ser pressionado para continuar.
			}
			else
				P1OUT &= ~(LED1 + LED2); // Apagar os leds
		}
	};
