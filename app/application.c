#include <application.h>

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

// IMPORTANT, FIRST SET LORA MODULE TO 9600 BAUD 
// AT+UART=9600\r
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
    if (event == BC_CMWX1ZZABZ_EVENT_READY)
    {
        volatile char deveui[20];
        volatile char devaddr[20];
        bc_cmwx1zzabz_get_deveui(&lora, (char*)deveui);
        bc_cmwx1zzabz_get_devaddr(&lora, (char*)devaddr);
        deveui[0] = 0;
    }

    if (event == BC_CMWX1ZZABZ_EVENT_ERROR)
    {
        bc_led_set_mode(&led, BC_LED_MODE_BLINK_FAST);
    }

    if (event == BC_CMWX1ZZABZ_EVENT_SEND_RF_FRAME_START)
    {
        bc_led_set_mode(&led, BC_LED_MODE_ON);
    }

    if (event == BC_CMWX1ZZABZ_EVENT_SEND_RF_FRAME_DONE)
    {
        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }

    if (event == BC_CMWX1ZZABZ_EVENT_MESSAGE_RECEIVED)
    {
        volatile uint8_t port = bc_cmwx1zzabz_get_received_message_port(self);
        volatile uint32_t length = bc_cmwx1zzabz_get_received_message_length(self);

        uint8_t msg_buffer[51];
        volatile uint32_t len = bc_cmwx1zzabz_get_received_message_data(self, msg_buffer, sizeof(msg_buffer));

        bc_led_pulse(&led, 800);

        port++;
        length++;
        len++;
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
    //bc_cmwx1zzabz_set_appkey(&lora, "4257FAD1D73C3B7D4C2EDBAE6EBDA740");
    //bc_cmwx1zzabz_set_appskey(&lora, "4357FAD1D73C3B7D4C2EDBAE6EBDA740");
    //bc_cmwx1zzabz_set_nwkskey(&lora, "4457FAD1D73C3B7D4C2EDBAE6EBDA740");

    bc_cmwx1zzabz_set_band(&lora, BC_CMWX1ZZABZ_CONFIG_BAND_EU868);
    bc_cmwx1zzabz_set_mode(&lora, BC_CMWX1ZZABZ_CONFIG_MODE_ABP);
    bc_cmwx1zzabz_set_class(&lora, BC_CMWX1ZZABZ_CONFIG_CLASS_A);

    //bc_cmwx1zzabz_join(&lora);
}

void application_task()
{
    if (!bc_cmwx1zzabz_is_ready(&lora))
    {
        bc_scheduler_plan_current_relative(200);
        return;
    }

    uint8_t buffer[] = {'A', 'B', 'C'};
    bc_cmwx1zzabz_send_message(&lora, buffer, sizeof(buffer) );

    // do not plan this task anymore
}
