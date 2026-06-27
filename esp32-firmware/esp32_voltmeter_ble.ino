/*
 * ESP32 BLE 电压表固件 (ADS1115版)
 * 硬件：ESP32 + ADS1115(16bit ADC) + 无线充电接收端
 * 接线：ADS1115 A0 ← 接收端正极
 *       ADS1115 VDD ← ESP32 3.3V
 *       ADS1115 SDA ← GPIO21
 *       ADS1115 SCL ← GPIO22
 */

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE 配置
#define DEVICE_NAME "ESP32-Voltmeter"
#define SERVICE_UUID "0000fff0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "0000fff1-0000-1000-8000-00805f9b34fb"

// ADS1115 配置
#define ADS1115_GAIN GAIN_ONE  // +/- 4.096V 量程
#define ADS1115_SDA 21
#define ADS1115_SCL 22
#define SAMPLE_COUNT 16   // 16次采样取平均，抑制噪声
#define FILTER_ALPHA 0.3   // 滤波系数，响应速度和稳定性之间的平衡点

Adafruit_ADS1X15 ads;



// BLE 对象
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

unsigned long lastSampleTime = 0;
#define SAMPLE_INTERVAL 100 // 100ms = 10Hz 更新率

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; Serial.println("Connected"); }
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; Serial.println("Disconnected"); }
};

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 BLE Voltmeter (ADS1115) starting...");

    // 初始化 I2C 和 ADS1115
    Wire.begin(ADS1115_SDA, ADS1115_SCL);
    if (!ads.begin()) {
        Serial.println("ADS1115 not found!");
        while (1) delay(1000);
    }
    ads.setGain(ADS1115_GAIN);  // +/- 4.096V
    ads.setDataRate(RATE_ADS1115_128SPS); // 128 SPS，低噪声且响应较快
    Serial.println("ADS1115 OK, gain=1 (+/-4.096V), 128SPS");

    initBLE();
    Serial.println("BLE started, name: " + String(DEVICE_NAME));
}

void loop() {
    if (millis() - lastSampleTime >= SAMPLE_INTERVAL) {
        lastSampleTime = millis();

        // ADS1115 多次采样取平均
        long sum = 0;
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            sum += ads.readADC_SingleEnded(0);  // A0通道
        }
        float avgRaw = (float)sum / SAMPLE_COUNT;

        // ADS1115: 16bit, gain=1 → 满量程4.096V
        float rawVoltage = (avgRaw / 32767.0) * 4.096;

        // 一阶低通滤波，抑制跳变和毛刺
        static float filteredVoltage = 0;
        filteredVoltage = filteredVoltage + FILTER_ALPHA * (rawVoltage - filteredVoltage);

        if (deviceConnected) {
            sendVoltage(filteredVoltage);
        }

        Serial.print("Raw: ");
        Serial.print(avgRaw, 0);
        Serial.print(" | RawV: ");
        Serial.print(rawVoltage, 4);
        Serial.print("V | Filtered: ");
        Serial.print(filteredVoltage, 4);
        Serial.println("V");
    }
    handleConnection();
    delay(10);
}



void initBLE() {
    BLEDevice::init(DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService* pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

void sendVoltage(float voltage) {
    uint8_t data[4];
    memcpy(data, &voltage, 4);
    pCharacteristic->setValue(data, 4);
    pCharacteristic->notify();
}

void handleConnection() {
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Re-advertising...");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
