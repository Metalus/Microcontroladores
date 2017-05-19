#define CHPX1 BIT0
#define CHPX2 BIT1
#define CHPX3 BIT2
#define CHPXS (CHPX1 + CHPX2 + CHPX3)

void charlie_on(char CHPX_OUT, char CHPX_ON)
{
	P1OUT &= ~CHPXS;
	P1DIR &= ~ CHPXS;
	P1DIR |= CHPX_OUT;
	P1OUT |= CHPX_ON;
}

int main(void)
{
	unsigned int i;
	WDTCTL = WDTPW | WDTHOLD;
	while(1)
	{
		for(i = 0; i < 0xFFFF; i++)
		{
			charlie_on(CHPX1+CHPX2,CHPX1);
			charlie_on(CHPX1+CHPX2,CHPX2);
		}
		for(i = 0; i< 0xFFFF; i++)
		{
			charlie_on(CHPX2+CHPX3,CHPX2);
			charlie_on(CHPX2+CHPX3,CHPX3);
		}
		for(i = 0; i< 0xFFFF; i++)
		{
			charlie_on(CHPX1+CHPX3,CHPX1);
			charlie_on(CHPX1+CHPX3,CHPX3);
		}
	}
}