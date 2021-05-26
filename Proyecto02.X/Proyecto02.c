/*
 * File:   Proyecto02.c
 * Author: marco
 *
 * Created on May 18, 2021, 1:35 AM
 */
//  Bits de configuracion   //
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf

// Defino los bits en palabras claves
#define _XTAL_FREQ 4000000
#define Motor01 PORTDbits.RD0 // RD0
#define Motor02 PORTDbits.RD1 // RD1
#define Motor03 PORTDbits.RD2 // RD2


//-----------------------------------------------------------------------------
//                            Variables 
//-----------------------------------------------------------------------------
volatile uint8_t var;
volatile char angle[] = { 64, 64, 64 };
char DutyCycle(char carga);

//-----------------------------------------------------------------------------
//                            Prototipos 
//-----------------------------------------------------------------------------
void setup(void);   // Defino las funciones antes de crearlas
void pwm(void);
void chanel(void);

//-----------------------------------------------------------------------------
//                            Interrupciones
//-----------------------------------------------------------------------------
void __interrupt() isr(void)
{
        // Interrupcion del ADC
       if(PIR1bits.ADIF == 1){       // Reviso la bandera del ADC
       if(ADCON0bits.CHS == 0) { // Si estoy en el canal 0 
             CCPR2L = (ADRESH>>1)+124;}
           
       
        else{         
            CCPR1L = (ADRESH>>1)+124;
           }
       
           PIR1bits.ADIF = 0;        // Bajo la bandera del ADC
       }
       
       if(INTCONbits.T0IF){
//           
           switch(var){
               case 0: 
                   Motor01 = 1;
                   TMR0 = 0-DutyCycle(angle[var]);
                   break;
               case 1: 
                   Motor01 = 0;
                   Motor02 = 1;
                   TMR0 = 0-DutyCycle(angle[var]);
                   break;
               case 2: 
                   Motor02 = 0;
                   Motor03 = 1;
                   TMR0 = 0-DutyCycle(angle[var]);
                   break;
               case 3: 
                   Motor03 = 0;
                   TMR0 = 0;
                   break;                   
           }
           var++;
           T0IF = 0;
                   
       }
    }

//-----------------------------------------------------------------------------
//                            Main
//-----------------------------------------------------------------------------
void main(void) {
    
    setup();    // Llamo a mi configuracion
    
    while(1)    // Equivale al loop
    {
      chanel();  
      RD0 = 1;
      __delay_us(50);
      RD0 = 0;
      
       
    }
}

void setup(void){
    
    // Configuraciones de puertos digitales
    ANSEL = 0b00011111;
    ANSELH = 0;
    
    // PORTA
    // Configurar bits de salida o entradaas
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA5 = 1;
    
    // PORTB
    // Configurar bits de salida o entradaas
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    
    // PORTD
    // Configurar bits de salida o entradas
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;    
    
    // Se limpian los puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    // Configuracion de Pull Up puerto B
    OPTION_REGbits.nRBPU = 0;       // Habilitar pullup interno
    WPUBbits.WPUB = 0b00000011;    
    
    // Se configura el oscilador
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;   // Se configura a 8MHz
    OSCCONbits.SCS = 1;
    
    // Configuacion de las interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;    // Periferical interrupt
    PIE1bits.ADIE = 1;      // Activar la interrupcion del ADC
    PIR1bits.ADIF = 0;      // Bandera del ADC
    
    // Configuracion del ADC
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 1;       // FOSC/32
    ADCON0bits.ADON = 1;        // Activar el ADC  
    ADCON0bits.CHS = 0;         // Canal 0
    __delay_us(50);
    
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON1bits.VCFG0 = 0;       // Volataje de referencia vss y vddd
    ADCON1bits.VCFG1 = 0;
    
    // Configuracion del PWM
    TRISCbits.TRISC1 = 1;
    TRISCbits.TRISC2 = 1;
    
    PR2 = 250;                  // Config periodo
    CCP1CONbits.P1M = 0;        // config modo pwm
    CCP1CONbits.CCP1M = 0b1100;     // configuracion del pwm 1
    CCP2CONbits.CCP2M = 0b1100;     // configuracion del pwm 2
    
    CCPR1L = 0x0f;              // ciclo trabajo normal
    CCP1CONbits.DC1B = 0;
    
    PIR1bits.TMR2IF = 0;         // apagamos bandera
    T2CONbits.T2CKPS = 0b11;     // prescaler 1:16
    T2CONbits.TMR2ON = 1;
    
    while(PIR1bits.TMR2IF == 0);
    PIR1bits.TMR2IF = 0;
    
    // salidas del PWM
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
  
}


void chanel(void){
 
    if(ADCON0bits.GO == 0){     // Se crea un cambio de canal
            if(ADCON0bits.CHS == 1) // si es 1 que se vuelva 0
                ADCON0bits.CHS = 0;
            else                // si no es 1, que se vuelva 1
                ADCON0bits.CHS = 1;
            
            __delay_us(100);        // Se espera un tiempo para hacer el cambio
            ADCON0bits.GO = 1;  // Activo las conversiones
        }
    
}

char DutyCycle(char carga){
    return  carga+31;
}

