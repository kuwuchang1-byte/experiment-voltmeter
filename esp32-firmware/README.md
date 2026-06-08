# ESP32 代码说明

## 文件列表

| 文件 | 用途 | 何时使用 |
|------|------|----------|
| `esp32_adc_test.ino` | ADC 测试代码 | 测试 ADC 是否正常，调试接线问题 |
| `esp32_voltmeter_ble.ino` | BLE 电压表固件 | 正式使用，连接小程序/网页 |

---

## 当前应该烧录

**esp32_voltmeter_ble.ino** — 这是正式版本，会通过蓝牙发送电压数据。

---

## 烧录步骤

1. 用 Arduino IDE 打开 `esp32_voltmeter_ble.ino`
2. 工具 → 开发板 → ESP32 Dev Module
3. 工具 → 端口 → 你的 COM 口
4. 点击上传按钮
5. 上传成功后，打开串口监视器确认正常

---

## 接线确认

- 接收端 VOUT → ESP32 GPIO35
- 接收端 GND → ESP32 GND

---

## 蓝牙设备名称

烧录后，手机/网页搜索蓝牙设备，找 **ESP32-Voltmeter**。
