
/*============================================================================
 * DUAL RING LIBRARY
 *
 * This library provides ....
 *
 *
 * For more info, see the associated wiki in github.
 *===========================================================================*/   
#include <Arduino.h>
#include "DualRingLED.H"

// Hardware definitions for our LED strip.
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

// Our LED Matrix:
// 16 LEDs in the inner loop, going couter-clockwise.
// 24 LEDs in the outer loop, going clockwise.
// These are helpful defines for where the loops start and end.
#define NUM_LEDS    (DUAL_RING_NUM_INNER + DUAL_RING_NUM_OUTER)

#define DEFAULT_BRIGHTNESS 30

const TProgmemPalette16 defaultPalette PROGMEM = 
{
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,

  CRGB::Blue,
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Blue,

  CRGB::Blue,
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Blue,

  CRGB::Blue,
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Blue
};

void DualRingLED::begin( void )
{
    //  HMMM...Fast LED doesn't like passing pin...must be a template thing.  
    //  I'm gonna cheat by case-switching this with constants.
    //  Assuming we're not using 0, 1, 2, or 3 (serial pins or xbee pins).  Also assuming not using analog pins.
    
   // FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

    //#if 0 
    switch (_pin)
    {
      case 4: FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 5: FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 6: FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 7: FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 8: FastLED.addLeds<LED_TYPE, 8, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 9: FastLED.addLeds<LED_TYPE, 9, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 10: FastLED.addLeds<LED_TYPE, 10, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 11: FastLED.addLeds<LED_TYPE, 11, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 12: FastLED.addLeds<LED_TYPE, 12, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      case 13: FastLED.addLeds<LED_TYPE, 13, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); break;
      default: return;
    }
    //#endif
    
    FastLED.setBrightness(  DEFAULT_BRIGHTNESS );
    
    // clear the array, just in case.

    fillAll(CRGB::Black);
    FastLED.show();
 
}

DualRingLED::DualRingLED(int pin)
{
  _pin = pin;
  _palette = defaultPalette;
  innerLEDs = _leds;
  outerLEDs = &(_leds[DUAL_RING_NUM_INNER]);
}

void DualRingLED::run( void )
{
  if (_runFunc)
  {
    _runFunc();
  }
  
  FastLED.show();
}

void DualRingLED::run( int delay_ms )
{
  if (_runFunc)
  {
    _runFunc();
  }

  FastLED.show();
  FastLED.delay(delay_ms);
}

void DualRingLED::setRunFunc(dualRingFuncType func)
{
  _runFunc = func;
}

/*===============================================================================
 * Function:  _rotateDownHelper
 *
 * Since the LEDs can be wired arbitrarily (is increasing the index going clockwise
 * or counter-clockwise), I'm gonna make these helpers just go "up" for increasing array
 * and "down" for decreasing array.  We can then map to clockwise and counter-clockwise
 * with the real rotate functions.
 */
void DualRingLED::_rotateDownHelper( CRGB *start_led, int num )
{
    CRGB roll_over_value;
    int i;
    
    // this is basically just gonna be a big shift with roll-over.
    
    // remember the "0th" value...it's gonna go into the "last" array value.
    roll_over_value = start_led[0];
    
    // now copy everything one slot "down"
    for (i=0; i< num -1; i++)
    {
        start_led[i] = start_led[i+1];
    }
    
    // Finally, store the last LED with that roll-over value.
    start_led[num - 1] = roll_over_value;
    
}  // end of _rotateDownHelper

/*===============================================================================
 * Function:  _rotateUpHelper
 *
 * Since the LEDs can be wired arbitrarily (is increasing the index going clockwise
 * or counter-clockwise), I'm gonna make these helpers just go "up" for increasing array
 * and "down" for decreasing array.  We can then map to clockwise and counter-clockwise
 * with the real rotate functions.
 */
void DualRingLED::_rotateUpHelper( CRGB *start_led, int num )
{
    CRGB roll_over_value;
    int i;
    
    // this is basically just gonna be a big shift with roll-over.
    
    // remember the "last" value...it's gonna go into the "first" array value.
    roll_over_value = start_led[num - 1];
    
    // now copy everything one slot "up"
    for (i = num - 1; i > 0; i--)
    {
        start_led[i] = start_led[i-1];
    }
    
    // Finally, store the first LED with that roll-over value.
    start_led[0] = roll_over_value;
    
}  // end of rotate_down_helper

/*===============================================================================
 * Function:  _drawStreakHelper
 *
 * In some instances, we want to draw a streak spanning the roll-over point.
 * This helper function does that.
 * Note this is heavier weight than the simple "make clockwise/counter-clockwise streak"
 * functions...and it doesn't fill in the background...it *JUST* updates the streak pixels.
 */
void DualRingLED::_drawStreakHelper( CRGB *ring_start, int ring_size, int streak_start_index, int streak_size, CRGB start_color, CRGB end_color)
{
   CRGB temp_led[DUAL_RING_NUM_OUTER];
   int  copy_index;
   int  num_copied=0;

   // a given streak may span our roll-over point.  
   // Exammple:  an inner streak of length 4, starting at led index 14 should light up
   // leds 14, 15, 0, and 1...but if we just use fill gradient from 14, it lights up
   // 14, 15, 16, and 17...two from the inner loop and two from the outer.
   //
   // One way to deal with this is to break the streak into two separate gradients..but then you need
   // to do color interpolation.   Instead, I'm going to have a temporary led array that *doesn't* 
   // roll over to do the fill_gradient (which will do the full interpolation for me), and then
   // copy the leds over to the right spots in our ring, dealing with the roll-over.

   // Start with some error checks...
   if (streak_size > DUAL_RING_NUM_OUTER) streak_size = DUAL_RING_NUM_OUTER;

   // make our gradient in the temp array.
   fill_gradient_RGB(temp_led, streak_size, start_color, end_color);

   // start by copying led up to the roll-over point
   copy_index = streak_start_index;
   while ((copy_index < ring_size) && (num_copied < streak_size))
   {
      ring_start[copy_index] = temp_led[num_copied];
      num_copied++;
      copy_index++;
   }

   // from here, we just rolled over...so start from the "zeroth" led.
   copy_index = 0;   

   // ...and copy the rest of the streak
   while (num_copied < streak_size)
   {
      ring_start[copy_index] = temp_led[num_copied];
      num_copied++;
      copy_index++;
   }
  
}

/*===============================================================================
 * Function:  _makeBumpHelper
 * 
 * bump size is the number of leds on either side of the bump led...so bump size of 2
 *   gives a TOTAL LED size of 5...one in the center, and 2 on either side.
 * 
 */
void DualRingLED::_makeBumpHelper(int center_led, int bump_size, CRGB background, CRGB bump)
{
  int start_led_index;
  
  // first cut here will treat rollovers as errors...do nothing.  
  // next cut can do modulo math to make the right thing happen.
  // note I'm also not dealing with the inner/outer loop rollover.
  if (bump_size < 1) return;
  if (center_led - bump_size < 0) return;
  if (center_led + bump_size > NUM_LEDS) return;

  start_led_index = center_led - bump_size;
  fill_gradient_RGB(&(_leds[start_led_index]), bump_size + 1, background, bump);
  fill_gradient_RGB(&(_leds[center_led]), bump_size+1, bump, background);
  
}


/*===============================================================================
 * Function:  fillAll
 */
void DualRingLED::fillAll(CRGB color)
{
    fill_solid(_leds, NUM_LEDS, color);
}

/*===============================================================================
 * Function:  fillInner
 */
void DualRingLED::fillInner(CRGB color)
{
    fill_solid(innerLEDs, DUAL_RING_NUM_INNER, color);
}

/*===============================================================================
 * Function:  fillOuter
 */
void DualRingLED::fillOuter(CRGB color)
{
    fill_solid(outerLEDs, DUAL_RING_NUM_OUTER, color);
}

/*===============================================================================
 * Function:  rotateInnerClockwise
 */
void DualRingLED::rotateInnerClockwise( void )
{
    _rotateDownHelper(innerLEDs, DUAL_RING_NUM_INNER);
}  


/*===============================================================================
 * Function:  rotateInnerCounterClockwise
 */
void DualRingLED::rotateInnerCounterClockwise( void )
{
    _rotateUpHelper(innerLEDs, DUAL_RING_NUM_INNER);
}  

/*===============================================================================
 * Function:  rotateOuterClockwise
 */
void DualRingLED::rotateOuterClockwise( void )
{
    _rotateUpHelper(outerLEDs, DUAL_RING_NUM_OUTER);  
} 
    
/*===============================================================================
 * Function:  rotateOuterCounterClockwise
 */
void DualRingLED::rotateOuterCounterClockwise( void )
{
    _rotateDownHelper(outerLEDs, DUAL_RING_NUM_OUTER);
}



/*===============================================================================
 * Function:  makeInnerBump
 */
void DualRingLED::makeInnerBump(int bump_size, CRGB background, CRGB bump)
{

  if (bump_size < 1) bump_size = 1;
  if (bump_size > 7) bump_size = 7;
  
  fillInner(background);

  // want the bump centered in the inner array, hence the 8
  _makeBumpHelper(8, bump_size, background, bump);
}

/*===============================================================================
 * Function:  makeOuterBump
 */
void DualRingLED::makeOuterBump(int bump_size, CRGB background, CRGB bump)
{
  if (bump_size < 1) bump_size = 1;
  if (bump_size > 11) bump_size = 11;
  
  fillOuter(background);

  // want the bump centered in the outer array, hence the 28
  _makeBumpHelper(28, bump_size, background, bump);
}

/*===============================================================================
 * Function:  makeInnerClockwiseStreak
 */
void DualRingLED::makeInnerClockwiseStreak(int streak_size, CRGB background, CRGB head)
{
  if (streak_size > DUAL_RING_NUM_INNER) streak_size = DUAL_RING_NUM_INNER;

  fillInner(background);

  // inner indexes go counter-clockwise.  
  // we're gonna put the head at index 0, and then fade as array indexes increase
  fill_gradient_RGB(innerLEDs, streak_size, head, background); 
}

/*===============================================================================
 * Function:  makeInnerCounterClockwiseStreak
 */
void DualRingLED::makeInnerCounterClockwiseStreak(int streak_size, CRGB background, CRGB head)
{
  if (streak_size > DUAL_RING_NUM_INNER) streak_size = DUAL_RING_NUM_INNER;

  fillInner(background);

  // since inner indexes go counter-clockwise, we need to start at the tail, and build to the head
  fill_gradient_RGB(innerLEDs, streak_size, background, head); 
}

/*===============================================================================
 * Function:  makeOuterClockwiseStreak
 */
void DualRingLED::makeOuterClockwiseStreak(int streak_size, CRGB background, CRGB head)
{
  if (streak_size > DUAL_RING_NUM_OUTER) streak_size = DUAL_RING_NUM_OUTER;

  fillOuter(background);
  
  // since outer indexes go counter-clockwise, we need to start at the tail, and build to the head
  fill_gradient_RGB(outerLEDs, streak_size, background, head); 

}

/*===============================================================================
 * Function:  makeOuterCounterClockwiseStreak
 */
void DualRingLED::makeOuterCounterClockwiseStreak(int streak_size, CRGB background, CRGB head)
{
  if (streak_size > DUAL_RING_NUM_OUTER) streak_size = DUAL_RING_NUM_OUTER;

  fillOuter(background);
  
  // since outer indexes go counter-clockwise, we need to start at the head, and build to the tail
  fill_gradient_RGB(outerLEDs, streak_size, head, background); 
}

/*===================================================================================
 * Function: drawInnerClockwiseStreak
 * The draw_ functions are different from the make_ functions;
 *   - They don't fill in the background
 *   - You specify the starting location
 *   - They are more processor and memory intensive.
 *  What does this mean?  Use the make_ functions if you can.  If not, use draw_.
 *  
 */
void DualRingLED::drawInnerClockwiseStreak(int start_index, int streak_size, CRGB head, CRGB tail)
{
   if (start_index < 0) start_index = 0;
   if (start_index > DUAL_RING_LAST_INNER) start_index = DUAL_RING_LAST_INNER;
   if (streak_size > DUAL_RING_NUM_INNER) streak_size = DUAL_RING_NUM_INNER;

   _drawStreakHelper(innerLEDs, DUAL_RING_NUM_INNER, start_index, streak_size, head, tail);
}

/*===================================================================================
 * Function: drawInnerCounterClockwiseStreak
 * The draw_ functions are different from the make_ functions;
 *   - They don't fill in the background
 *   - You specify the starting location
 *   - They are more processor and memory intensive.
 *  What does this mean?  Use the make_ functions if you can.  If not, use draw_.
 *  
 */
void DualRingLED::drawInnerCounterClockwiseStreak(int start_index, int streak_size, CRGB head, CRGB tail)
{
   if (start_index < 0) start_index = 0;
   if (start_index > DUAL_RING_LAST_INNER) start_index = DUAL_RING_LAST_INNER;
   if (streak_size > DUAL_RING_NUM_INNER) streak_size = DUAL_RING_NUM_INNER;

   // since we're filling "backwards", we need to adjust our starting position
   start_index = start_index - streak_size + 1;
   if (start_index < 0) start_index = start_index + DUAL_RING_NUM_INNER;
    
   _drawStreakHelper(innerLEDs, DUAL_RING_NUM_INNER, start_index, streak_size, tail, head);
}

/*===================================================================================
 * Function: drawOuterCounterClockwiseStreak
 * The draw_ functions are different from the make_ functions;
 *   - They don't fill in the background
 *   - You specify the starting location
 *   - They are more processor and memory intensive.
 *  What does this mean?  Use the make_ functions if you can.  If not, use draw_.
 *  
 *  Note that "start index" is the index into the outer_leds array, not the absolute index.
 */
void DualRingLED::drawOuterCounterClockwiseStreak(int start_index, int streak_size, CRGB head, CRGB tail)
{
   if (start_index < 0) start_index = 0;
   if (start_index > DUAL_RING_LAST_OUTER) start_index = DUAL_RING_LAST_OUTER;
   if (streak_size > DUAL_RING_NUM_OUTER) streak_size = DUAL_RING_NUM_OUTER;

   _drawStreakHelper(outerLEDs, DUAL_RING_NUM_OUTER, start_index, streak_size, head, tail);
}

/*===================================================================================
 * Function: drawOuterClockwiseStreak
 * The draw_ functions are different from the make_ functions;
 *   - They don't fill in the background
 *   - You specify the starting location
 *   - They are more processor and memory intensive.
 *  What does this mean?  Use the make_ functions if you can.  If not, use draw_.
 *  
 *  Note that "start index" is the index into the outer_leds array, not the absolute index.
 *  
 */
void DualRingLED::drawOuterClockwiseStreak(int start_index, int streak_size, CRGB head, CRGB tail)
{
   if (start_index < 0) start_index = 0;
   if (start_index > DUAL_RING_LAST_OUTER) start_index = DUAL_RING_LAST_OUTER;
   if (streak_size > DUAL_RING_NUM_OUTER) streak_size = DUAL_RING_NUM_OUTER;

   // since we're filling "backwards", we need to adjust our starting position
   start_index = start_index - streak_size + 1;
   if (start_index < 0) start_index = start_index + DUAL_RING_NUM_OUTER;
   
   _drawStreakHelper(outerLEDs, DUAL_RING_NUM_OUTER, start_index, streak_size, tail, head);
}

void DualRingLED::setPalette(CRGBPalette16 palette)
{
   _palette = palette;
}

void DualRingLED::pulseAll( void )
{
  static uint8_t index;
  CRGB           color;

  color = ColorFromPalette(_palette, index);
  fillAll(color);

  index++;
}

void DualRingLED::pulseInner( void )
{
  static uint8_t index;
  CRGB           color;

  color = ColorFromPalette(_palette, index);
  fillInner(color);

  index++;
}

void DualRingLED::pulseOuter( void )
{
  static uint8_t index;
  CRGB           color;

  color = ColorFromPalette(_palette, index);
  fillOuter(color);

  index++;
}

void DualRingLED::waterfall( void )
{

}

