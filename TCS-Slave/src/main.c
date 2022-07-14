#include <avr/io.h>
#include <util/delay.h>


void SPI_init(){
    DDRB = (0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4);

    /*
        SPI Type: Slave
        SPI Clock Rate: 8MHz / 128 = 62.5 kHz
        SPI Clock Phase: Cycle Half
        SPI Clock Polarity: Low
        SPI Data Order: MSB First
    */
    SPCR = (0<<SPIE) | (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);

}

void LED_ON(){
    PORTC = (1 << PC0);
}

void LED_OFF(){
    PORTC = (0 << PC0);
}

void Cooler_Activate(int temperature){

    double dutyCycle = 255 / 2;
    /* FAST PWM */
    DDRB  |= ( 1 << PORTB3 );
    TCCR0 |= ( 1 << WGM01 ) | ( 1 << WGM00 );
    TCCR0 |= ( 1 << COM01 );
    TCCR0 |= ( 1 << CS01 );
    OCR0 = (int)(dutyCycle) + (int)(((temperature - 25)/5) * 0.1 * 255);

}

void Cooler_DeActivate(){
    /* FAST PWM */
    DDRB  |= ( 1 << PORTB3 );
    TCCR0 |= ( 1 << WGM01 ) | ( 1 << WGM00 );
    TCCR0 |= ( 1 << COM01 );
    TCCR0 |= ( 1 << CS01 );
    OCR0 = 0;
}

void Heater_Activate(){
    PORTD = ( 1 << PORTD0 ); 
}

void Heater_DeActivate(){
    PORTD = ( 0 << PORTD0 ); 
}


int main(void) {

    unsigned int temp = 0;
    SPI_init();

    DDRC = (1 << PC0) | (0 << PC1) | (0 << PC2) | (0 << PC3) | (0 << PC4) | (0 << PC5) | (0 << PC6) | (0 << PC7);
    
    DDRD  |= ( 1 << PORTD0 );

    while(1) {

        while (((SPSR >> SPIF) & 1) == 0);
        temp = SPDR;

        if (temp > 55){
            Cooler_DeActivate();
            Heater_DeActivate();
            LED_ON();

        }
        else if ( (temp <= 55) && (temp >= 25) ){
            Heater_DeActivate();
            LED_OFF();
            Cooler_Activate(temp);
        }
        else if( (temp >=20) && (temp < 25)){
             Cooler_DeActivate();
             LED_OFF();
             Heater_DeActivate();
        }
        else if (temp < 20) {
            Cooler_DeActivate();
            LED_OFF();
            Heater_Activate();
        }

    }
}