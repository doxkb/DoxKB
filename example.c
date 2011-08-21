#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define bool            uint8_t
#define true            1
#define false           0

#define _PINB           (uint8_t *const)&PINB
#define _PORTC          (uint8_t *const)&PORTC
#define _PORTD          (uint8_t *const)&PORTD
#define _PORTF          (uint8_t *const)&PORTF

#define NULL            0
#define NA              0
#define KEY_MACRO       0
#define KEY_FN          0
#define KEY_MACRO1		0
#define KEY_MACRO2		0
#define KEY_MACRO3		0
#define KEY_MACRO4		0
#define KEY_MACRO5		0 
#define FN_KEY1_ID		13*6+1
#define FN_KEY2_ID		0*6+1
#define KEY_MACRO1_ID	0*6+5
#define KEY_MACRO2_ID	1*6+4
#define KEY_MACRO3_ID	2*6+4
#define KEY_MACRO4_ID	3*6+4
#define KEY_MACRO5_ID	13*6+2
#define MACRO_KEY_ID    0*6+0



const uint8_t is_modifier[84] = {
		//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
1,		1,			1,			1,			0,			0,			// COL  0
		1,			0,			0,			0,			0,		0,	// COL  1
		1,			0,			0,			0,			0,		0,	// COL  2
		0,			0,			0,			0,			0,		0,	// COL  3
		0,			0,			0,			0,			0,		0,	// COL  4
		0,			0,			0,			0,			0,		0,	// COL  5
		0,			0,			0,			0,			0,		0,	// COL  6
		0,			0,			0,			0,			0,		0,	// COL  7
		0,			0,			0,			0,			0,		0,	// COL  8
		1,			0,			0,			0,			0,		0,	// COL  9
		1,			0,			0,			0,			0,		0,	// COL 10
		0,			0,			0,			0,			0,		0,	// COL 11
		1,			1,			0,			0,			0,		0,	// COL 12 
		0,			1,			0,			0,			0,			// COL 13
														0			// COL 14
};

const uint8_t layer1[84] = {
		//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
KEY_MACRO,	KEY_FN,			KEY_LEFT_SHIFT,		KEY_LEFT_CTRL,		KEY_TAB,		KEY_ESC,		// COL  0
		KEY_LEFT_GUI,		KEY_Z,			KEY_A,			KEY_Q,			KEY_1,		NULL,	// COL  1
		KEY_LEFT_ALT,		KEY_X,			KEY_S,			KEY_W,			KEY_2,		NULL,	// COL  2
		NULL,			KEY_C,			KEY_D,			KEY_E,			KEY_3,		NULL,	// COL  3
		NULL,			KEY_V,			KEY_F,			KEY_R,			KEY_4,		NULL,	// COL  4
		KEY_SPACE,		KEY_B,			KEY_G,			KEY_T,			KEY_5,		NULL,	// COL  5
		NULL,			KEY_N,			KEY_H,			KEY_Y,			KEY_6,		NULL,	// COL  6 
		NULL,			KEY_M,			KEY_J,			KEY_U,			KEY_7,		NULL,	// COL  7
		NULL,			KEY_COMMA,		KEY_K,			KEY_I,			KEY_8,		NULL,	// COL  8
		KEY_RIGHT_ALT,		KEY_PERIOD,		KEY_L,			KEY_O,			KEY_9,		NULL,	// COL  9
		KEY_RIGHT_GUI,		KEY_SLASH,		KEY_SEMICOLON,		KEY_P,			KEY_0,		NULL,	// COL 10
		NULL,			NULL,			KEY_QUOTE,		KEY_LEFT_BRACE,		KEY_MINUS,	NULL,	// COL 11
		KEY_RIGHT_CTRL,		KEY_RIGHT_SHIFT,	NULL,			KEY_RIGHT_BRACE,	KEY_EQUAL,	NULL,	// COL 12
		KEY_DELETE,		KEY_FN,			KEY_ENTER,		KEY_BACKSPACE,		KEY_BACKSLASH,		// COL 13    
														KEY_TILDE		// COL 14
};

const uint8_t layer2[84] = {
		//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
 NULL,		KEY_FN,			KEY_LEFT_SHIFT,		KEY_LEFT_CTRL,		KEY_CAPS_LOCK,		KEY_ESC,		// COL  0
		KEY_LEFT_GUI,		KEY_Z,			KEY_LEFT,		KEY_Q,			KEY_F1,		NULL,	// COL  1
		KEY_LEFT_ALT,		KEY_X,			KEY_DOWN,		KEY_UP,			KEY_F2,		NULL,	// COL  2
		NULL,			KEY_C,			KEY_RIGHT,		KEY_E,			KEY_F3,		NULL,	// COL  3
		NULL,			KEY_V,			KEY_F,			KEY_R,			KEY_F4,		NULL,	// COL  4
		KEY_SPACE,		KEY_B,			KEY_G,			KEY_T,			KEY_F5,		NULL,	// COL  5
		NULL,			KEY_N,			KEY_H,			KEY_Y,			KEY_F6,		NULL,	// COL  6 
		NULL,			KEY_M,			KEY_J,			KEY_U,			KEY_F7,		NULL,	// COL  7
		NULL,			KEY_HOME,		KEY_PAGE_UP,		KEY_PRINTSCREEN,	KEY_F8,		NULL,	// COL  8
		KEY_RIGHT_ALT,		KEY_END,		KEY_PAGE_DOWN,		KEY_SCROLL_LOCK,	KEY_F9,		NULL,	// COL  9
		KEY_RIGHT_GUI,		KEY_DOWN,		KEY_LEFT,		KEY_PAUSE,		KEY_F10,	NULL,	// COL 10
		NULL,			NULL,			KEY_RIGHT,		KEY_UP,			KEY_F11,	NULL,	// COL 11
		KEY_RIGHT_CTRL,		KEY_RIGHT_SHIFT,	NULL,			KEY_RIGHT_BRACE,	KEY_F12,	NULL,	// COL 12
		KEY_DELETE,		KEY_FN,			KEY_ENTER,		KEY_BACKSPACE,		KEY_INSERT,		// COL 13 
														KEY_DELETE		// COL 14
};


const uint8_t layer3[84] = {
		//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
 NULL,		KEY_FN,			KEY_LEFT_SHIFT,		KEY_LEFT_CTRL,		KEY_TAB,		KEY_MACRO1,		// COL  0
		KEY_LEFT_GUI,		KEY_Z,			KEY_VOLDN,		KEY_APP,		KEY_MACRO2,	NULL,	// COL  1
		KEY_LEFT_ALT,		KEY_X,			KEY_VOLUP,		KEY_W,			KEY_MACRO3,	NULL,	// COL  2
		NULL,			KEY_C,			KEY_MUTE,		KEY_E,			KEY_MACRO4,	NULL,	// COL  3
		NULL,			KEY_V,			KEY_F,			KEY_R,			KEY_F4,		NULL,	// COL  4
		KEY_SPACE,		KEY_B,			KEY_G,			KEY_T,			KEY_F5,		NULL,	// COL  5
		NULL,			KEY_N,			KEY_H,			KEY_Y,			KEY_F6,		NULL,	// COL  6 
		NULL,			KEYPAD_0,		KEYPAD_1,		KEYPAD_4,		KEYPAD_7,	NULL,	// COL  7
		NULL,			KEY_COMMA,		KEYPAD_2,		KEYPAD_5,		KEYPAD_8,	NULL,	// COL  8
		KEY_RIGHT_ALT,		KEY_PERIOD,		KEYPAD_3,		KEYPAD_6,		KEYPAD_9,	NULL,	// COL  9
		KEY_RIGHT_GUI,		KEY_SLASH,		KEY_SEMICOLON,		KEY_P,			KEYPAD_SLASH,	NULL,	// COL 10
		NULL,			NULL,			KEY_QUOTE,		KEY_LEFT_BRACE,		KEYPAD_MINUS,	NULL,	// COL 11
		KEY_RIGHT_CTRL,		KEY_RIGHT_SHIFT,	NULL,			KEY_RIGHT_BRACE,	KEYPAD_PLUS,	NULL,	// COL 12
		KEY_DELETE,		KEY_FN,			KEY_MACRO5,		KEY_BACKSPACE,		KEYPAD_ASTERIX,		// COL 13    
														KEY_NUM_LOCK		// COL 14
};

const uint8_t *layout = layer1;

uint8_t *const row_port[6]  = { _PINB,  _PINB,  _PINB,  _PINB,  _PINB,  _PINB};
const uint8_t   row_bit[6]  = {  0x01,   0x02,   0x04,   0x08,   0x10,   0x20};
uint8_t *const col_port[14] = {_PORTD, _PORTD, _PORTD, _PORTD, _PORTD, _PORTC, _PORTD, _PORTD, _PORTF, _PORTF, _PORTF, _PORTF, _PORTF, _PORTF};
const uint8_t   col_bit[14] = {  0x01,   0x02,   0x04,   0x08,   0x10,   0x40,   0x80,   0x40,   0x01,   0x02,   0x10,   0x20,   0x80,   0x40};

bool pressed[84];
uint8_t queue[7] = {0,0,0,0,0,0,0};
uint8_t mod_keys = 0;

void init(void);
void send(void);
void poll(void);
void key_press(uint8_t key_id);
void key_release(uint8_t key_id);

int main(void) {
  init();
  for(;;) poll();
}

void send(void) {
  uint8_t i;

    if(pressed[FN_KEY1_ID])
    layout = layer2;
  else if(pressed[FN_KEY2_ID])
	  layout = layer3;
  else
    layout = layer1;

  if(layout == layer3 && pressed[KEY_MACRO1_ID])
  {Macro1(); return;}
  else if (layout == layer3 && pressed[KEY_MACRO2_ID])
  {Macro2(); return;}
    else if (layout == layer3 && pressed[KEY_MACRO3_ID])
  {Macro3(); return;}
      else if (layout == layer3 && pressed[KEY_MACRO4_ID])
  {Macro4(); return;}
else if(layout == layer3 && pressed[KEY_MACRO5_ID])
{Macro5(); return;}

  for(i=0; i<6; i++)
    keyboard_keys[i] = layout[queue[i]];
  keyboard_modifier_keys = mod_keys;
  usb_keyboard_send();
}

void poll() {
  uint8_t row, col, key_id;
  _delay_ms(5);
  for(col=0; col<14; col++) {
    *col_port[col] &= ~col_bit[col]; 
    _delay_us(1);
    for(row=0; row<6; row++) {
      key_id = col*6+row;
      if(!(*row_port[row] & row_bit[row])) {
	if(!pressed[key_id])
	  key_press(key_id);
      }
      else if(pressed[key_id])
	key_release(key_id);
    }
    *col_port[col] |= col_bit[col];
  }
}

void key_press(uint8_t key_id) {
  uint8_t i;
  pressed[key_id] = true;
  if(is_modifier[key_id])
    mod_keys |= layer1[key_id];
  else {
    for(i=5; i>0; i--) queue[i] = queue[i-1];
    queue[0] = key_id;
  }
  send();
}

void key_release(uint8_t key_id) {
  uint8_t i;
  pressed[key_id] = false;
  if(is_modifier[key_id])
    mod_keys &= ~layout[key_id];
  else {
    for(i=0; i<6; i++) if(queue[i]==key_id) break;
    for(; i<6; i++) queue[i] = queue[i+1];
  }
  send();
}

void init(void) {
  uint8_t i;
  CLKPR = 0x80; CLKPR = 0;
  usb_init();
  while(!usb_configured());
  _delay_ms(1000);
  // PORTB is set as input with pull-up resistors
  // PORTC,D,F are set to high xoutput
  DDRB  = 0x00; DDRC  = 0xFF; DDRD  = 0xFF; DDRF  = 0xFF;
  PORTB = 0xFF; PORTC = 0xFF; PORTD = 0xFF; PORTF = 0xFF; 

  for(i=0; i<84; i++) pressed[i] = false;
}






void SetInitialModifiers(uint8_t key_id)
{
	keyboard_modifier_keys = key_id;
keyboard_keys[0] = 0;
keyboard_keys[1] = 0;
keyboard_keys[2] = 0;
keyboard_keys[3] = 0;
keyboard_keys[4] = 0;
keyboard_keys[5] = 0;
usb_keyboard_send();
_delay_ms(2);	
}

void SendSingleKey(uint8_t key_id)
{
	keyboard_keys[0] = key_id;
usb_keyboard_send();
_delay_ms(2);

keyboard_keys[0] = 0;
usb_keyboard_send();
_delay_ms(2);
}

void SendSingleModifier(uint8_t key_id)
{
	keyboard_modifier_keys = key_id;
usb_keyboard_send();
_delay_ms(2);
}

//ctrl + alt + del
void Macro1()
{
SetInitialModifiers(KEY_CTRL);

keyboard_modifier_keys = KEY_CTRL | KEY_ALT;
usb_keyboard_send();
_delay_ms(2);

SendSingleKey(KEY_DELETE);

SendSingleModifier(0);
}

//insert ()
void Macro2()
{
SetInitialModifiers(KEY_LEFT_SHIFT);

SendSingleKey(KEY_9);
SendSingleKey(KEY_0);

SendSingleModifier(0);

SendSingleKey(KEY_LEFT);
}

//insert ""
void Macro3()
{
SetInitialModifiers(KEY_LEFT_SHIFT);

SendSingleKey(KEY_2);
SendSingleKey(KEY_2);

SendSingleModifier(0);

SendSingleKey(KEY_LEFT);
}

//insert {}
void Macro4()
{
SetInitialModifiers(0);

SendSingleKey(KEY_END);

SendSingleKey(KEY_ENTER);

SendSingleModifier(KEY_RIGHT_ALT);

SendSingleKey(KEY_QUOTE);

SendSingleModifier(0);

SendSingleKey(KEY_ENTER);

SendSingleModifier(KEY_RIGHT_ALT);

SendSingleKey(KEY_BACKSLASH);

SendSingleModifier(0);

SendSingleKey(KEY_UP);

SendSingleKey(KEY_ENTER);
}

//insert new line
void Macro5()
{
SetInitialModifiers(0);

SendSingleKey(KEY_END);

SendSingleKey(KEY_ENTER);
}
