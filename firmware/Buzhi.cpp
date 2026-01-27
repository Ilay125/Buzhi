#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"

#include "config.h"

#ifdef DEBUG
    #include "pico/cyw43_arch.h"
    #include "modules/tcp_client.h"
    #include "lwip/netif.h"
    #include "creds.h"
#endif

#include "kinematics.h"

#include "modules/debug.h"
#include "modules/mfrc522.h"
#include "modules/motor.h"
#include "modules/servo.h"

#include <stdio.h>
#include <vector>

struct point_data {
    Dir dir;
    int steps;
    double speed;
};

// Shared memory space between cores
Motor* motB = nullptr;
point_data point_dataB;
volatile bool thread_active = false;


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

void core1_entry() {

    printf("Core 1 started, step=%d, dir=%d, speed=%f\n", point_dataB.steps, point_dataB.dir, point_dataB.speed);
    while (true) {

        if(!thread_active) {
            tight_loop_contents();
            continue;
        }

        motB->move(point_dataB.steps, point_dataB.dir, point_dataB.speed);

        thread_active = false;
    }
}

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

    motB = &mot2;

    point_dataB.dir = clockwise;
    point_dataB.steps = 400;
    point_dataB.speed = 50.0;

    multicore_launch_core1(core1_entry);

    mot1.disable();
    mot2.disable();

    // Set servo
    Servo servo(SERVO_PIN, 50);
    servo.set_angle(SERVO_DOWN_ANGLE);

    sleep_us(5);
    
    mot1.enable();
    mot2.enable();

    thread_active = true;
    mot1.move(400, counterclockwise, 50.0);

    mot1.disable();
    mot2.disable();

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