#pragma once
/* The sekigon EC driver predates QMK's GPIO/analog API renames.
 * These are pure macro aliases (lazy — only expand where the old names are
 * used, i.e. ec_switch_matrix.c / matrix.c), so force-including is safe. */

#ifndef setPinOutput
#  define setPinOutput(pin)     gpio_set_pin_output(pin)
#endif
#ifndef setPinInput
#  define setPinInput(pin)      gpio_set_pin_input(pin)
#endif
#ifndef setPinInputHigh
#  define setPinInputHigh(pin)  gpio_set_pin_input_high(pin)
#endif
#ifndef writePinHigh
#  define writePinHigh(pin)     gpio_write_pin_high(pin)
#endif
#ifndef writePinLow
#  define writePinLow(pin)      gpio_write_pin_low(pin)
#endif
#ifndef writePin
#  define writePin(pin, level)  gpio_write_pin(pin, level)
#endif
#ifndef readPin
#  define readPin(pin)          gpio_read_pin(pin)
#endif

/* RP2040 ADC reference is fixed; analogReference() is an AVR-ism — no-op it. */
#ifndef ADC_REF_INTERNAL
#  define ADC_REF_INTERNAL 0
#endif
#ifndef analogReference
#  define analogReference(x) ((void)0)
#endif

/* cli()/sei() are AVR interrupt enable/disable -> ARM CMSIS intrinsics. */
#ifndef cli
#  define cli() __disable_irq()
#endif
#ifndef sei
#  define sei() __enable_irq()
#endif
