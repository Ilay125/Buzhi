#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/rand.h"

#include "config.h"

#ifdef DEBUG
    #include "pico/cyw43_arch.h"
    #include "modules/tcp_client.h"
    #include "lwip/netif.h"
    #include "creds.h"
#endif

#include "kinematics.h"
#include "commands.h"
#include "commandQ.h"
#include "screen_manager.h"

#include "modules/debug.h"
#include "modules/mfrc522.h"
#include "modules/motor.h"
#include "modules/servo.h"
#include "modules/nfc_utils.h"
#include "modules/screen.h"

#include <stdio.h>
#include <vector>


// Shared memory space between cores
Motor* motB = nullptr;
point_data* point_dataB = nullptr;
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

    printf("Core 1 started, step=%d, dir=%d, speed=%f\n", point_dataB->steps, point_dataB->dir, point_dataB->speed);
    while (true) {

        if(!thread_active) {
            tight_loop_contents();
            continue;
        }

        motB->move(point_dataB->steps, point_dataB->dir, point_dataB->speed);

        thread_active = false;
    }
}

int main_release () {
    std::vector<point_data*> pointsA;
    std::vector<point_data*> pointsB;
    CommandQueue command_queue;

    stdio_init_all();

    sleep_ms(1000);

    // Debug LED init
    Debug debug_led;

    // Screen init
    Screen screen(
        spi1,
        PIN_SCREEN_SCK,
        PIN_SCREEN_MOSI,
        PIN_SCREEN_DC,
        PIN_SCREEN_CS,
        PIN_SCREEN_RST
    );

    Screen_Manager screen_manager(&screen);

    // Inverse kinematcs for homing
    double s1, s2;
    inverse_kin(X0, Y0, s1, s2);

    // Set motors
    Motor mot1(STEP1_PIN, DIR1_PIN, ENABLE1_PIN, s1);
    Motor mot2(STEP2_PIN, DIR2_PIN, ENABLE2_PIN, s2);

    mot1.disable();
    mot2.disable();

    motB = &mot2;

    // Set servo
    Servo servo(SERVO_PIN, 50);
    servo.set_angle(SERVO_UP_ANGLE);
    int last_servo_angle = SERVO_UP_ANGLE;

    // Set NFC reader
    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);
    sleep_ms(500);
    uint8_t version = PCD_ReadRegister(mfrc, VersionReg);
    printf("MFRC522 Version: 0x%02X\n\r", version);

    std::vector<uint8_t> nfc_data;
    
    multicore_launch_core1(core1_entry);


    // Drawing sequence - Main Loop
    sleep_ms(2000);

    // Show looking for card screen
    screen_manager.set_state(LOOKING_FOR_CARD);
    while (true) {
        // Setting home position - move manually for calibration
        servo.set_angle(SERVO_UP_ANGLE);
        inverse_kin(X0, Y0, s1, s2);
        mot1.home(s1);
        mot2.home(s2);

        // Update screen animation
        screen_manager.update();

        // Waiting for NFC card
        printf("Waiting for NFC card...\n");
        while (!PICC_IsNewCardPresent(mfrc)) {

            // Update screen animation while waiting for card
            screen_manager.update();
        }

        if (!PICC_ReadCardSerial(mfrc)) continue;

        printf("Card UID:");
        for (uint8_t u = 0; u < mfrc->uid.size; u++) {
            printf(" %02X", mfrc->uid.uidByte[u]);
        }
        printf("\n\r");

        // Read bytes from card
        nfc_data.clear();

        const uint8_t START_PAGE = 0x04;
        const uint8_t END_PAGE   = 0xE2; // exclusive

        bool read_error = false;

        for (uint8_t page = START_PAGE; page < END_PAGE; page += 4) {
            uint8_t buffer[18];
            uint8_t size = sizeof(buffer);

            StatusCode status = MIFARE_Read(mfrc, page, buffer, &size);
            if (status != STATUS_OK) {
                printf("Read error at page %02X: %s\n\r", page, GetStatusCodeName(status));
                read_error = true;
                break;
            }

            // buffer[0..15] are the 16 data bytes (4 pages)
            for (int b = 0; b < 16; b++) {
                nfc_data.push_back(buffer[b]);
            }
        }

        if (read_error) {
            PICC_HaltA(mfrc);
            continue;
        }

        mot1.disable();
        mot2.disable();

        sleep_ms(500);

        // Extract data
        std::vector<uint8_t> only_data;
        if (!extract_only_data_raw_commands(nfc_data, only_data)) {
            printf("Failed to extract raw command stream (no M/L/C/Z found)\n\r");
            PICC_HaltA(mfrc);
            continue;
        }

        printf("ONLY DATA (%lu bytes):\n\r", (unsigned long)only_data.size());
        for (size_t k = 0; k < only_data.size(); k++) {
            printf("%02X ", only_data[k]);
        }
        printf("\n\r");

        PICC_HaltA(mfrc);
        
        // Reset the reader after halt so it can detect new cards
        PCD_Reset(mfrc);
        sleep_ms(50);

        // Parse commands
        command_queue.parse(only_data, only_data.size());
        //command_queue.add_command(new Move(X0 - X_OFFSET, Y0 + 3 - Y_OFFSET));
        //command_queue.add_command(new Line(X0 - X_OFFSET, Y0 + 3 - Y_OFFSET, X0 - X_OFFSET, Y0 - 8 - Y_OFFSET));
        // Generate drawing vectors

        debug_led.set(true);

        command_queue.gen_draw_vec(mot1, mot2, pointsA, pointsB);

        if (pointsA.size() != pointsB.size()) {
            printf("Error: pointsA and pointsB have different sizes.\n");
            return 1;
        }

        // Drawing sequence
        printf("\n\n-------------------\nSTARTING DRAWING\n-------------------\nGenerated %d points for drawing.\n", (int)pointsA.size());

        screen_manager.set_state(DRAWING);

        sleep_ms(1000);
        mot1.enable();
        mot2.enable();
        sleep_ms(500);
        printf("Motors enabled!\n");

        for (int i = 0; i < pointsA.size(); i++) {

            // Update drawing progress on screen every 50 points to avoid excessive updates
            if (i % 50 == 0) {
                screen_manager.set_drawing_progress((i + 1) / (double)pointsA.size());
                screen_manager.update();
            }

            point_data* pointA = pointsA[i];
            point_data* pointB = pointsB[i];


            if (pointA->servo_angle != pointB->servo_angle) {
                printf("Warning: pointA and pointB have different servo angles (%f vs %f). Using pointA's angle.\n", pointA->servo_angle, pointB->servo_angle);
            }
            
            
            if (last_servo_angle != pointA->servo_angle) {
                sleep_ms(500);
                mot1.disable();
                mot2.disable();

                // use the stall to update screen.
                screen_manager.set_drawing_progress((i + 1) / (double)pointsA.size());
                screen_manager.update();

                sleep_ms(500); // Small delay before moving servo, adjust as needed
                printf("Setting servo angle from %d to %d\n", last_servo_angle, pointA->servo_angle);
                servo.set_angle(pointA->servo_angle);
                sleep_ms(500); // Small delay to allow servo to move up before moving motors

                if (pointA->servo_angle == SERVO_UP_ANGLE || last_servo_angle == SERVO_UP_ANGLE) {
                    // If we're moving up, wait a bit longer
                    sleep_ms(1000);
                }


                last_servo_angle = pointA->servo_angle;

                mot1.enable();
                mot2.enable();
            }


            printf("Drawing point %d: Motor 1 - steps=%d, dir=%d, speed=%f; Motor 2 - steps=%d, dir=%d, speed=%f; Servo - angle=%d\n",
                i, pointA->steps, pointA->dir, pointA->speed,
                pointB->steps, pointB->dir, pointB->speed, pointA->servo_angle);

            // Set shared data for core 1
            point_dataB = pointB;
            thread_active = true;

            // Move motor 1 in main thread
            mot1.move(pointA->steps, pointA->dir, pointA->speed);

            // Wait for core 1 to finish
            while (thread_active) {
                tight_loop_contents();
            }

            delete pointA;
            delete pointB;

            //sleep_us(100); // Small delay between points, adjust as needed
        }
        
        mot1.disable();
        mot2.disable();

        servo.set_angle(SERVO_UP_ANGLE);

        debug_led.set(false);

        sleep_ms(1000);

        screen_manager.set_state(LOOKING_FOR_CARD);
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