#include "Message.h"
#include "WIFIModule.h"
#include <Arduino.h>

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial2.begin(9600);
    initWiFi();
}

void loop() {
    // put your main code here, to run repeatedly:
    if (serverMark == 1) {
        dnsServer.processNextRequest();
        server.handleClient();
    }
    checkWiFiConnect();
    restoreWiFi();
    receiveMsg();
    if (USART_RX_STACOMPLETE == 1) {
        Serial.println("receive Msg");
        for (int i = 0; i < USART_RX_STA; i++) {
            Serial.print(buffer[i]);
        }
        USART_RX_STACOMPLETE = 0;
        Serial.println();
        handleMsg();
        //test();
    }
}