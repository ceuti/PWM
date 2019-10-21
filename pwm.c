/*
 * File:   ADC.c
 * Author: Pedro
 *
 * Created on 22 de diciembre de 2017, 11:03
 */
// CONFIG1
#pragma config FOSC = INTOSC       //  (ECH, External Clock, High Power Mode (4-32 MHz); device clock supplied to CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOREN = OFF    // Low Power Brown-out Reset enable bit (LPBOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)
#define _XTAL_FREQ 1000000

#include <xc.h>
#include <htc.h>
#include "pic12f1572.h"

int resultado,resultado1,a;

void main(void) {
    //configuraci�n del osclador
    //*****************
    OSCCONbits.SPLLEN=0;
    OSCCONbits.IRCF=0b1011;
    OSCCONbits.SCS=0b00;
    OSCSTATbits.OSTS=1;
    //*****************
    //Configuraci�n de los puertos 
    TRISA=0b000000;
    TRISAbits.TRISA4=1;//RAA 4 entrada
    TRISAbits.TRISA5=0; //como salida para poder usar el PWM
    ANSELAbits.ANSA4=1;//entrada anal�gica en RA4
    WPUAbits.WPUA4=0;//desactivo Pull Up del pin
    
    //configuraci�n del puerto RA2 para INT. 
    //configuraci�n de interrupcioones
    CWG1CON0bits.G1EN=0; //desactivo el CWG del RA2.
    PWM3CONbits.EN=0;// DESACTIVO EL PWM3, QUE ES EL ASIGNADO AL RA2
    PWM3CONbits.OE=0;//desactivo el PWM3
    ANSELAbits.ANSA2=0; //fijamos RA2 como entrada digital.
    
    CM1CON0bits.C1OE=0; //desactivo el comparador asignado RA2.
    INTCONbits.GIE=0;// desactivo la interrupciones, no es necesario la interrupci�n
    //nos vale con trabajar el FLAG.
    INTCONbits.INTE=0;//activo o desactivo  la interrupci�n externa, INT externa desactivada
    //redundante con la instrucci�n anterior. 
    INTCONbits.INTF=0;//flag de la interrupci�n externa
    INTCONbits.IOCIE=0;//este bit me sirve para desactivar la interrupci�n de cambio 
    OPTION_REGbits.INTEDG=0;//la interrupci�n externa est� activada por flanco ascendente
    TRISAbits.TRISA2=1; //fijamos RA2 como entrada. 
  
    OPTION_REGbits.nWPUEN=0;//funciona con l�ginca negativa, resistencias pull ups activadas. 
    WPUAbits.WPUA2=1; /* hacemos uso de resistencia interna pull-up.
    esto nos permite ahorrar en hardware ya que s�la mente har� falta conectar
    el interruptor hacia masa */
   
    //***************************
    
    //Configuraci�n del AD
    ADCON1bits.ADFM=1;//justificaci�n derecha
    ADCON1bits.ADCS=0b000;//Frecuencia de muestreo Fsco/2
    ADCON1bits.ADPREF=0b00;//Cojo la alimentaci�n como referencia.
    ADCON0bits.CHS=0b00011;// configuro la entrada analogica en el pin3 (RA4/AN3)
    ADCON0bits.ADON=1;//activa el conversor analogico digital
    //Fin de configuraci�n AD
    
    //Configurac�n PWM
    APFCONbits.P1SEL=1;//Uso el bit PWM 1 en el puerto RA5
    //Configuro los bits de PWM1CON por separado para mayor claridad
    PWM1CONbits.EN=0; //PWM1 esta activado.
    PWM1CONbits.OE=1; // El bit de salida, PWM1CONbits.5 esta activado
    PWM1CONbits.POL=0; //Salida activa a nivel alto
    PWM1CONbits.MODE=0b00; //PWM en modo estandar.
    //PWM1INTE controla las interrupciones por coindicendia de Offset, Phase
    //Duty Cicle y Periodo. Dejo ( por ahora) todas descativadas.
    PWM1INTE=0;//la interrupci�n de uso de PWM est� descativada. 
    //Configuracion del reloj
    PWM1CLKCONbits.PS=0b001; //Sin prescaler
    PWM1CLKCONbits.CS=0b00; //Usamos el oscilador interno.
    
    //fuente de disparao OFFSET registro de selecci�n
    PWM1OFCONbits.OFM=0b00; //Modo independiente
    PWM1OFCONbits.OFS=0b00; //reservado
    
    //Espacio reservado para PWM1LDCON
    PWM1LDCONbits.LDT=0;
    //ESPACIO reservado para PWM1PH (registro de fase)
    PWM1PH=0; // // pruebo con la fase igual a cero.
    //Configuro el periodo para 20 ms.
    PWM1PR=10000;
    a=0;
    while (1){ //while (1), bucle infinito.
    ADCON0bits.GO_nDONE=1;// Pongo ADCON0bits a 1, cuando cambie a 0 es porque ha terminado la conversi�n.
    while (ADCON0bits.GO_nDONE==1);//mientras sea 1 no ha terminado la conversi�n, esperamos.
     if (ADRES != resultado || a ==0){//entra en este if si el resultado de la conversi�n es distinto al anterior o si es la primera vez
         a=1;//lo cambio a 1 para siempre, a=0 nos ayuda a entrar la primera vez.
         resultado=ADRES;//guardo el resultado de la conversi�n del ADC
         resultado1=(resultado*0.488)+500;//transformo la escala de tiempo
        PWM1DC=resultado1;//guardo el resultado de la conversi�n en PWM1DC, registro que controla el "duty cycle"
        PWM1CONbits.EN=0;
         }
     PWM1LDCONbits.LDA=1;
     PWM1CONbits.EN=1;
     while (PWM1INTFbits.PRIF==0);
     PWM1INTF=0;
     //En la siguiente instrucci�n se pretende hacer la funci�n de centrado del servo
     //el programa s�lo entrar� en este if cuando el FLAG INT sea 1, despues de pulsar el 
     // el pulsador. 
     if (INTCONbits.INTF==1){
         __delay_ms(1000);//retraso antirebote
         INTCONbits.INTF=0;//pongo a cero el flago.
         PWM1DC=749.5;//cargo el valor que dar� el centrado del servo, 1`5 ms.
         PWM1CONbits.EN=0;//se descativa el m�dulo PWM1
         PWM1LDCONbits.LDA=1;//carga los valore de DC, PH y Offset
         PWM1CONbits.EN=1;//se activa el m�dulo para cargar el centrado
         while(INTCONbits.INTF==0);//paro el programa hasta que se vuelva a pulsar el pulsador
         __delay_ms(1000);//retraso antirebote
         INTCONbits.INTF=0;//vuelvo a poner a cero la bandera de interrupci�n
     } 
    }
    
    
    return;
}
           


