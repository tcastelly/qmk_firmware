#define I2C1_SCL_PIN GP3
#define I2C1_SDA_PIN GP2


#define PS2_PIO_USE_PIO1 // Force the usage of PIO1 peripheral, by default the PS2 implementation uses the PIO0 peripheral

#define AZOTEQ_IQS5XX_TPS65
#define AZOTEQ_IQS5XX_ROTATION_270

#define AUDIO_PIN GP19
#define AUDIO_PWM_DRIVER PWMD1
#define AUDIO_PWM_CHANNEL RP2040_PWM_CHANNEL_B

#define AUDIO_VOICES
#define AUDIO_DAC_SAMPLE_MAX 4095U
#define AUDIO_VOICE_DEFAULT butts_fader

#ifdef AUDIO_ENABLE
  #define STARTUP_SOUND E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),
#endif

