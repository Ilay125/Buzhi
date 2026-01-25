#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "config.h"

#ifdef DEBUG
    #include "pico/cyw43_arch.h"
    #include "modules/tcp_client.h"
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


int main_debug() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("CYW43 init failed!\n");
        return 1;
    }


    sleep_ms(1000);

    Debug debug_led;

    cyw43_arch_enable_sta_mode();

    // Connecting to Wifi
    printf("Waiting for connection...\n");
    int rc = cyw43_arch_wifi_connect_async(WIFI_SSID,
                                                WIFI_PASSWORD,
                                                CYW43_AUTH_WPA2_AES_PSK);
    
    if (rc) {
        printf("Connection to %s failed. code %d", WIFI_SSID, rc);
        return 1;
    }

    // --- Wait until STA link is fully ready ---
    while (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
        printf(".");      // optional visual feedback
        cyw43_arch_poll(); // must poll lwIP so packets are processed
        sleep_ms(200);
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



    return 0;
}

int main_release () {
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