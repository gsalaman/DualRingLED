#ifndef DUAL_RING_LED_H
#define DUAL_RING_LED_H
/*============================================================================
 * DUAL RING LED LIBRARY
 *
 * This library provides ....
 *
 * USAGE: 
 * *===========================================================================*/   
#include <Arduino.h>
#include <FastLED.h>

// Our LED Matrix:
// 16 LEDs in the inner loop, going couter-clockwise.
// 24 LEDs in the outer loop, going clockwise.
// These are helpful defines for where the loops start and end.
#define DUAL_RING_NUM_INNER  16
#define DUAL_RING_NUM_OUTER  24
#define DUAL_RING_LAST_INNER (DUAL_RING_NUM_INNER - 1)
#define DUAL_RING_LAST_OUTER (DUAL_RING_NUM_OUTER - 1)

typedef void (*dualRingFuncType)();

class DualRingLED
{
  public:
    DualRingLED(int pin);
    void begin(void);
    void run(void);
    void run(int delay);
    CRGB *innerLEDs;
    CRGB *outerLEDs;
    void setRunFunc(dualRingFuncType runFunc);

    void fillAll(CRGB color);
    void fillInner(CRGB color);
    void fillOuter(CRGB color);
    void rotateInnerClockwise(void);
    void rotateInnerCounterClockwise(void);
    void rotateOuterClockwise(void);
    void rotateOuterCounterClockwise(void);
    void makeInnerBump(int bumpSize, CRGB background, CRGB bump);
    void makeOuterBump(int bumpSize, CRGB background, CRGB bump);
    void makeInnerClockwiseStreak( int streakSize, CRGB background, CRGB Head);
    void makeInnerCounterClockwiseStreak(int streakSize, CRGB background, CRGB Head);
    void makeOuterClockwiseStreak( int streakSize, CRGB background, CRGB Head);
    void makeOuterCounterClockwiseStreak(int streakSize, CRGB background, CRGB Head);
    void drawInnerClockwiseStreak(int startIndex, int streakSize, CRGB head, CRGB tail);
    void drawInnerCounterClockwiseStreak(int startIndex, int streakSize, CRGB head, CRGB tail);
    void drawOuterClockwiseStreak(int startIndex, int streakSize, CRGB head, CRGB tail);
    void drawOuterCounterClockwiseStreak(int startIndex, int streakSize, CRGB head, CRGB tail);
    void setPalette(CRGBPalette16 palette);
    void pulseAll( void );
    void pulseInner( void );
    void pulseOuter( void );
    void waterfall( void );

    
    

  private:
    CRGB             _leds[40];
    dualRingFuncType _runFunc=NULL;
    int              _pin;

    CRGBPalette16    _palette; 

    /* I don't think these actually need to be in the class...*/
    void _rotateDownHelper( CRGB *startLed, int num );
    void _rotateUpHelper( CRGB *startLed, int num );
    void _drawStreakHelper( CRGB *ringStart, int ringSize, int streakStartIndex, int streakSize, CRGB startColor, CRGB endColor);
    void _makeBumpHelper(int centerLed, int bumpSize, CRGB background, CRGB bump);
  
};

#endif // DUAL_RING_LED_H
