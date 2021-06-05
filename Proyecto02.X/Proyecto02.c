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
#define _XTAL_FREQ 8000000

//-----------------------------------------------------------------------------
//                            Variables 
//-----------------------------------------------------------------------------

char var;
char var1;
char var2;
char flag_uart;
char dato;
char direccion;
char lectura_01;
char lectura_02;
char lectura_03;
char lectura_04;
char lectura_05;
char lec_pwm1;
char lec_pwm2;

//-----------------------------------------------------------------------------
//                            Prototipos 
//-----------------------------------------------------------------------------
void setup(void);   // Defino las funciones antes de crearla
void chanel(void);
void dedo1_1(void);
void dedo1_2(void);
void dedo1_3(void);
void dedo2_1(void);
void dedo2_2(void);
void dedo2_3(void);
void dedo3_1(void);
void dedo3_2(void);
void dedo3_3(void);
void write_eeprom (char dato, char direccion);
char read_eeprom (char direccion);
void putch(char data);
void menu(void);

//-----------------------------------------------------------------------------
//                            Interrupciones
//-----------------------------------------------------------------------------
void __interrupt() isr(void)
{
        // Interrupcion del ADC
       if(PIR1bits.ADIF == 1){       // Reviso la bandera del ADC
       if(ADCON0bits.CHS == 0) { // Si estoy en el canal 0 
             CCPR2L = (ADRESH>>1)+124;
             lec_pwm1 = ADRESH;
             
           }
       
        else if(ADCON0bits.CHS == 1){        // canal 1 
            CCPR1L = (ADRESH>>1)+124;
            lec_pwm2 = ADRESH;
           }
       
        else if(ADCON0bits.CHS == 2){       // canal 2
            var = ADRESH;
            
           if (var <= 85){
              dedo1_3();
              
                 }
           if ((var <= 170)&&(var >= 86)){  
               dedo1_2();
                 }
            if (var >= 171){
               dedo1_1();
                 }
             }
       else if(ADCON0bits.CHS == 3){        // canal 3
            var1 = ADRESH;

           if (var1 <= 85){
              dedo2_3();
              
                 }
           if ((var1 <= 170)&&(var1 >= 86)){
               dedo2_2();
                 }
            if (var1 >= 171){
               dedo2_1();
                 }
             }
       else if(ADCON0bits.CHS == 4){        // canal 4
            var2 = ADRESH;

           if (var2 <= 85){
              dedo3_3();
                 }
           if ((var2 <= 170)&&(var2 >= 86)){
               dedo3_2();
                 }
            if (var2 >= 171){
               dedo3_1();
                 }
             }
       
           __delay_us(50);
           PIR1bits.ADIF = 0;        // Bajo la bandera del ADC
       }
       
       
       if (RBIF == 1)  // Verificar bandera de la interrupcion del puerto b
    {
        if  (PORTBbits.RB1 == 0)    // Se oprimo el boton 1
        {
            ADCON0bits.ADON = 0;
            PORTBbits.RB7 = 1;  //
            lectura_01 = read_eeprom (0x17);
            lectura_02 = read_eeprom (0x18);
            lectura_03 = read_eeprom (0x19);
            lectura_04 = read_eeprom (0x20);
            lectura_05 = read_eeprom (0x21);
            
            CCPR1L = lectura_04;
            CCPR2L = lectura_05 ;
            
            
            if (lectura_01 <= 85){
              dedo1_3();
              
                 }
           if ((lectura_01 <= 170)&&(lectura_01 >= 86)){  
               dedo1_2();
                 }
            if (lectura_01 >= 171){
               dedo1_1();
                 }
            if (lectura_02 <= 85){
              dedo2_3();
              
                 }
           if ((lectura_02 <= 170)&&(lectura_02 >= 86)){
               dedo2_2();
                 }
            if (lectura_02 >= 171){
               dedo2_1();
                 }
            if (lectura_03 <= 85){
              dedo3_3();
                 }
           if ((lectura_03 <= 170)&&(lectura_03 >= 86)){
               dedo3_2();
                 }
            if (lectura_03 >= 171){
               dedo3_1();
                 }
            __delay_ms(3500);
            ADCON0bits.ADON = 1;
        }
        else if  (PORTBbits.RB0 == 0)    // Se oprimo el boton 2
        {
            PORTBbits.RB6 = 1;  //
            write_eeprom (var,  0x17);
            write_eeprom (var1, 0x18);
            write_eeprom (var2, 0x19);
            write_eeprom (lec_pwm1, 0x20);
            write_eeprom (lec_pwm2, 0x21);
            __delay_ms(500);
        }
        else if (PORTBbits.RB2 == 0){
            flag_uart = 1;      // Se activa la bandera del usart
            
            while (flag_uart == 1){
                TXSTAbits.TXEN = 1;
                menu();     // Entra en un loop desactivando todo lo demas  
            }   // Hasta desactivar la bandera se sale de la uart
            TXSTAbits.TXEN = 0;
        }
        else {
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 0;
        }
        INTCONbits.RBIF = 0;    // Se limpia la bandera de la interrupcion
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
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISBs.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA5 = 1;
    
    // PORTB6 = 0;
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
    WPUBbits.WPUB = 0b00000111;     // Definir cuales pines estan habilitados
    IOCBbits.IOCB = 0b00000111;     // Interrupt on change
    
    // Se configura el oscilador
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;   // Se configura a 8MHz
    OSCCONbits.SCS = 1;
    
    // Configuacion de las interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.RBIF = 1;    // Para que el boton funcione a la primera
    INTCONbits.RBIE = 1;    // Interrupcion Puerto B
    INTCONbits.PEIE = 1;    // Periferical interrupt
    PIE1bits.ADIE = 1;      // Activar la interrupcion del ADC
    PIR1bits.ADIF = 0;      // Bandera del ADC
    PIE1bits.RCIE = 1;      // Interrupcion rx
    PIE1bits.TXIE = 1;      // Interrupcion TX
    
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
  
    // Configuraciones TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 208;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    
    TXSTAbits.TXEN = 0;
    
    PIR1bits.RCIF = 0;  // Bandera rx
    PIR1bits.TXIF = 0;  // bandera tx
}

    
void chanel(void){  // Se crea rutina para cambio de canales
 
    if(ADCON0bits.GO == 0){     // Se crea un cambio de canal
            if(ADCON0bits.CHS == 0){ // si es 1 que se vuelva 0
                ADCON0bits.CHS = 1;
            }
            else if (ADCON0bits.CHS == 1){   // si no es 1, que se vuelva 1
                ADCON0bits.CHS = 2;
            }
            else if (ADCON0bits.CHS == 2){   // si no es 1, que se vuelva 1
                ADCON0bits.CHS = 3;
            }
            else if (ADCON0bits.CHS == 3){   // si no es 1, que se vuelva 1
                ADCON0bits.CHS = 4;
            }
            else if (ADCON0bits.CHS == 4){   // si no es 1, que se vuelva 1
                ADCON0bits.CHS = 0;
            }
            
            __delay_us(50);        // Se espera un tiempo para hacer el cambio
            ADCON0bits.GO = 1;  // Activo las conversiones
        }
}

void dedo1_1(void){
    PORTDbits.RD0 = 1;
    __delay_ms(0.7);
    PORTDbits.RD0 = 0;
    __delay_ms(19.3);
    }

void dedo1_2(void){
    PORTDbits.RD0 = 1;
    __delay_ms(1.5);
    PORTDbits.RD0 = 0;
    __delay_ms(18.5);
    }

void dedo1_3(void){
    PORTDbits.RD0 = 1;
    __delay_ms(2);
    PORTDbits.RD0 = 0;
    __delay_ms(18);
    }

void dedo2_1(void){
    PORTDbits.RD1 = 1;
    __delay_ms(0.7);
    PORTDbits.RD1 = 0;
    __delay_ms(19.3);
    }

void dedo2_2(void){
    PORTDbits.RD1 = 1;
    __delay_ms(1.5);
    PORTDbits.RD1 = 0;
    __delay_ms(18.5);
    }

void dedo2_3(void){
    PORTDbits.RD1 = 1;
    __delay_ms(2);
    PORTDbits.RD1 = 0;
    __delay_ms(17);
    }

void dedo3_1(void){
    PORTDbits.RD2 = 1;
    __delay_ms(0.7);
    PORTDbits.RD2 = 0;
    __delay_ms(19.3);
    }

void dedo3_2(void){
    PORTDbits.RD2 = 1;
    __delay_ms(1.5);
    PORTDbits.RD2 = 0;
    __delay_ms(18.5);
    }

void dedo3_3(void){
    PORTDbits.RD2 = 1;
    __delay_ms(2);
    PORTDbits.RD2 = 0;
    __delay_ms(17);
    }

void write_eeprom (char dato, char direccion){
    EEADR = direccion;      // Donde se escribe lo que se graba
    EEDAT = dato;           // Lo que quiero guardar en la memoria
    
    
    INTCONbits.GIE = 0;     // Se apagan las interrupciones globales
    
    EECON1bits.EEPGD = 0;   // Apuntar a la DATA memory
    EECON1bits.WREN = 1;    // Habilitar la escritura
    
     
    EECON2 = 0x55;          
    EECON2 = 0xAA;          // El anterior en hexa
    
    EECON1bits.WR = 1;      // Se inicia la escritura
    
    while(PIR2bits.EEIF == 0);//Para darle tiempo hasta que termine la escritura
    PIR2bits.EEIF = 0;
    
    EECON1bits.WREN = 0;    // Para asegurar que no se este escribiendo
}

char read_eeprom (char direccion){
    EEADR = direccion;      // El byte de memoria que se lee
    EECON1bits.EEPGD = 0;   // Apunta hacia el Program memory
    EECON1bits.RD = 1;      // Modo lectura activado
    char dato = EEDATA;     // Se guarda el dato en la variable
    return dato;            // la operacion regresa a la variable
}

// Se crea debido a que se usa printf
void putch(char data){      // Funcion especifica de libreria stdio.h
    while(TXIF == 0);
    TXREG = data; // lo que se muestra de la cadena
    return;
}

// Funcion donde se definen las cadenas mediante printf
void menu(void){
    __delay_ms(250); //Tiempos para el despliegue de los caracteres
    printf("\n -----------Bienvenido----------- \n");
    __delay_ms(250);
    printf("\n Elija una de las siguientes opciones: \n");
    __delay_ms(250);
    printf(" 1. Mover motores individuales \n");
    __delay_ms(250);
    printf(" 2. Encender leds \n");
    __delay_ms(250);
    printf(" 3. Salir de la terminal \n");

    while (RCIF == 0);
    // Se establecen las opciones del menu
    if (RCREG == '1'){
        __delay_ms(500);
        printf("\n Que motor desea mover? \n");
        __delay_ms(250);
        printf(" a. Mover motor 1 \n");
        __delay_ms(250);
        printf(" b. Mover motor 2 \n");
        __delay_ms(250);
        printf(" c. Mover motor 3 \n");
        __delay_ms(250);
        
        while (RCIF == 0);
        
        if (RCREG == 'a'){
            dedo1_1();
            __delay_ms(1000);
            dedo1_2();
        }
        if (RCREG == 'b'){
            dedo2_1();
            __delay_ms(1000);
            dedo2_2();
        }
        if (RCREG == 'c'){
            dedo3_1();
            __delay_ms(1000);
            dedo3_2();
        }        
        else{
            NULL;
        }
    }
    if (RCREG == '2'){
        PORTBbits.RB6 = 1;
        __delay_ms(500);
        PORTBbits.RB6 = 0;
    }
    
    if (RCREG == '3'){ //tercera opcion del menu
        printf("\r Esta seguro? \n");
        __delay_ms(250);
        printf(" d. Si \n");
        __delay_ms(250);
        printf(" e. No \n");
        __delay_ms(250);
        
        while (RCIF == 0);
        if (RCREG == 'd'){
            flag_uart = 0;
//            TXSTAbits.TXEN = 0;
            printf(" Gracias por todo \n");
            __delay_ms(250);
            
        }
        else if (RCREG == 'e'){
            NULL;
        }
    }
    else{ //Si se ingresa una popcion fuera de la lista, no sucede nada
        NULL;       // equivalente a un nop
    }
    return;
}