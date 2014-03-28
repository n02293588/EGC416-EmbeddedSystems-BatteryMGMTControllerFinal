#include "derivative.h" /* derivative-specific definitions */
#include <mc9s12dt256.h>


void Delay(unsigned int);

void LCD_CMD(void);
void LCD_DAT(char);
void TMR_INI(void);
void LCD_INI(void);
void ATD_INI(void);

void main(void) {

    unsigned int Tcount, T1, T2, V;
    int i = 0;
    int k = 0;
    char tmp[3];
    char tmp2[4];
    float n = 0;
    int t = 0;

    DDRA = 0xFF; //Port A LCD output
    DDRB = 0x07; //Port B LCD input (PB0-E, PB1-RS, PB2-RW) 
    
    ATD_INI();

    TMR_INI();    

    LCD_INI();


    for(;;) {


        T1=0;
        T2=0;
        Tcount=0;
        V=0;
        k=0;
        tmp2[0]=0;
        tmp2[1]=0;
        tmp2[2]=0;
        tmp2[3]=0;
        i=0;

        TFLG1 = 0x01;
        while(!(TFLG1 & 0x01)); //Check falling edge at PT0
        T1 = TC0; //records the time when there is a falling edge
        TFLG1 = 0x01;
        while(!(TFLG1 & 0x01)); //records the next time when falling edge occurs
        T2 = TC0;
        Tcount = T2 - T1; //calculate period
        Tcount = (Tcount * 64)/1000; 
        V = 30000/Tcount;
        
        while(V != 0) {
            tmp2[i] = (V % 10) + 48;
            V = (V/10) ;
            i++;
        }
        
        LCD_DAT('R');
        LCD_DAT('P');
        LCD_DAT('M');
        LCD_DAT(';');;
        LCD_DAT(tmp2[3]);
        LCD_DAT(tmp2[2]);
        LCD_DAT(tmp2[1]);
        LCD_DAT(tmp2[0]);

        PORTA = 0xC0;
        LCD_CMD();

        ATD0CTL5 = 0x80; //ATD Channel 0
        while(!(ATD0STAT0 & ATD0STAT0_SCF_MASK));
        PORTA = ATD0DR0L;
        t = (ATD0DR0L * 100)/255;
        if (t < 100) {   //Display duty cycle when under 100
            tmp[0] = (t % 10) + 48;
            t = t/10;
            tmp[1] = t + 48;
            LCD_DAT('D');
            LCD_DAT('t');
            LCD_DAT('y');
            LCD_DAT('C');
            LCD_DAT(';');
            LCD_DAT(tmp[1]);
            LCD_DAT(tmp[0]);
            LCD_DAT('%');
           
            PORTA = 0x01;
            LCD_CMD();
        } 
        else {
            LCD_DAT('D');
            LCD_DAT('t');
            LCD_DAT('y');
            LCD_DAT('C');
            LCD_DAT(':');
            LCD_DAT('1');
            LCD_DAT('0');
            LCD_DAT('0');
            LCD_DAT('%');

            PORTA = 0x01;
            LCD_CMD();
        }
        
    }
}
//---------------------------------------------------------------------------------------
void Delay(unsigned int itime) {
// Delays for itime microseconds

    int i, j;

    for (i = 0; i < itime; i++)
    for (j = 0; j < 331; j++);
}

//----------------------------------------------------------------------------------------
void LCD_CMD(void) {
    PORTB = 0x01; // Turn on enable pin
    Delay(16);
    PORTB = 0x00;
    Delay(16);
}

void LCD_DAT(char dis) {
    PORTA = dis;
    PORTB = 0x02;
    Delay(16);
    PORTB = 0x01;
    Delay(16);
    PORTB = 0x00;
    Delay(16);
}

void TMR_INI(void) {
    TSCR1 = 0x80; // Enable timer
    TSCR2 = 0x00; // No prescalar
    TIOS = TIOS & ~0x01; // Channel 0, Input Capture
    TCTL4 = 0x01; // Falling Edge, Channel 0
}

void LCD_INI(void) {
    Delay(250); //250 microsecond delay
    PORTA = 0x30;
    LCD_CMD();
    PORTA = 0x30;
    LCD_CMD();
    PORTA = 0x30;
    LCD_CMD();
    PORTA = 0x38;
    LCD_CMD();
    PORTA = 0x08;
    LCD_CMD();
    PORTA = 0x01;
    LCD_CMD();
    PORTA = 0x06;
    LCD_CMD();
    PORTA = 0x0E;
    LCD_CMD();
}

void ATD_INI(void) {
    ATD0CTL2 = 0x80;
    Delay(20);
    ATD0CTL3 = 0x08;
    ATD0CTL4 = 0xE0;
    PWMPRCLK = 0x02;
    PWMCLK = 0x00;
    PWMPOL = 0x01;
    PWMCAE = 0x00;
    PWMCTL = 0x00;
    PWMPER0 = 255;    
}                 