#include <application.h>
#include <bc_cmwx1zzabz.h>
// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

bc_cmwx1zzabz_t lora;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
    }
}

void lora_callback(bc_cmwx1zzabz_t *self, bc_cmwx1zzabz_event_t event, void *event_param)
{
    volatile int a = 0;
    a++;

    if(event == BC_CMWX1ZZABZ_EVENT_ERROR)
    {
        bc_led_set_mode(&led, BC_LED_MODE_BLINK_FAST);
    }

    if(event == BC_CMWX1ZZABZ_EVENT_SEND_RF_FRAME_START)
    {
        bc_led_set_mode(&led, BC_LED_MODE_ON);
    }

    if(event == BC_CMWX1ZZABZ_EVENT_SEND_RF_FRAME_DONE)
    {
        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_pulse(&led, 100);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_cmwx1zzabz_init(&lora, BC_UART_UART1);
    
    bc_cmwx1zzabz_set_event_handler(&lora, lora_callback, NULL);
}

void application_task()
{
    uint8_t buffer[] = {'A', 'B', 'C'};

    if(!bc_cmwx1zzabz_is_ready(&lora))
    {
        bc_scheduler_plan_current_relative(200);
        return;
    }

    bc_cmwx1zzabz_send_rf_frame(&lora, buffer, sizeof(buffer) );

    // do not plan this task anymore
}


