/*
 * ESP32 ADC 测试代码
 * 只测试 ADC 读取，不用 BLE
 */

#define ADC_PIN 35

void setup() {
    delay(1000);  // 等待串口稳定
    Serial.begin(115200);
    delay(100);
    Serial.println("ADC 测试开始...");
    
    // 配置 ADC
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

void loop() {
    // 读取 ADC 原始值
    int raw = analogRead(ADC_PIN);
    
    // 计算电压
    float voltage = (raw / 4095.0) * 3.3;
    
    // 打印结果
    Serial.print("ADC原始值: ");
    Serial.print(raw);
    Serial.print(" | 电压: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
    
    delay(500);
}
