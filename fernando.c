     /*
Programa pensado para PIC 18F4550 que pretende leer tres sensores en
AN0, AN1 y AN2 y sacar los porcentajes de lectura por un display 16x2.
Cada sensor tiene un voltaje de referencia distinto, el cual es switcheado por
compuertas HEF4660, las cuales alimentan a su vez a VREF+ en el PIC.
Los enables de las tres compuertas son controlados por PORTB, el cual se
asegura de que solo una de las tres esté activa al momento de una lectura para
que los voltajes no se sumen.

Este programa es de libre uso y distribución.

"El conocimiento humano pertenece al mundo"

Fernando González. IPN UPIICSA

*/


sbit LCD_RS at RD2_bit;
sbit LCD_EN at RD3_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D7 at RD7_bit;

sbit LCD_RS_Direction at TRISD2_bit;
sbit LCD_EN_Direction at TRISD3_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;

void sensorBit(int sen) {
    int sample10b = 0;
    int sample8b = 0;
    int sigVRef;
    //float porcentaje10b = 0;
    //float porcentaje8b = 0;
    //char p1[16];
    //char p2[16];
    char s10b[16];
    char s8b[16];

    sample10b = ADC_read(sen); //lectura analógica del puerto en turno
    sample8b = sample10b >> 2; //se hace un recorrimiento a la derecha

    /*
    //obtenemos porcentajes del sampleo
    porcentaje10b = (sample10b / 1024.0) * 100;
    porcentaje8b = (sample8b / 256.0) * 100;

    //reutilizaremos variables para almacenar el porcentaje como entero
    //y luego convertirlos a cadena de texto
    sample10b = porcentaje10b;
    sample8b = porcentaje8b;
    */
    
    inttostr(sample10b, s10b);
    inttostr(sample8b, s8b);
    

    switch(sen) {
        case 0:
            //lcd_cmd(_LCD_CLEAR);
            lcd_out(1, 1, s10b);
            lcd_out(2, 1, s8b);
            sigVRef = 0b01000000; //RB6 en alto
            break;

        case 1:
            lcd_out(1, 7, s10b);
            lcd_out(2, 7, s8b);
            sigVRef = 0b10000000; //RB7 en alto
            break;

        case 2:
            lcd_out(1, 13, s10b);
            lcd_out(2, 13, s8b);
            sigVRef = 0b00100000; //RB5 en alto
            break;

        default: ;//ignorado, no hay caso improvisto
    }
    PORTB = sigVRef;
}

void sensorPor(int sen) {
    int sample10b = 0;
    //int sample8b = 0;
    int sigVRef;
    float porcentaje10b = 0;
    //float porcentaje8b = 0;
    char p1[16];
    //char p2[16];

    sample10b = ADC_read(sen); //lectura analógica del puerto en turno
    //sample8b = sample10b >> 2; //se hace un recorrimiento a la derecha

    //obtenemos porcentajes del sampleo
    porcentaje10b = (sample10b / 1024.0) * 100;
    //porcentaje8b = (sample8b / 256.0) * 100;

    //reutilizaremos variables para almacenar el porcentaje como entero
    //y luego convertirlos a cadena de texto
    sample10b = porcentaje10b;
    //sample8b = porcentaje8b;

    intToStr(sample10b, p1);
    //intToStr(sample8b, p2);

    switch(sen) {
        case 0:
            LCD_out(1, 1, p1);
            sigVRef = 0b01000000; //RB6 en alto
            break;

        case 1:
            LCD_out(1, 7, p1);
            sigVRef = 0b10000000; //RB7 en alto
            break;

        case 2:
            lcd_out(1, 13, p1);
            sigVRef = 0b00100000; //RB5 en alto
            break;

        default: ;//ignorado, no hay caso improvisto
    }
    PORTB = sigVRef;
}

void main() {
    int i = 0, estado = 0, flag = 0;
    ADCON1 = 0b00011100; //VREF+ en AN3; AN0, AN1 y AN2 entradas analógicas
    TRISB = 0;  //PORTB será el encargado de activar el switch 4066
    PORTB = 0b00100000; //PORTB inicia con RB5 en alto
    TRISE = 1;
    LCD_init();
    LCD_cmd(_LCD_CURSOR_OFF);
    LCD_cmd(_LCD_SHIFT_LEFT);
    LCD_cmd(_LCD_SHIFT_LEFT);

    while(1) {
        estado = PORTE.b0;
        
        if(estado == 1) { //Si es 1, quiere decir que el botón fue apretado
            flag = 1 - flag; //Si flag era 1, cambia a 0 y viceversa
            delay_ms(100);   //Cada que el botón es apretado, flag cambia
        }
        estado = 0; //El estado vuelve a ser lo que normalmente recibe RE1

        if(flag == 1)
            for(i = 0; i < 3; i++)
                sensorBit(i);
        else {
            lcd_out(2, 1, "  H21A1  IR  CNY70   ");
            for(i = 0; i < 3; i++)
                sensorPor(i);
        }
    }
}//main