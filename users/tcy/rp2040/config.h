#define PS2_PIO_USE_PIO1 // Force the usage of PIO1 peripheral, by default the PS2 implementation uses the PIO0 peripheral

#define I2C1_SCL_PIN GP3
#define I2C1_SDA_PIN GP2

#ifdef AUDIO_ENABLE
  #define AUDIO_PIN GP19
  #define AUDIO_PWM_DRIVER PWMD1
  #define AUDIO_PWM_CHANNEL RP2040_PWM_CHANNEL_B
  
  #define AUDIO_VOICES
  #define AUDIO_DAC_SAMPLE_MAX 4095U
  #define AUDIO_VOICE_DEFAULT butts_fader
  #define STARTUP_SOUND E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),
#endif

// defines for tuning the OLED
#define OLED_TIMEOUT 15000
#define OLED_UPDATE_INTERVAL 100
#define OLED_FADE_OUT
#define OLED_FADE_OUT_INTERVAL 2
#define OLED_UPDATE_PROCESS_LIMIT 4

#define PS2_MOUSE_X_MULTIPLIER 1
#define PS2_MOUSE_Y_MULTIPLIER 1
#define PS2_MOUSE_V_MULTIPLIER 2

// defines for making the Trackpoint more fluid
#define PS2_MOUSE_USE_REMOTE_MODE

#define PS2_MOUSE_SCROLL_BTN_SEND 500 /* Default is 300 */

#undef PS2_MOUSE_SCROLL_BTN_MASK
#define PS2_MOUSE_SCROLL_BTN_MASK (1<<PS2_MOUSE_BTN_MIDDLE) /* Default */
#define PS2_MOUSE_BTN_LEFT      0
#define PS2_MOUSE_BTN_RIGHT     1
#define PS2_MOUSE_BTN_MIDDLE    2

#define PS2_MOUSE_INIT_DELAY 1000 /* Default */

#ifdef PS2_USE_USART
/* synchronous, odd parity, 1-bit stop, 8-bit data, sample at falling edge */
/* set DDR of CLOCK as input to be slave */
#define PS2_USART_INIT() do {   \
	PS2_CLOCK_DDR &= ~(1<<PS2_CLOCK_BIT);   \
	PS2_DATA_DDR &= ~(1<<PS2_DATA_BIT);     \
	UCSR1C = ((1 << UMSEL10) |  \
			(3 << UPM10)   |  \
			(0 << USBS1)   |  \
			(3 << UCSZ10)  |  \
			(0 << UCPOL1));   \
	UCSR1A = 0;                 \
	UBRR1H = 0;                 \
	UBRR1L = 0;                 \
} while (0)
#define PS2_USART_RX_INT_ON() do {  \
	UCSR1B = ((1 << RXCIE1) |       \
			(1 << RXEN1));        \
} while (0)
#define PS2_USART_RX_POLL_ON() do { \
	UCSR1B = (1 << RXEN1);          \
} while (0)
#define PS2_USART_OFF() do {    \
	UCSR1C = 0;                 \
	UCSR1B &= ~((1 << RXEN1) |  \
			(1 << TXEN1));  \
} while (0)
#define PS2_USART_RX_READY      (UCSR1A & (1<<RXC1))
#define PS2_USART_RX_DATA       UDR1
#define PS2_USART_ERROR         (UCSR1A & ((1<<FE1) | (1<<DOR1) | (1<<UPE1)))
#define PS2_USART_RX_VECT       USART1_RX_vect
#endif

