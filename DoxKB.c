#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define bool            uint8_t
#define true            1
#define false           0

#define _PORTB         (uint8_t *const)&PORTB
#define _PINC          (uint8_t *const)&PINC
#define _PIND          (uint8_t *const)&PIND
#define _PINF          (uint8_t *const)&PINF

#define NULL            0
#define NA              0
#define KEY_MACRO       0
#define KEY_FN          0
#define FN_KEY1_ID		0*6+0
#define FN_KEY2_ID		6*6+0

#define KEY_MACRO1		0
#define KEY_MACRO1_ID	13*6+0
#define KEY_MACRO2		0
#define KEY_MACRO2_ID	1*6+0
#define KEY_MACRO3		0
#define KEY_MACRO3_ID	2*6+0
#define KEY_MACRO4		0
#define KEY_MACRO4_ID	3*6+0
#define KEY_MACRO5		0
#define KEY_MACRO5_ID	4*6+0
#define KEY_MACRO6		0
#define KEY_MACRO6_ID	0*6+3

const uint8_t is_modifier[84] = {
			//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
			1,				1,				1,				0,				0,				0,		// COL  0
			0,				0,				0,				0,				0,				0,		// COL  1
			0,				0,				0,				0,				0,				0,		// COL  2
			0,				0,				0,				0,				0,				0,		// COL  3
			1,				0,				0,				0,				0,				0,		// COL  4
			0,				0,				0,				0,				0,				0,		// COL  5
			1,				0,				1,				0,				0,				0,		// COL  6
			0,				0,				0,				0,				0,				0,		// COL  7
			0,				0,				0,				0,				0,				0,		// COL  8
			0,				0,				0,				0,				0,				0,		// COL  9
			0,				0,				0,				0,				0,				0,		// COL 10
			0,				0,				0,				0,				0,				0,		// COL 11
			1,				1,				0,				0,				0,				0,		// COL 12
			0,				0,				0,				0,				0,				0		// COL 13
};

const uint8_t layer1[84] = {
			//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
			KEY_FN,			KEY_LEFT_SHIFT,	KEY_LEFT_CTRL,	KEY_TAB,		KEY_EQUAL,		NULL, 	// COL  0
			KEY_LEFT,		KEY_Z,			KEY_A,			KEY_Q,			KEY_1,			NULL,	// COL  1
			KEY_UP,			KEY_X,			KEY_S,			KEY_W,			KEY_2,			NULL,	// COL  2
			KEY_DOWN,		KEY_C,			KEY_D,			KEY_E,			KEY_3, 			NULL,	// COL  3
			KEY_LEFT_GUI,	KEY_V,			KEY_F,			KEY_R,			KEY_4,			NULL,	// COL  4
			KEY_BACKSPACE,	KEY_B,			KEY_G,			KEY_T,			KEY_5,			NULL,	// COL  5
			KEY_FN,			KEY_ENTER,		KEY_RIGHT_ALT,	KEY_ESC,		NULL,			NULL,	// COL  6
			KEY_SPACE,		KEY_N,			KEY_H,			KEY_Y,			KEY_6,			NULL,	// COL  7
			KEY_LEFT,		KEY_M,			KEY_J,			KEY_U,			KEY_7,			NULL,	// COL  8
			KEY_DOWN,		KEY_COMMA,		KEY_K,			KEY_I,			KEY_8,			NULL,	// COL  9
			KEY_UP,			KEY_PERIOD,		KEY_L,			KEY_O,			KEY_9,			NULL,	// COL 10
			KEY_RIGHT,		KEY_SLASH,		KEY_SEMICOLON,	KEY_P,			KEY_0,			NULL,	// COL 11
			KEY_APP,		KEY_RIGHT_SHIFT,KEY_QUOTE,		KEY_LEFT_BRACE,	KEY_MINUS,		NULL,	// COL 12
			NULL,			NULL,			NULL,			NULL,			NULL,			NULL	// COL 13
};

const uint8_t layer2[84] = {
			//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
			KEY_FN,			KEY_LEFT_SHIFT,	KEY_LEFT_CTRL,	KEY_MACRO6,		KEY_F12,		NULL, 	// COL  0
			KEY_MACRO2,		KEY_Z,			KEY_VOLDN,		KEY_BACKSLASH,	KEY_F1,			NULL,	// COL  1
			KEY_MACRO3,		KEY_X,			KEY_VOLUP,		KEY_TILDE,		KEY_F2,			NULL,	// COL  2
			KEY_MACRO4,		KEY_C,			KEY_DELETE,		KEY_E,			KEY_F3, 		NULL,	// COL  3
			KEY_MACRO5,		KEY_V,			KEY_MUTE,		KEY_R,			KEY_F4,			NULL,	// COL  4
			KEY_BACKSPACE,	KEY_B,			KEY_G,			KEY_T,			KEY_F5,			NULL,	// COL  5
			KEY_FN,			KEY_ENTER,		KEY_RIGHT_ALT,	KEY_ESC,		NULL,			NULL,	// COL  6
			KEY_SPACE,		KEY_HOME,		KEY_LEFT,		KEY_Y,			KEY_F6,			NULL,	// COL  7
			KEY_LEFT,		KEY_PAGE_DOWN,	KEY_DOWN,		KEY_PRINTSCREEN,KEY_F7,			NULL,	// COL  8
			KEY_DOWN,		KEY_PAGE_UP,	KEY_UP,			KEY_SCROLL_LOCK,KEY_F8,			NULL,	// COL  9
			KEY_UP,			KEY_END,		KEY_RIGHT,		KEY_PAUSE,		KEY_F9,			NULL,	// COL 10
			KEY_RIGHT,		KEY_SLASH,		KEY_SEMICOLON,	KEY_P,			KEY_F10,		NULL,	// COL 11
			KEY_APP,		KEY_RIGHT_SHIFT,KEY_QUOTE,		KEY_RIGHT_BRACE,KEY_F11,		NULL,	// COL 12
			NULL,			NULL,			NULL,			NULL,			NULL,			NULL	// COL 13
};

const uint8_t layer3[84] = {
			//ROW 0			ROW 1			ROW 2			ROW 3			ROW 4
			KEY_FN,			KEY_LEFT_SHIFT,	KEY_LEFT_CTRL,	KEY_MACRO6,		KEY_F12,		NULL, 	// COL  0
			KEY_MACRO2,		KEY_Z,			KEY_VOLDN,		KEY_BACKSLASH,	KEY_F1,			NULL,	// COL  1
			KEY_MACRO3,		KEY_X,			KEY_VOLUP,		KEY_TILDE,		KEY_F2,			NULL,	// COL  2
			KEY_MACRO4,		KEY_C,			KEY_DELETE,		KEY_E,			KEY_F3, 		NULL,	// COL  3
			KEY_MACRO5,		KEY_V,			KEY_MUTE,		KEY_R,			KEY_F4,			NULL,	// COL  4
			KEY_BACKSPACE,	KEY_B,			KEY_G,			KEY_T,			KEY_F5,			NULL,	// COL  5
			KEY_FN,			KEY_ENTER,		KEY_RIGHT_ALT,	KEY_ESC,		NULL,			NULL,	// COL  6
			KEY_SPACE,		KEY_HOME,		KEY_LEFT,		KEY_Y,			KEY_F6,			NULL,	// COL  7
			KEY_LEFT,		KEY_PAGE_DOWN,	KEY_DOWN,		KEY_PRINTSCREEN,KEY_F7,			NULL,	// COL  8
			KEY_DOWN,		KEY_PAGE_UP,	KEY_UP,			KEY_SCROLL_LOCK,KEY_F8,			NULL,	// COL  9
			KEY_UP,			KEY_END,		KEY_RIGHT,		KEY_PAUSE,		KEY_F9,			NULL,	// COL 10
			KEY_RIGHT,		KEY_SLASH,		KEY_SEMICOLON,	KEY_P,			KEY_F10,		NULL,	// COL 11
			KEY_APP,		KEY_RIGHT_SHIFT,KEY_QUOTE,		KEY_RIGHT_BRACE,KEY_F11,		NULL,	// COL 12
			NULL,			NULL,			NULL,			NULL,			NULL,			NULL	// COL 13
};

const uint8_t *layout = layer1;

uint8_t *const row_port[6]  = { _PORTB,  _PORTB,  _PORTB,  _PORTB,  _PORTB,  _PORTB};
const uint8_t   row_bit[6]  = {  0x01,   0x02,   0x04,     0x08,    0x10,    0x20};

uint8_t *const col_port[14] = {_PIND, 	_PIND, 	_PIND, 	_PIND, 	_PINC, _PIND, 	_PIND, 	_PINF, 	_PINF, 	_PINF,	_PINF, 	_PINF, 	_PINF, _PIND};
const uint8_t   col_bit[14] = {  0x01,  0x02,   0x04,   0x08,   0x40,   0x40,   0x80,   0x01,   0x02,   0x10,   0x20,   0x40,   0x80,   0x10};

//0		1		2		3	 	4		5		6		7
//0x01	0x02	0x04	0x08	0x10	0x20	0x40	0x80

bool pressed[84];
uint8_t queue[6] = {0,0,0,0,0,0};
uint8_t mod_keys = 0;

bool semicolonPressed;
bool semicolonPassed;

void init(void);
void send(void);
void poll(void);
void key_press(uint8_t key_id);
void key_release(uint8_t key_id);

void SetInitialModifiers(uint8_t key_id);
void SendSingleKey(uint8_t key_id);
void SendSingleModifier(uint8_t key_id);

void Macro1(void);
void Macro2(void);
void Macro3(void);
void Macro4(void);
void Macro5(void);
void Macro6(void);

int main(void) {
  init();
  semicolonPressed = false;
  semicolonPassed = false;
  //SendSingleKey(KEY_E);
  for(;;) poll();
}

void send(void)
{
	uint8_t i;

 	if (pressed[FN_KEY1_ID])
		layout = layer2;
	else if (pressed[FN_KEY2_ID])
	 	layout = layer2;
	else if (semicolonPressed)
		layout = layer3;
	else
		layout = layer1;

	// if (layout == layer3 && pressed[KEY_MACRO1_ID])
	// {
	// 	Macro1();
	// 	return;
	// }
	if (layout == layer2 && pressed[KEY_MACRO2_ID])
	{
		Macro2();
		return;
	}
	else if (layout == layer2 && pressed[KEY_MACRO3_ID])
	{
		Macro3();
		return;
	}
	else if (layout == layer2 && pressed[KEY_MACRO4_ID])
	{
		Macro4();
		return;
	}
	else if (layout == layer2 && pressed[KEY_MACRO5_ID])
	{
		Macro5();
		return;
	}
	else if (layout == layer2 && pressed[KEY_MACRO6_ID])
	{
		Macro6();
	 	return;
	}

	for(i = 0; i < 6; i++)
		keyboard_keys[i] = layout[queue[i]];

	keyboard_modifier_keys = mod_keys;
	usb_keyboard_send();
}

void poll(void)
{
	uint8_t row, col, key_id;
	_delay_ms(5);

	for(row = 0; row < 6; row++){
		*row_port[row] &= ~row_bit[row];
		_delay_us(1);
		for(col = 0; col < 14; col++)
		{
			key_id = col*6+row;
			
			if (!(*col_port[col] & col_bit[col]))
			{
				if (!pressed[key_id])
					key_press(key_id);
			}
			else if (pressed[key_id])
				key_release(key_id);
		}
		*row_port[row] |= row_bit[row];
	}
}

void key_press(uint8_t key_id)
{
	uint8_t i;

	pressed[key_id] = true;

	if (layout[key_id] == KEY_SEMICOLON)
	{
		semicolonPressed = true;
		semicolonPassed = false;
		return;
	}

	if (semicolonPressed)
		semicolonPassed = true;

	if (is_modifier[key_id])
		mod_keys |= layer1[key_id];
	else
	{
		for(i = 5; i>0; i--)
			queue[i] = queue[i-1];

		queue[0] = key_id;
	}
	send();
}

void key_release(uint8_t key_id)
{
	uint8_t i;

	pressed[key_id] = false;

	if (layout[key_id] == KEY_SEMICOLON)
	{
		semicolonPressed = false;

		if (semicolonPassed == false)
			SendSingleKey(KEY_SEMICOLON);
		return;
	}

	if (is_modifier[key_id])
		mod_keys &= ~layout[key_id];
	else
	{
		for(i = 0; i < 6; i++)
		{
			if (queue[i] == key_id)
				break;
		}

		for(; i < 6; i++)
			queue[i] = queue[i+1];
	}
	send();
}

void init(void)
{
	uint8_t i;
	CLKPR = 0x80;
	CLKPR = 0;

	usb_init();

	while(!usb_configured());
	_delay_ms(1000);

	// PORTB is set as input with pull-up resistors
	// PORTC,D,F are set to high xoutput
	DDRB = 0xFF;
	PORTB = 0xFF;

	DDRC = 0x00;
	PORTC = 0xFF;

	DDRD = 0x00;
	PORTD = 0xFF;

	DDRF = 0x00;
	PORTF = 0xFF;

	for(i = 0; i < 84; i++)
		pressed[i] = false;
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
void Macro1(void)
{
	SetInitialModifiers(KEY_CTRL);

	keyboard_modifier_keys = KEY_CTRL | KEY_SHIFT;
	usb_keyboard_send();
	_delay_ms(2);

	SendSingleKey(KEY_ESC);

	SendSingleModifier(0);
}

//insert ()
void Macro2(void)
{
	SetInitialModifiers(KEY_LEFT_SHIFT);

	SendSingleKey(KEY_9);
	SendSingleKey(KEY_0);

	SendSingleModifier(0);

	SendSingleKey(KEY_LEFT);
}

//insert ""
void Macro3(void)
{
	SetInitialModifiers(KEY_LEFT_SHIFT);

	SendSingleKey(KEY_2);
	SendSingleKey(KEY_2);

	SendSingleModifier(0);

	SendSingleKey(KEY_LEFT);
}

//insert {}
void Macro4(void)
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
void Macro5(void)
{
	SetInitialModifiers(0);

	SendSingleKey(KEY_END);

	SendSingleKey(KEY_ENTER);
}

//Jump to bootloader
void Macro6(void)
{
	cli();
	// disable watchdog, if enabled
	// disable all peripherals
	UDCON = 1;
	USBCON = (1<<FRZCLK);  // disable USB
	UCSR1B = 0;
	_delay_ms(5);
#if defined(__AVR_AT90USB162__)                // Teensy 1.0
    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0;
    TIMSK0 = 0; TIMSK1 = 0; UCSR1B = 0;
    DDRB = 0; DDRC = 0; DDRD = 0;
    PORTB = 0; PORTC = 0; PORTD = 0;
    asm volatile("jmp 0x3E00");
#elif defined(__AVR_ATmega32U4__)              // Teensy 2.0
    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
    DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
    PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
    asm volatile("jmp 0x7E00");
#elif defined(__AVR_AT90USB646__)              // Teensy++ 1.0
    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0; UCSR1B = 0; TWCR = 0;
    DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
    PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
    asm volatile("jmp 0xFC00");
#elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0; UCSR1B = 0; TWCR = 0;
    DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
    PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
    asm volatile("jmp 0x1FC00");
#endif
}
