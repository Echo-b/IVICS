#ifndef WIFIMODULE_H
#define WIFIMODULE_H
#include "ESPmDNS.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wifi.h>

// WEB服务器端口
#define WEB_PORT 80
// 配网网页
#define ROOT_HTML " <!DOCTYPE html> <html> <head> <title>WIFI Config</title> <meta charset='UTF-8'> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> </head> <style type=\"text/css\"> .input { display: block; margin-top: 10px; } .input span { width: 100px; float: left; float: left; height: 36px; line-height: 36px; } .input input { height: 30px; width: 200px; } .btn { width: 120px; height: 35px; background-color: #000000; border: 0px; color: #ffffff; margin-top: 15px; margin-left: 100px; } </style> <body> <form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span><input type=\"text\" name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P> </form>"
// 自动连接超时时间
#define OUTTIME 30
// WiFi连接指示灯
#define WiFiLed 2
// 复位键
#define RESTART 0

IPAddress AP_IP(192, 168, 4, 1);
const char *AP_SSID = "RSU_WiFiConfig";
const char *AP_PASSWORD = "12345678";

String scanNetworksID = "";
String wifi_ssid = "";
String wifi_pass = "";
int serverMark = 0;

const byte DNS_PORT = 53; // 设置DNS端口号

DNSServer dnsServer;        // 创建dnsServer实例
WebServer server(WEB_PORT); // 开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数

// 初始化AP
void initSoftAP()
{
    Serial.println("initSoftAP");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID, AP_PASSWORD))
    {
        Serial.print("Local IP:");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("WiFi AP init Failed");
        Serial.println("ready restart...");
        delay(3000);
        ESP.restart();
    }
}

// 扫描周围WiFi并存储
inline bool scanfWiFi()
{
    String str;
    Serial.println("");
    Serial.println("scan start");
    WiFi.mode(WIFI_STA); // 设置为STA模式
    WiFi.disconnect();   // 断开当前可能的连接
    int n = WiFi.scanNetworks();
    while (n == -2)
    {
        Serial.println("扫描失败");
        n = WiFi.scanNetworks();
    }
    if (n == 0)
    {
        Serial.println("no networks found");
        str = String("no networks found");
        scanNetworksID = "<P>" + str + "</P>";
        return false;
    }
    else
    {
        Serial.print("networks found:");
        Serial.println(n);
        for (int i = 0; i < n; i++)
        {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")  ");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "lock");
            scanNetworksID += "<P>" + WiFi.SSID(i) + "  " + String(WiFi.RSSI(i)) + "</P>";
        }
        return true;
    }
}

// 初始化DNS服务器
void initDNS()
{
    // 判断将所有地址映射到esp32的ip上是否成功
    if (dnsServer.start(DNS_PORT, "*", AP_IP))
    {
        Serial.println("start dns server success.");
    }
    else
    {
        Serial.println("start dns server failed.");
    }
}

// 使用web端获取的账号密码连接
void connectToWiFi()
{
    Serial.println("进入connectToWiFi()函数");
    // 设置为STA模式并连接WIFI
    WiFi.mode(WIFI_STA);
    Serial.print("用web配置信息连接");
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    wifi_ssid = "";
    wifi_pass = "";
    int Connect_time = 0; // 用于连接计时，如果长时间连接不成功，复位设备
    while (WiFi.status() != WL_CONNECTED)
    { // 等待WIFI连接成功
        Serial.println(".");
        delay(1000);
        Connect_time++;
        if (Connect_time > OUTTIME)
        {
            // 长时间连接不上，复位
            Serial.println("");
            Serial.println("WIFI autoconnect fail, restart now...");
            ESP.restart();
            return;
        }
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WIFI connect Success");
        Serial.printf("SSID:%s", WiFi.SSID().c_str());
        Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
        Serial.print("LocalIP:");
        Serial.print(WiFi.localIP());
        Serial.print(" ,GateIP:");
        Serial.println(WiFi.gatewayIP());
        Serial.print("WIFI status is:");
        Serial.println(WiFi.status());
        digitalWrite(WiFiLed, HIGH);
    }
}

// 处理网站根目录“/”(首页)的访问请求,将显示配置wifi的HTML页面
void handleRoot()
{
    // scanfWiFi();
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
}

// 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
void handleNotFound()
{
    handleRoot(); // 访问不存在目录则返回配置页面
}

// 处理配置页
void handleConfigWifi()
{
    Serial.print("get ssid:");
    wifi_ssid = server.arg("ssid");
    Serial.println(wifi_ssid);

    Serial.print("get password:");
    wifi_pass = server.arg("pass");
    Serial.println(wifi_pass);

    // 判断是否有账号
    if (wifi_ssid.length() == (unsigned int)0)
    {
        Serial.println("error, not found ssid");
        server.send(200, "text/html",
                    "<meta charset='UTF-8'>未填写账号，请手动返回上一页"); // 返回错误页面
        return;
    }
    server.send(200, "text/html",
                "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); // 返回保存成功页面
    delay(2000);
    WiFi.softAPdisconnect(true); // 参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。

    server.close(); // 关闭web服务
    dnsServer.stop();
    serverMark = 0;

    WiFi.softAPdisconnect(); // 在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("开始调用连接函数connectToWiFi()");
        connectToWiFi();
    }
}

// 初始化服务器
void initWebServer()
{
    // 必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
    server.on(
        "/", HTTP_GET,
        handleRoot); //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
    server.on(
        "/configwifi", HTTP_POST,
        handleConfigWifi); //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
    server.onNotFound(
        handleNotFound); // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
    // Tells the server to begin listening for incoming connections.Returns None
    server.begin(); // 启动TCP SERVER
    serverMark = 1;
    Serial.println("WebServer Ready!");
}

// web配网
void smartConnect()
{
    scanfWiFi();
    initSoftAP();
    initDNS();
    initWebServer();
}

// 用于上电自动连接WiFi 20s后未自动连接成功则返回失败
bool autoConnect()
{
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true); // 设置自动连接
    WiFi.begin();
    for (int i = 0; i < 20; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
            Serial.println("WIFI AutoConnect Success");
            Serial.printf("SSID:%s", WiFi.SSID().c_str());
            Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
            Serial.print("LocalIP:");
            Serial.print(WiFi.localIP());
            Serial.print(" ,GateIP:");
            Serial.println(WiFi.gatewayIP());
            digitalWrite(WiFiLed, HIGH);
            return true;
        }
        else
        {
            Serial.print("WIFI AutoConfig Waiting......");
            Serial.println(wstatus);
            delay(500);
        }
    }
    Serial.println("WIFI AutoConfig Faild!");
    return false;
}

// 清空wifi信息
void restoreWiFi()
{
    if (digitalRead(RESTART) == LOW)
    {
        delay(1000);
        esp_wifi_restore();
        Serial.println("restore WiFi Config ,ready restart");
        delay(10);
        ESP.restart();
    }
}

// 初始化化WiFI
void initWiFi()
{
    pinMode(WiFiLed, OUTPUT);
    digitalWrite(WiFiLed, LOW);
    if (!autoConnect())
    {
        smartConnect();
    }
}

// 检查WiFI连接与重连
void checkWiFiConnect()
{
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() == WIFI_STA)
    {
        Serial.println("WIFI未连接.");
        Serial.println("正在重新连接WiFi...");
        digitalWrite(WiFiLed, LOW);
        initWiFi();
    }
}

#endif