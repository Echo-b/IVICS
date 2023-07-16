#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define VEHICLEINFO 1
#define CROSSINGINFO 2

#define CAR 1
#define TRUCK 2

#define OPEN 1
#define CLOSE_TEAIN 2
#define CLOSE_MALFUNCTION 3
#define CLOSE_OBSTACLE 4
#define CLOSE_INIT 5
#define CLOSE_TYPE 6

#define LED_RED 1
#define LED_GREEN 0

#define DOWN 1
#define UP 0

const char *address = "Crossing 1";

HTTPClient http;

static int RxState = 0;
int USART_RX_STA = 0;         // 接收状态标记
int USART_RX_STACOMPLETE = 0; // 是否完成标记
byte buffer[128];

inline String getTime()
{
    http.begin("http://quan.suning.com/getSysTime.do");
    int httpCode = http.GET(); // 发起GET请求
    if (httpCode > 0)          // 如果状态码大于0说明请求过程无异常
    {
        if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
        {
            String payload = http.getString();
            // Serial.println(payload);
            StaticJsonDocument<200> doc;                                // 声明一个JsonDocument对象
            DeserializationError error = deserializeJson(doc, payload); // 反序列化JSON数据

            if (!error) // 检查反序列化是否成功
            {
                // 读取json节点
                //  String time = ;  //等同于const char *myC = doc["myChar"].as<char*>();
                // Serial.println(time);
                return doc["sysTime2"];
            }
        }
        return "";
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return "";
    }
    http.end(); // 结束当前连接
    return "";
}

inline void receiveMsg()
{
    if (Serial2.available())
    {
        byte i = Serial2.read();
        // Serial.println(i);
        if (RxState == 0)
        {
            if (i == 0xFF)
            {
                USART_RX_STA = 0;
                RxState = 1;
            }
        }
        else if (RxState == 1)
        {
            if (i == 0XFE)
            {
                RxState = 0;
                USART_RX_STACOMPLETE = 1;
            }
            else
            {
                buffer[USART_RX_STA] = i;
                USART_RX_STA++;
            }
        }
    }
}

inline String analysisCrossingInfo()
{
    StaticJsonDocument<1000> doc; // 声明一个JsonDocument对象;
    doc["time"] = getTime();
    doc["address"] = address;

    String crossingStaus;
    String ledStaus;
    String barrierStaus;

    switch (buffer[0])
    {
    case OPEN:
    {
        crossingStaus = "道口开启";
        break;
    }
    case CLOSE_TEAIN:
    {
        crossingStaus = "列车到达，道口关闭";
        break;
    }
    case CLOSE_MALFUNCTION:
    {
        crossingStaus = "道口故障，维修中";
        break;
    }
    case CLOSE_OBSTACLE:
    {
        crossingStaus = "轨道存在障碍物，道口关闭";
        break;
    }
    case CLOSE_INIT:
    {
        crossingStaus = "系统初始化中，道口关闭";
        break;
    }
    default:
    {
        crossingStaus = "道口开启中";
        break;
    }
    }

    switch (buffer[1])
    {
    case LED_RED:
    {
        ledStaus = "红灯";
        break;
    }
    case LED_GREEN:
    {
        ledStaus = "绿灯";
        break;
    }
    default:
        break;
    }

    switch (buffer[2])
    {
    case DOWN:
    {
        barrierStaus = "落下";
        break;
    }
    case UP:
    {
        barrierStaus = "升起";
        break;
    }
    default:
        break;
    }

    doc["crossingStatus"] = crossingStaus;
    doc["ledStatus"] = ledStaus;
    doc["barrierStatus"] = barrierStaus;
    doc["vehicleNum"] = buffer[3];

    JsonArray vehicleInfoArray = doc.createNestedArray("vehicleInflows");

    for (int i = 0; i < buffer[3]; i++)
    {
        String str;
        StaticJsonDocument<200> vehicleInfo; // 声明一个JsonDocument对象
        int curIndex = i * 3 + 4;
        vehicleInfo["id"] = buffer[curIndex];
        switch (buffer[curIndex + 1])
        {
        case TRUCK:
        {
            vehicleInfo["kind"] = "卡车";
            break;
        }
        case CAR:
        {
            vehicleInfo["kind"] = "轿车";
            break;
        }
        default:
            break;
        }
        vehicleInfo["speed"] = buffer[curIndex + 2];
        serializeJson(vehicleInfo, str);
        // Serial.println(str);
        vehicleInfoArray.add(vehicleInfo);
    }

    String str;
    serializeJson(doc, str);
    Serial.println(str);
    return str;
}

inline void sendMsg(String str)
{
    // http.begin("http://192.168.1.152:8080/crossing");
    http.begin("http://192.168.1.109:8080/crossing");
    // http.begin("http://jsonplaceholder.typicode.com/posts");  //Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain"); // Specify content-type header                                                       //int httpCode = http.POST(analysisCrossingInfo());         // 发起POST请求
    int httpResponseCode = http.POST(str);        // Send the actual POST request
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Get the response to the request
        Serial.println(httpResponseCode);   // Print return code
        Serial.println(response);           // Print request answer
    }
    else
    {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }
    http.end(); // 结束当前连接
}

inline void handleMsg()
{
    sendMsg(analysisCrossingInfo());
}

inline void test()
{
    http.begin("http://192.168.1.115:8080");
    int httpCode = http.GET(); // 发起GET请求
    if (httpCode > 0)          // 如果状态码大于0说明请求过程无异常
    {
        if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
        {
            String payload = http.getString();
            Serial.println(payload);
            // Serial.println(payload);
            //  StaticJsonDocument< 200 > doc;                                    //声明一个JsonDocument对象
            //  DeserializationError      error = deserializeJson(doc, payload);  //反序列化JSON数据

            // if (!error)  //检查反序列化是否成功
            // {
            //     //读取json节点
            //     // String time = ;  //等同于const char *myC = doc["myChar"].as<char*>();
            //     //Serial.println(time);
            //     return doc["sysTime2"];
            // }
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end(); // 结束当前连接
}

#endif