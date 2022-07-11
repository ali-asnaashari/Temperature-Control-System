#include <avr/io.h>
#include <util/delay.h>
#include <LCD.h>
#include <string.h>


void LCD_init(){

    unsigned char element;
    unsigned char Welcome_MSG[15]="MP-TCS-Project";

    /* Call init_LCD Function for initialization */
    init_LCD();

    /* Make blinking cursor */
    LCD_cmd(0x0F);

    for(element  = 0 ; element < 15 ; element++){
        LCD_write(Welcome_MSG[element]);
    }
    /* clear screen */
    LCD_cmd(0x01);
     
}


void SPI_init(){

    /*
        SPI Type: Master
        SPI Clock Rate: 8MHz / 128 = 62.5 kHz
        SPI Clock Phase: Cycle Half
        SPI Clock Polarity: Low
        SPI Data Order: MSB First
    */
    SPCR = (0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);

}

void AC_init(){

    ACSR |= (0 << ACD) | (0 << ACBG) | (0 << ACO) | (0 << ACI) | (0 << ACIE) | (0 << ACIC) | (0 << ACIS1) | (0 << ACIS0);
}

void ADC_init(void){

    /*  Define ADC (PA0-PA7) port as input */
    
    DDRA   = (0 << PA0) | (0 << PA1) | (0 << PA2) | (0 << PA3) | (0 << PA4) | (0 << PA5) | (0 << PA6) | (0 << PA7);

    /* Define Vref Mode -> use AVcc */	        
	ADMUX   = (0 << REFS1) | (1 << REFS0);


    /* 
       ADCSRA Register Configuration
       ADEN: 1 -> Analog To Digital Enable
       Prescaler: 128
    */	     
    ADCSRA  = (1<<ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  
}

int ADC_Operation(void){

    /* 
       ADCSRA Register Configuration
       ADSC: 1 -> (ADC start conversion)
       ADCW -> ADCL + ADCH -> Catch Information
    */	     

    ADCSRA |= ((1 << ADSC) | (1 << ADIF)); 
    while( (ADCSRA & (1 << ADIF)) == 0 );  // wait till end of the conversion
	ADCSRA |= ( 1 << ADIF );            
	return ADCW;                       
}


void print_result_TempA_BigThan_TempB(){

    unsigned char TempMSG[10]="Temp-A is ";
    char sensor[10];
    float constant = 0.4888;
	float temp;

    // Convert Voltage to Number -> LM35 DataSheet
    temp = (ADC_Operation() * constant);
    itoa((int)temp,sensor,10);


    /* Print Temperature on LCD */

    for(int i=0; i<10; i++){
        LCD_write(TempMSG[i]);
    }
    for(int i=0; i<2; i++){
        LCD_write(sensor[i]);
    }
    _delay_ms(100);

    /* clear screen */
    LCD_cmd(0x01);
}

void print_result_TempA_lessThan_TempB(){
    unsigned char ErrorMSG[15]="Temp-B > Temp-A";
    for(unsigned char element  = 0 ; element < 15 ; element++){
        LCD_write(ErrorMSG[element]);
    }
    _delay_ms(100);

    /* clear screen */
    LCD_cmd(0x01);
}


int main(void) {


    /* LCD Configuration */
    
    DDRC = (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7);
    DDRD = (1 << PD0) | (1 << PD1) | (1 << PD2) | (0 << PD3) | (0 << PD4) | (0 << PD5) | (0 << PD6) | (0 << PD7);

    LCD_init();

    /* SPI Configuration */
    SPI_init();

    /* Analog To Digital Configuration */
    ADC_init();

    /* Analog Comparator Control and Status Register Configuration */
    AC_init();

    
    while(1) {


        /* Check If AIN0 > AIN1 Means TEMP-Value-A > TEMP-Value-B Then Analog Comparator Output(ACO) = 1 */
        if(( (ACSR >> ACO) & 1) == 1){


            print_result_TempA_BigThan_TempB();


            /* PB2 = AIN0 = TEMP-Value-A */

            // SPDR = sensor;

        }
        else {
            print_result_TempA_lessThan_TempB();
        }


    }


}