// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer (WDT dis)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (Power-up Timer is on)
#pragma config CP = OFF         // Code Protection bit (Code protection disabled)

#define _XTAL_FREQ 4000000
#include <xc.h>

#define TRUE 1
#define FALSE 0

#define OFF 0
#define ON 1

#define NUMBER_OF_LIVES 8

#define LED_LIFE8 RB7
#define LED_LIFE7 RB6
#define LED_LIFE6 RB5
#define LED_LIFE5 RB4
#define LED_LIFE4 RB3
#define LED_LIFE3 RB2
#define LED_LIFE2 RB1
#define LED_LIFE1 RB0

#define LED_GAME_WON RA3

#define INPUT_TOUCHWIRE RA0
#define INPUT_FINISH_POINT RA1

#define BUZZER RA2

typedef  enum { TOUCHED_WIRE, TOUCHED_FINISH_POINT } PlayerAction;

static void initialiseLivesLeds( void ) {
    TRISB = 0x00; // All ports output to lives LEDs
}

static void initialiseTouchwireBuzzerWinLed() {
    TRISA = 0x03; // RA0 touch wire, RA1 finish, RA2 buzzer, RA3 Won LED
}

static void showLives( unsigned char lives ){
    LED_LIFE1 = lives > 0;
    LED_LIFE2 = lives > 1;
    LED_LIFE3 = lives > 2;
    LED_LIFE4 = lives > 3;
    LED_LIFE5 = lives > 4;
    LED_LIFE6 = lives > 5;
    LED_LIFE7 = lives > 6;
    LED_LIFE8 = lives > 7;
}

static PlayerAction waitUntilTouchOrFinish( void ){
           
    for(;;) {
        
        if ( INPUT_TOUCHWIRE == ON ) {
            return TOUCHED_WIRE;
        }
    
        if ( INPUT_FINISH_POINT == ON ) {
            return TOUCHED_FINISH_POINT ;
        }
        
        // Wait loop - crude filtering of input
        // adequate as no concurrency in this code
        __delay_ms( 200 );
    }
}    

static void showWin( void ) {
    showLives(0);
    LED_GAME_WON = ON ;
    
    for ( unsigned char i = 0; i<3; i++ ) {
        BUZZER = ON;
        __delay_ms( 100 );
        BUZZER = OFF ;
        __delay_ms( 200 );
    }
    
    __delay_ms( 2000 );
    LED_GAME_WON = OFF ;
}

static void showLostLife( unsigned char lives ){
    showLives( 0 ); // All life LEDs off
    BUZZER = ON;
    
    __delay_ms( 200 );
    
    BUZZER = OFF;
    showLives( lives );
}

static void rippleLifeLedsDown(){
    showLives(0);
    
    LED_LIFE8 = ON;
    __delay_ms( 150 );
    LED_LIFE8 = OFF;
    
    LED_LIFE7 = ON;
    __delay_ms( 120 );
    LED_LIFE7 = OFF;
    
    LED_LIFE6 = ON;
    __delay_ms( 90 );
    LED_LIFE6 = OFF;
    
    LED_LIFE5 = ON;
    __delay_ms( 80 );
    LED_LIFE5 = OFF;
    
    LED_LIFE4 = ON;
    __delay_ms( 50 );
    LED_LIFE4 = OFF;
    
    LED_LIFE3 = ON;
    __delay_ms( 45 );
    LED_LIFE3 = OFF;
    
    LED_LIFE2 = ON;
    __delay_ms( 40 );
    LED_LIFE2 = OFF;
    
    LED_LIFE1 = ON;
    __delay_ms( 40 );
    LED_LIFE1 = OFF;
}

static void showLostGame( void ){
    for (unsigned char i = 0; i < 3; i++ ){
        rippleLifeLedsDown();
    }
}

static void indicatePowerUp( void ) {
    for ( unsigned char i=0; i < 5; i++ ){
        
        showLives(8);
        __delay_ms(250);
        
        showLives(0);
        __delay_ms(250);
    }
    
    __delay_ms(1000);
}

static void playGame( void ) {
        
    unsigned char isGameOver = FALSE ;
    unsigned char lives = NUMBER_OF_LIVES ;

    showLives( lives );
    BUZZER = OFF;
    LED_GAME_WON = OFF;

    while( !isGameOver ) {
        PlayerAction action = waitUntilTouchOrFinish();

        if ( action == TOUCHED_FINISH_POINT ) {
            showWin();
            isGameOver = TRUE;
        }
        else {
            lives --;
            showLostLife( lives );

            if ( lives == 0 ){
                showLostGame();
                isGameOver = TRUE ;
            }
        }
    }
}
int main() {

    initialiseLivesLeds();
    initialiseTouchwireBuzzerWinLed();
    
    indicatePowerUp();

    while(1) {
        playGame();
        
        __delay_ms( 2000 );
    }
    
    return 0;
}

