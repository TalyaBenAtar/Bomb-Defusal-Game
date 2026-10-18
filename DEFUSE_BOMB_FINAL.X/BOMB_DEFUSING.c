#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/attribs.h>
#include <string.h>
#include <p32xxxx.h>
#include <time.h>    // For time
#include "ssd.h" //for 7 segment desplay
#include "ssd.c"

// Updated Configuration Bits
#pragma config JTAGEN = OFF       // Disable JTAG to free shared pins
#pragma config FWDTEN = OFF       // Disable Watchdog Timer
#pragma config FNOSC = FRCPLL     // Oscillator selection: Fast RC with PLL
#pragma config FSOSCEN = OFF      // Disable Secondary Oscillator
#pragma config POSCMOD = EC       // Primary Oscillator Mode: External Clock
#pragma config OSCIOFNC = ON      // Enable CLKO (Oscillator Output)
#pragma config FPBDIV = DIV_1     // Peripheral Clock Divisor: 1
#pragma config FPLLIDIV = DIV_2   // PLL Input Divider: Divide by 2
#pragma config FPLLMUL = MUL_20   // PLL Multiplier: Multiply by 20
#pragma config FPLLODIV = DIV_1   // PLL Output Divider: Divide by 1


// Define macros for button pins
#define BTN_UP    PORTBbits.RB1
#define BTN_LEFT  PORTBbits.RB0
#define BTN_CENTER PORTFbits.RF0
#define BTN_RIGHT PORTBbits.RB8
#define BTN_DOWN  PORTAbits.RA15

// Define macros for RGB pins
#define RED_OC   OC3RS
#define GREEN_OC OC5RS
#define BLUE_OC  OC4RS
#define true 1
#define false 0

int fastModeOn = false;
int FAST_MODE_DELAY = 500;
int SLOW_MODE_DELAY = 3000;

int beeperON = true;
int beepAlt =true;

int DELAY= 1000;
int DELAY_BEEPER=200;

//int CUTTING_ORDER=0;
//int WRONG_CUTS=0;
//int SUCCESS=0;
//int PI_DIGIT=0;
//int onesInPI=0;
char globalResult[20]="\0";
//char LETTER;
//int WIRE_ATTEMPTS= 0;
//#define wireAttempts WRONG_CUTS+SUCCESS

int TIME=80;
int clockCount = 0;

//int wires[8]= {0,0,0,0,0,0,0,0}; //0- wire isnt in the sequence. 1-4 is the wire's placment in the sequence
//int wireCut[8]= {0,0,0,0,0,0,0,0}; //0- not cut, 1- cut
//int cuttingOrder[8]= {0,0,0,0,0,0,0,0};


#define PRESSED 1
#define RELEASED 0 
int eventStateKeys[4] = {0,0,0,0}; // 0 - no event, 1 low to high,2 high to low
//int keyHistory[4] = {1,1,1,1}; // 1 NOT PRESSED, 0 PRESSED
int evenStateButons[5]={0,0,0,0,0};
int TIMER2DISABLED = false;
void configureLeds(){
    TRISA &= 0xff00; // Configure PORTA as output
    PORTA = 0x00;    // Initialize PORTA to 0
}

void configureKeys(){
    TRISCbits.TRISC2=0;//RC2            
    TRISCbits.TRISC1=0;//RC1           
    TRISCbits.TRISC4=0;//RC4            
    TRISGbits.TRISG6=0;//RG6
    ANSELGbits.ANSG6=0;//???????
    TRISCbits.TRISC3 =1;//RC3
    CNPUCbits.CNPUC3;               
    TRISGbits.TRISG7=1;//RG7
    ANSELGbits.ANSG7=0;
    CNPUGbits.CNPUG7;                  
    TRISGbits.TRISG8=1;//RG8
    ANSELGbits.ANSG8=0;//???????
    CNPUGbits.CNPUG8;//?????
    TRISGbits.TRISG9=1;//RG9
    ANSELGbits.ANSG9=0;//???????
    CNPUGbits.CNPUG9;//????? 
    
    PORTCbits.RC2=0;
    PORTCbits.RC1=0;
    PORTCbits.RC4=0;
    PORTGbits.RG6=0;
}

void configureSwitches(){
     // Configure switches as inputs TODO put in function called configure switches
    TRISFbits.TRISF3 = 1; // RF3 (SW0)
    TRISFbits.TRISF5 = 1; // RF5 (SW1)
    TRISFbits.TRISF4 = 1; // RF4 (SW2)
    TRISDbits.TRISD15 = 1; // RD15 (SW3)
    TRISDbits.TRISD14 = 1; // RD14 (SW4)
    TRISBbits.TRISB11 = 1; // RB11 (SW5)
    TRISBbits.TRISB10 = 1; // RB10 (SW6)
    TRISBbits.TRISB9 = 1;  // RB9 (SW7)
    ANSELBbits.ANSB11 = 0; // Disable analog for RB11
    ANSELBbits.ANSB10 = 0; // Disable analog for RB10
    ANSELBbits.ANSB9 = 0;  // Disable analog for RB9
}
int getSwitchState(int index){
    switch (index) {
        case 0:
            return PORTFbits.RF3;
        case 1:
            return PORTFbits.RF5;
        case 2:
            return PORTFbits.RF4;
        case 3:
            return PORTDbits.RD15;
        case 4:
            return PORTDbits.RD14;
        case 5:
            return PORTBbits.RB11;
        case 6:
            return PORTBbits.RB10;
        case 7:
            return PORTBbits.RB9;
    }
}
void configureSwitchesToLeds(){
    PORTAbits.RA0 = !PORTFbits.RF3;
    PORTAbits.RA1 = !PORTFbits.RF5;
    PORTAbits.RA2 = !PORTFbits.RF4;
    PORTAbits.RA3 = !PORTDbits.RD15;
    PORTAbits.RA4 = !PORTDbits.RD14;
    PORTAbits.RA5 = !PORTBbits.RB11;
    PORTAbits.RA6 = !PORTBbits.RB10;
    PORTAbits.RA7 = !PORTBbits.RB9;
}

void configureSwitchesToLeds1(int index){
    switch (index) {
        case 0:
            PORTAbits.RA0=0;
            return;
        case 1:
            PORTAbits.RA1=0;
            return;
        case 2:
            PORTAbits.RA2=0;
            return;
        case 3:
            PORTAbits.RA3=0;
            return;
        case 4:
            PORTAbits.RA4=0;
            return;
        case 5:
            PORTAbits.RA5=0;
            return;
        case 6:
            PORTAbits.RA6=0;
            return;
        case 7:
            PORTAbits.RA7=0;
            return;
    }
    

}

void configureBeeper(){
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;
    PORTBbits.RB14 = 0;
}

void aultranteBeeper(){
    PORTBbits.RB14 = !PORTBbits.RB14;
}

void isScreenBusy(void)
{
    char RD,RS;
    int STATUS_TRISE;
    int portMap;
    RD=PORTDbits.RD5;
    RS=PORTBbits.RB15;
    STATUS_TRISE=TRISE;
	PORTDbits.RD5 = 1;//w/r
	PORTBbits.RB15 = 0;//rs 
    portMap = TRISE;
	portMap |= 0x80;
	TRISE = portMap;
    do
    {
         PORTDbits.RD4=1;//enable=1
         PORTDbits.RD4=0;//enable=0
    }
    while(PORTEbits.RE7); // BF ?????
    PORTDbits.RD5=RD; 
    PORTBbits.RB15=RS;
    TRISE=STATUS_TRISE;   
 }
void configureScreenForArrowsGame(){
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // Disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0;  // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0;  // RD4 (DISP_EN) set as an output
    TRISE &= 0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    ANSELEbits.ANSE7 = 0;
    PORTBbits.RB15 = 0; // rs=0
    PORTDbits.RD5 = 0;  // w=0
    
    char control[7] = {0x38, 0x38, 0x38, 0xe, 0x6, 0x1, 0x40}; // set CGRAM=0x40
    for (int i = 0; i < 7; i++) {
        PORTE = control[i];
        PORTDbits.RD4 = 1;
        PORTDbits.RD4 = 0;
        busy();
    }
}

void configureScreen(){
     int j,i;
    char string[]="hello!!";
    char control[]={0x38,0x38,0x38,0xe,0x6,0x1};
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    //TRISEbits.TRISE0 = 1; // RE0 (DB0) set as input (change 1 to 0 for
    
    TRISE&=0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE3 = 0;
    TRISEbits.TRISE4 = 0;
    TRISEbits.TRISE5 = 0;
    TRISEbits.TRISE7 = 0;
    
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//w=0
    ANSELEbits.ANSE7 = 0;

    for(i=0;i<6;i++)
    {
        PORTE=control[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        isScreenBusy();
    }
    PORTBbits.RB15=1;//rs=0
    PORTDbits.RD5=0;//w=0
}

void setControl(char ctrl){
    PORTBbits.RB15=0;
    PORTE = 0;
    PORTE = ctrl;

    PORTDbits.RD4=1;
    PORTDbits.RD4=0;
    isScreenBusy();
    PORTBbits.RB15=1;
}

void setText(char* string,char ctrl){
    setControl(ctrl);
     for(int i=0;i< strlen(string);i++)
    {
        PORTE=string[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        isScreenBusy();
    }
}

void resetScreen(){
    PORTBbits.RB15=0;
//    PORTEbits.RE0 = 1;
    PORTE = 1;
    
    PORTDbits.RD4=1;
    PORTDbits.RD4=0;
    isScreenBusy();
    PORTBbits.RB15=1;
}

void busy(void) {
    char RD, RS;
    int STATUS_TRISE;
    int portMap;

    RD = PORTDbits.RD5;
    RS = PORTBbits.RB15;
    STATUS_TRISE = TRISE;

    PORTDbits.RD5 = 1;   // w/r
    PORTBbits.RB15 = 0;  // rs

    portMap = TRISE;
    portMap |= 0x80;
    TRISE = portMap;

    do {
        PORTDbits.RD4 = 1; // enable=1
        PORTDbits.RD4 = 0; // enable=0
    } while (PORTEbits.RE7); // Check busy flag (BF)

    PORTDbits.RD5 = RD;
    PORTBbits.RB15 = RS;
    TRISE = STATUS_TRISE;
}
int keyBoardPressed(int index){
   switch (index) {
        case 0:
            return !PORTGbits.RG9;
        case 1:
            return !PORTGbits.RG8;
        case 2:
            return !PORTGbits.RG7;
        case 3:
            return !PORTCbits.RC3;
    }
}

int buttonPressed(int index){
   switch (index) {
        case 0:
            return BTN_UP;
        case 1:
            return BTN_LEFT;
        case 2:
            return BTN_RIGHT;
        case 3:
            return BTN_DOWN;
        case 4:
            return BTN_CENTER;
    }
}

void splitNumber(int minutes,int seconds, int *d1, int *d2,int *d3, int *d4) {
//    *d1 = num / 1000;         // Extracts the first digit
//    *d2 = (num / 100) % 10;   // Extracts the second digit
//    *d3 = (num / 10) % 10;    // Extracts the third digit
//    *d4 = num % 10;           // Extracts the fourth digit
    *d1 = minutes / 10;  // Tens place of minutes
    *d2 = minutes % 10;  // Ones place of minutes
    *d3 = seconds / 10;  // Tens place of seconds
    *d4 = seconds % 10;  // Ones place of seconds
}

int wasKeyPressed(char key){
    return eventStateKeys[key -'A'] == PRESSED;
}

//void releaseKey(char key){
//    keyHistory[key -'A'] == PRESSED;
//}

//int wasKeyReleased(char key){
//    return keyHistory[key -'A'] == PRESSED;
//}

void resetAllKeyEvents(){
    for(int i = 0 ; i < 4 ; i++)
        eventStateKeys[i] = RELEASED;
}

void checkKeyBoardForEvents(){
    for(int i = 0 ; i < 4 ; i++){
        if(keyBoardPressed(i)){ 
            eventStateKeys[i] = PRESSED;
           // keyHistory[i] = RELEASED;
        }
    }
}

void resetAllButtonEvents(){
    for(int i = 0 ; i < 5 ; i++)
        evenStateButons[i] = RELEASED;
}

int wasButtonPressed(int button){
    return evenStateButons[button] == PRESSED;
}

void checkButtonsForEvents(){
    for(int i = 0 ; i < 5 ; i++){
        if(buttonPressed(i)){ 
            evenStateButons[i] = PRESSED;
           // keyHistory[i] = RELEASED;
        }
    }
}

void displayTime(int freez){
    clockCount++;
    if(clockCount%4!=0)
        return;
    if(!freez){
        TIME--;
    }
    int time[]={0,0,0,0};
    
    playBeeper();
    // Convert TIME (in seconds) to MM:SS format
    int minutes = TIME / 60;  // Get minutes
    int seconds = TIME % 60;  // Get remaining seconds
    
    if (TIME>=0 && TIME<10000){
        splitNumber(minutes,seconds,&time[0], &time[1],&time[2], &time[3]);
    }   
    SSD_WriteDigits(time[3],time[2],time[1],time[0],0,0,1,0);
}

void setTime(){
    if(TIMER2DISABLED)
        return;

    
    displayTime(false);
  
    if (!TIME){
        bombExplodes(1);
    }
    
    if (TIME<=10) {
       last10Sec();
       playBeeper();
    }

} 
void last10Sec(){
    RGB_SetColor(3999,0,0);
    
    //ADD ALARM
}

void delayWithEvents(){
 //   int delay_count = DELAY;
    for (int forCounter = 0; forCounter < DELAY; forCounter++){
     //   playBeeper();
        //setTime();
        checkKeyBoardForEvents();
        checkButtonsForEvents();
    }
}

//void specialText(char *str, char *strOp3, char *strOp4, char *strOp34 , int pos){
//    if (reverse && fastModeOn){
//        setText(strOp34,0xc0+pos);
//        
//    } else if (reverse){
//        setText(strOp3,0xc0+pos);
//        
//    }else if (fastModeOn){
//        setText(strOp4,0xc0+pos);
//        
//    } else setText(str,0xc0+pos);
//    
//}


void delayWithCount(int count){
    for (int forCounter = 0; forCounter < count; forCounter++);
}


void playBeeper(){    
    if(beeperON){
        if(beepAlt){
            for(int i=0; i< 20;i++){
                    PORTBbits.RB14=1;
                    delayWithCount(100);
                    PORTBbits.RB14=0;
                    delayWithCount(100);            
            }
        }
        else{
            for(int i=0; i< 40;i++){
                PORTBbits.RB14=1;
                delayWithCount(50);
                PORTBbits.RB14=0;
                delayWithCount(50);            
            }
        }
    }
    beepAlt = !beepAlt;
}


void ADC_Init(){

    AD1CON1	=0; 
    AD1CON1bits.SSRC = 7;   // Internal counter ends sampling and starts conversion (auto convert)
    AD1CON1bits.FORM = 0;   // Integer 16-bit
	// Setup for manual sampling
    AD1CSSL	=0;
    AD1CON3	=0x0002;     // ADC Conversion Clock Select bits: TAD = 6 TPB
    AD1CON2	=0;
    AD1CON2bits.VCFG = 0;   // Voltage Reference Configuration bits: VREFH = AVDD and VREFL = AVSS
	// Turn on ADC
    AD1CON1bits.ON = 1;
} 

unsigned int ADC_AnalogRead(unsigned char analogPIN)
{
    int adc_val = 0;
    
    IEC0bits.T2IE = 0;
    AD1CHS = analogPIN << 16;       // AD1CHS<16:19> controls which analog pin goes to the ADC
 
    AD1CON1bits.SAMP = 1;           // Begin sampling
    while( AD1CON1bits.SAMP );      // wait until acquisition is done
    while( ! AD1CON1bits.DONE );    // wait until conversion is done
 
    adc_val = ADC1BUF0;
    IEC0bits.T2IE = 1;
    return adc_val;
}


void displayAnalogValue(char *strStart,char *strEnd, int value, int placement) {
    char valueStr[4];
    char result[50]= "\0";
    
    // Convert the int to a string
    sprintf(valueStr, "%d", value); // "%.2f" limits to 2 decimal places
    
    strcat(result, strStart);
     
    strcat(result, valueStr);
   
    strcat(result, strEnd);
    
   // setText(valueStr, 0x80);
    setText(result, 0xc0);

}

void setMic(){
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 1;
    ADC_Init();
}

void microphoneVolt(){
    int x= ADC_AnalogRead(4);//4 for microphone input RB4
    displayAnalogValue("Analog 2- <", ">V", x, 0xc0 );
    PORTA=x/4;
}

void setResistor(){
    TRISBbits.TRISB2 = 1;
    ANSELBbits.ANSB2 = 1;
    ADC_Init();
}

int resistor(){
    int x= ADC_AnalogRead(2);//2 for resistor input RB4
    PORTA=x/4;
    return x;
}


void originalFunction(){
    int x;
    TRISA &= 0xff00;
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 1;
    ADC_Init();
    while(1)
    {
      x= ADC_AnalogRead(4);//2 for IN analog RB2, 4 for microphone input RB4
      PORTA=x/4; 
        
    }
}

void originalKeyFunction(){
    while(1){
        //original from menachem:
            PORTCbits.RC2=0;
            PORTCbits.RC1=0;
            PORTCbits.RC4=0;
            PORTGbits.RG6=0;
            
            PORTAbits.RA0=PORTCbits.RC3;//led 0 is controlled by F,E,D
            PORTAbits.RA1=PORTGbits.RG7;//led 1 is controlled by 8,9,C
            PORTAbits.RA2=PORTGbits.RG8;//led 2 is controlled by 5,6,B
            PORTAbits.RA3=PORTGbits.RG9; //led 3 is controlled by 2,3,A
            
    }
}
   // Initialize Peripheral Pin Select (PPS)
void PPS_Init(void) {
    RPD2R = 0b1011;  // Map OC3 to RD2 (Red)
    RPD12R = 0b1011; // Map OC4 to RD12 (Green)
    RPD3R = 0b1011;  // Map OC5 to RD3 (Blue)
}

// Set RGB LED color
void RGB_SetColor(uint16_t red, uint16_t green, uint16_t blue) {
    RED_OC = red;      // Set red duty cycle (0 to PR2)
    GREEN_OC = green;  // Set green duty cycle (0 to PR2)
    BLUE_OC = blue;    // Set blue duty cycle (0 to PR2)
}



void __ISR(_TIMER_2_VECTOR, ipl2auto) Timer2SR(void) {
   setTime();
   IFS0bits.T2IF = 0; // Clear interrupt flag
}

// Initialize Timer 2 for PWM
//void Timer2_Init(void) {
//    T2CON = 0;             // Clear the Timer 2 control register
//    T2CONbits.TCKPS = 2;   // Set prescaler to 1:4
//    PR2 = 3999;            // Set the period for a 1 kHz frequency (adjust for your clock)
//    TMR2 = 0;              // Clear the timer register
//    T2CONbits.ON = 1;      // Turn on Timer 2
//}

void timer2Setup(){
    // Timer 2 Setup
//    PR2 = (int)(((float)(10000000.0 * 40000000000000000000) / 1) + 0.5);//3999999999999999999999999999999999;//3905;//156249;//0xFF;          // Set period register
    PR2 = 0xffffffff;
    TMR2 = 0;              // Clear timer register
    T2CONbits.TCKPS = 0b111; // 1:256 prescale
    T2CONbits.ON = 1;      // Turn on Timer 4
    IPC2bits.T2IP = 2;     // Set priority level
    IFS0bits.T2IF = 0;     // Clear interrupt flag
    IEC0bits.T2IE = 1;     // Enable Timer 4 interrupt
    INTCONbits.MVEC = 1;   // Multi-vector interrupts
    asm("ei");             // Enable interrupts globally
}

// Initialize Output Compare modules for PWM
void OC_Init(void) {
    // Configure OC3 for RED
    OC3CON = 0;                // Turn off OC3 for setup
    OC3R = 0;                  // Initial duty cycle = 0
    OC3RS = 0;                 // Secondary compare value
    OC3CONbits.OCM = 0b110;    // PWM mode, fault pin disabled
    OC3CONbits.ON = 1;         // Turn on OC3

    // Configure OC4 for GREEN
    OC4CON = 0;                // Turn off OC4 for setup
    OC4R = 0;                  // Initial duty cycle = 0
    OC4RS = 0;                 // Secondary compare value
    OC4CONbits.OCM = 0b110;    // PWM mode, fault pin disabled
    OC4CONbits.ON = 1;         // Turn on OC4

    // Configure OC5 for BLUE
    OC5CON = 0;                // Turn off OC5 for setup
    OC5R = 0;                  // Initial duty cycle = 0
    OC5RS = 0;                 // Secondary compare value
    OC5CONbits.OCM = 0b110;    // PWM mode, fault pin disabled
    OC5CONbits.ON = 1;         // Turn on OC5
}


// Initialize Button Pins
void Button_Init(void) {
    // Configure BTNU (RB1)
    TRISBbits.TRISB1 = 1; // Set RB1 as input
    ANSELBbits.ANSB1 = 0; // Disable analog functionality

    // Configure BTNL (RB0)
    TRISBbits.TRISB0 = 1; // Set RB0 as input
    ANSELBbits.ANSB0 = 0; // Disable analog functionality

    // Configure BTNC (RF0)
    TRISFbits.TRISF0 = 1; // Set RF0 as input

    // Configure BTNR (RB8)
    TRISBbits.TRISB8 = 1; // Set RB8 as input
    ANSELBbits.ANSB8 = 0; // Disable analog functionality

    // Configure BTND (RA15)
    TRISAbits.TRISA15 = 1; // Set RA15 as input
}

//void initializeEverything(){
//    // Initialize peripherals
//    PPS_Init();
//    Timer2_Init();
//    OC_Init();
//    Button_Init();
//} 
void configureEverything(){
    SSD_Init();//7 segment desplay
    configureKeys();
    configureLeds();
    configureSwitches();
    configureBeeper();
    configureScreen();
    PPS_Init();
//    Timer2_Init();
    OC_Init();
    Button_Init();
    timer2Setup();
    teachScreenArrows();
    teachScreenTarget();
     teachScreenExploion();
  //  initializeEverything();
//    configurePWM();
}



int countOnesInBinary(int number) {
    int count = 0;
    while (number > 0) {
        count += number & 1; // Add 1 to count if the least significant bit is 1
        number >>= 1;        // Right shift the number to check the next bit
    }
    return count;
}


int chooseWires() {
    int piDigits[] = {3, 1, 4, 1, 5, 9, 6, 5, 3, 5, 8, 9, 7, 9, 3, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 7, 9, 5};

    int piLength = sizeof(piDigits) / sizeof(piDigits[0]); // Get the length of the array
    int randomIndex = rand() % piLength;
    return piDigits[randomIndex];
    
}

int convertSwitchToNumber(){
    int number = 0;
    for (int i = 0; i < 8; i++){
        number += getSwitchState(i) << i;
    }
    return number;
}



void combineIntWithString(int number, char *text,char* outputTest) {
    char numberStr[5]; // Buffer to hold the converted number as a string
    sprintf(numberStr, "%d", number); // Convert the integer to a string

    // Combine the strings
    strcpy(outputTest, text);    // Copy the base string into the global result
    strcat(outputTest, numberStr); // Concatenate the number string
}



void answerReaction(char* message, int answerColor, int red, int green, int blue){
    int colors[]={0,3999};
    setText(message, 0x80);
   
    int delay=100000;
    if (!answerColor) 
        delay=10000;
    
    
    for (int i=0; i<8; i++){
        
        RGB_SetColor(colors[answerColor], colors[!answerColor], 0); // Set color to red
        PORTA=0;
        delayWithCount(delay);
        RGB_SetColor(red,green,blue);//event color
        PORTA=255;
        delayWithCount(delay);
    }
}

char chooseLetter(){
    char options[] = "ABCD";
    return options[rand() % 4];
}

void teachScreenArrows(){//16 instead of 24
    char CG_arrows[16] = {0x0, 0x4, 0x2, 0x1F, 0x2, 0x4, 0x0, 0x0,// 
                          0x0, 0x0, 0x4, 0x8, 0x1F, 0x8, 0x4, 0x0// 
                           }; 
    
    
    PORTBbits.RB15 = 1; // rs
    for (int i = 0; i < 16; i++) {// 16 instead of 24
        PORTE = CG_arrows[i];
        PORTDbits.RD4 = 1; // enable=1
        PORTDbits.RD4 = 0; // enable=0
        busy();
    }
    PORTBbits.RB15 = 0; // rs control
}
void teachScreenTarget(){//16 instead of 24
    char CG_target[16] = {0x2E, 0x31, 0x31, 0x35, 0x35, 0x31, 0x31, 0x2E,// 
                          0x2E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x2E 
                           }; //
    
    
    PORTBbits.RB15 = 1; // rs
    for (int i = 0; i < 16; i++) {// 16 instead of 24
        PORTE = CG_target[i];
        PORTDbits.RD4 = 1; // enable=1
        PORTDbits.RD4 = 0; // enable=0
        busy();
    }
    PORTBbits.RB15 = 0; // rs control
}
int desplayArrows(int pos, int whichArrow, int row){
    PORTBbits.RB15 = 0; // rs
    PORTE = row+pos;
    PORTDbits.RD4 = 1;  // enable=1
    PORTDbits.RD4 = 0;  // enable=0
    busy();

    PORTBbits.RB15 = 1; // rs

    PORTE = whichArrow%2;
    PORTDbits.RD4 = 1; // enable=1
    PORTDbits.RD4 = 0; // enable=0
    busy();
    
    
    return 0;
}
int desplayTarget(int row, int pos,int whichTarget){
    PORTBbits.RB15 = 0; // rs
    PORTE = row+pos;
    PORTDbits.RD4 = 1;  // enable=1
    PORTDbits.RD4 = 0;  // enable=0
    busy();

    PORTBbits.RB15 = 1; // rs

    PORTE = whichTarget;
    PORTDbits.RD4 = 1; // enable=1
    PORTDbits.RD4 = 0; // enable=0
    busy();
    
    
    return 0;
}
void setTarget(int* targetPos, int* targetRow){
    int place[2]={0x80, 0xc0};
    
    *targetRow=place[rand() %2];
    *targetPos=(rand() %15)+1;
    
}
void teachScreenExploion(){//16 instead of 24
    char CG_explosion[24] = {0x0, 0x0, 0x0, 0x0, 0x44, 0x0, 0x0, 0x0,// 
                          0x0, 0x0, 0x0, 0x44, 0x4A, 0x44, 0x0, 0x0 ,
                            0x0, 0x44, 0x4A, 0x51, 0x55, 0x51, 0x4A, 0x44
                           }; //
    
    
    PORTBbits.RB15 = 1; // rs
    for (int i = 0; i < 24; i++) {// 16 instead of 24
        PORTE = CG_explosion[i];
        PORTDbits.RD4 = 1; // enable=1
        PORTDbits.RD4 = 0; // enable=0
        busy();
    }
    PORTBbits.RB15 = 0; // rs control
}
int desplayExplosion(int pos, int whichExplosion, int row){
    PORTBbits.RB15 = 0; // rs
    PORTE = row+pos;
    PORTDbits.RD4 = 1;  // enable=1
    PORTDbits.RD4 = 0;  // enable=0
    busy();

    PORTBbits.RB15 = 1; // rs

    PORTE = whichExplosion;
    PORTDbits.RD4 = 1; // enable=1
    PORTDbits.RD4 = 0; // enable=0
    busy();
    
    
    return 0;
}

void desplayExplosions(){
    while(1){
        
        for (int i=4;i<7;i++){
            displayTime(true);
            PORTA=255;
            desplayExplosion(0,i,0x80);
            desplayExplosion(7,i,0xc0);
            desplayExplosion(12,i,0x80); 
            desplayExplosion(11,i,0xc0);
            desplayExplosion(16,i,0x80);
            desplayExplosion(15,i,0xc0);
            desplayExplosion(3,i,0xc0);
            
            delayWithCount(30000);
            playBeeper();
            PORTA=0;
            SSD_WriteDigits(10,10,10,10,0,0,0,0);
            
            desplayExplosion(5,i,0xc0);
            desplayExplosion(2,i,0x80);
            desplayExplosion(9,i,0xc0);
            desplayExplosion(14,i,0x80);
            desplayExplosion(13,i,0xc0);
            desplayExplosion(1,i,0xc0);
            desplayExplosion(16,i,0xc0);
            
            delayWithCount(30000);
        }
        
    }
}

void bombExplodes(int color){
    int colors[]={0,3999};
    
    configureScreenForArrowsGame();
    teachScreenArrows();
    teachScreenTarget();
    teachScreenExploion();
    resetScreen();
    
    RGB_SetColor(colors[color], colors[!color], 0); // red
    setText("GAME OVER",0X80+3);
    desplayExplosions();
    
    //add alarm
}

void shootArrow(int targetPos, int targetRow){
    int pos = 0;
    int whichArrow=0;
    int row=0x80;
    int whichTarget=2;
    while (1) {
        
        if (pos==targetPos && row==targetRow){
                whichTarget=3;
        } else whichTarget=2;
        if(pos==0x10) pos =0x0;
        if(pos==-0x1) pos =0x10;
        
        desplayArrows(pos,  whichArrow, row);
        desplayTarget(targetRow,targetPos,whichTarget);
        delayWithEvents();
        
        if (wasButtonPressed(0)) {
            RGB_SetColor(2000, 0, 2000);  // Magenta
            if (row==0xc0)
                row=0x80;
        
        } else if (wasButtonPressed(1)) {
            RGB_SetColor(2000, 2000, 2000);  // White
            pos--;
            whichArrow=1;
            
        }  else if (wasButtonPressed(2)) {
            RGB_SetColor(2500, 2500, 0); // Yellow
            pos++;
            whichArrow=0;
            
        } else if (wasButtonPressed(3)) {
            RGB_SetColor(0, 3999, 3999); // Cyan
            if (row==0x80)
                row=0xc0;
            
        } else if (wasButtonPressed(4)) {
            RGB_SetColor(500, 2500, 2500); // Blue
            if (pos==targetPos && row==targetRow){
                answerReaction("Success!",0 ,2000, 0, 2000);//message,green, pink, beam leds
                return true;
                
            } else answerReaction("Wrong place!",1 ,2000, 0, 2000);//message,red, pink, beam leds
        }else {
            RGB_SetColor(0, 0, 0); // Turn off RGB
        }
        
         resetScreen();
         resetAllButtonEvents();
    }
}



int buttonsGame(){
    configureScreenForArrowsGame();
    teachScreenArrows();
    teachScreenTarget();
    teachScreenExploion();
    
    int targetPos=15;
    int targetRow=0xc0;
    
    setTarget(&targetPos, &targetRow);
    shootArrow(targetPos,targetRow);
    
    return true;
}

int wireCurringStageThatWorkws(){
    int number =chooseWires();
    int numberOfAttempts =countOnesInBinary(number)+3;
    
    RGB_SetColor(0,0,3999);//BLUE COLOR
    
    char string[20]="\0";
    combineIntWithString(number, "Cut the wires: ",string);
    resetScreen();
    setText(string, 0x80);
    
    int attempts = 0;
    int currentSwitchState = convertSwitchToNumber();
    int prevSwitchState = currentSwitchState;
    while (attempts < numberOfAttempts){
        
        RGB_SetColor(0,0,3999);//BLUE COLOR
        configureSwitchesToLeds();
        currentSwitchState = convertSwitchToNumber();
        if(currentSwitchState != prevSwitchState){
            if(currentSwitchState == number){
                resetScreen();
                answerReaction("Success!",0 ,0,0,3999);//message,red, blue, beam leds
                return true;
            }else{
                attempts++;
            }
            prevSwitchState = currentSwitchState;
        }
        
        delayWithCount(10000);
        
    }
    resetScreen();
    answerReaction("Too many cuts!",1 ,0,0,3999);//message,red, blue, beam leds
    return false;
}


int letterGameThatWorks(){
    
    char pressLetter[20] = "Press letter: ";
    setText(pressLetter, 0x80);
    char temp[2]= {0, '\0'};;
    int sucssesfullAttempts = 0;
    char letter = chooseLetter();
    temp[0] = letter;
    setText(temp, 0xc0);
    while(sucssesfullAttempts < 4){
       
        RGB_SetColor(2500, 0, 2500); // purple
        delayWithEvents();
        if(wasKeyPressed(letter)){
            sucssesfullAttempts++;
            letter = chooseLetter();
            temp[0] = letter;
            setText(temp, 0xc0);
        }
        
        resetAllKeyEvents();
        
    }
    resetScreen();
    answerReaction("Success!",0 ,2500,0,2500);//message,red, blue, beam leds
}

int resistorGame(){
    int value= rand() &500;
    char string[20]="\0";
    combineIntWithString(value, "Match: ",string);
    resetScreen();
    setText(string, 0x80);
  
    setResistor();
    while(true){
        RGB_SetColor(2500, 2500, 2500); // purple
        int analogResistor= resistor();
        displayAnalogValue("Analog: ","", analogResistor, 0xc0 );
        if (analogResistor==value){
            delayWithCount(60000);
            resetScreen();
            answerReaction("Success!",0 ,0,2500,2500);//message,red, blue, beam leds
            return true;
        }
    }
}



void main(){
    srand(time(NULL)); // Seed the random number generator
    configureEverything();
//    originalKeyFunction();

    while(!wireCurringStageThatWorkws());
    
    letterGameThatWorks();
    
    buttonsGame();
    
    resistorGame();
    
    TIMER2DISABLED = true;
    bombExplodes(0);
    
  
}
    
