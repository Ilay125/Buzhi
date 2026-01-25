#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "config.h"

#ifdef DEBUG
    #include "pico/cyw43_arch.h"
    #include "modules/tcp_client.h"
    #include "lwip/netif.h"
    #include "creds.h"
#endif

#include "hardware/pio.h"
#include "stepper.pio.h"

#include "kinematics.h"

#include "modules/debug.h"
#include "modules/mfrc522.h"
#include "modules/motor.h"
#include "modules/servo.h"

#include <stdio.h>
#include <vector>

#ifdef DEBUG
int main_debug() {
    stdio_init_all();
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_ISRAEL)) {
        printf("CYW43 init failed!\n");
        return 1;
    }


    sleep_ms(1000);

    Debug debug_led;

    cyw43_arch_enable_sta_mode();

    // Connecting to Wifi
    printf("Waiting for connection...\n");
    int rc = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID,
                                                WIFI_PASSWORD,
                                                CYW43_AUTH_WPA2_AES_PSK,
                                                30000);
    
    if (rc) {
        printf("Connection to %s failed. code %d", WIFI_SSID, rc);
        return 1;
    }

    printf("Connected to %s\n", WIFI_SSID);

    sleep_ms(1000);

    debug_led.blink(3, 200);

    // Connecting to client
    TcpClient client;
    client.set_server_ip(SERVER_IP);
    client.set_server_port(SERVER_PORT);

    client.set_iterations(10);
    client.set_poll_interval_seconds(5);

    if (!client.start()) {
        printf("Failed to start TCP client.\n");
        cyw43_arch_deinit();
        return 1;
    }

    printf("Connected to client %s:%d\n", SERVER_IP, SERVER_PORT);

    while (true) {
        printf("im here!");
        cyw43_arch_poll();
        client.service();
        sleep_ms(100);
    }



    return 0;
}
#endif

int main_release () {
    stdio_init_all();

    sleep_ms(1000);

    // Debug LED init
    Debug debug_led;


    // Inverse kinematcs for homing
    double s1, s2;
    inverse_kin(X0, Y0, s1, s2);

    // Set motors
    Motor mot1(STEP1_PIN, DIR1_PIN, ENABLE1_PIN, s1);
    Motor mot2(STEP2_PIN, DIR2_PIN, ENABLE2_PIN, s2);

    mot1.disable();
    mot2.disable();

    // Set servo
    Servo servo(SERVO_PIN, 50);
    servo.set_angle(SERVO_DOWN_ANGLE);

    sleep_us(5);

    // PIO SETUP
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &move_program); // Flash PIO instructions

    uint sA = pio_claim_unused_sm(pio, true);
    uint sB = pio_claim_unused_sm(pio, true);

    pio_sm_set_clkdiv(pio, sA, PIO_CLKDIV);
    pio_sm_set_clkdiv(pio, sB, PIO_CLKDIV);


    // ---smA---
    pio_sm_config cA = move_program_get_default_config(offset);
    sm_config_set_sideset_pins(&cA, STEP1_PIN);
    pio_sm_set_consecutive_pindirs(pio, sA, STEP1_PIN, 1, true);

    // ---smB---
    pio_sm_config cB = move_program_get_default_config(offset);
    sm_config_set_sideset_pins(&cB, STEP2_PIN);
    pio_sm_set_consecutive_pindirs(pio, sB, STEP2_PIN, 1, true);

    pio_sm_init(pio, sA, offset, &cA);
    pio_sm_init(pio, sB, offset, &cB);

    // Starts both machines on same cycle
    pio_enable_sm_mask_in_sync(pio, (1u << sA) | (1u << sB));

    sleep_ms(2000);
    // Main loop
    printf("Main loop\n");
    while (true) {
        inverse_kin(X0, Y0-1, s1, s2);
        printf("moving\n");

        mot1.enable();
        mot2.enable();
        debug_led.set(1);
        move_motors(mot1, mot2, s1, s2, 1, pio, sA, sB);
        debug_led.set(0);
        mot1.disable();
        mot2.disable();

        sleep_ms(1000);
    }

    return 0;
}

int main() {

    #ifdef DEBUG
        return main_debug();
    #else
        return main_release();
    #endif

    return 0;
}