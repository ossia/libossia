#pragma once
#include <functional>
#include <iostream>
#include <mutex>
#include <phidget22.h>
namespace ppp
{
/*
struct interface_kit
{
public:
  interface_kit(int serial)
  {
    PhidgetInterfaceKit_create(&m_handle);

    PhidgetInterfaceKit_set_OnInputChange_Handler(
        m_handle,
        [](PhidgetInterfaceKitHandle, void* userPtr, int index,
           int st) -> int {
          auto& self = *(interface_kit*)(userPtr);
          if (self.onInputChange)
            self.onInputChange(index, st);
          return EPHIDGET_OK;
        },
        this);
    PhidgetInterfaceKit_set_OnOutputChange_Handler(
        m_handle,
        [](PhidgetInterfaceKitHandle, void* userPtr, int index,
           int st) -> int {
          auto& self = *(interface_kit*)(userPtr);
          if (self.onOutputChange)
            self.onOutputChange(index, st);
          return EPHIDGET_OK;
        },
        this);
    PhidgetInterfaceKit_set_OnSensorChange_Handler(
        m_handle,
        [](PhidgetInterfaceKitHandle, void* userPtr, int index,
           int sensorValue) -> int {
          auto& self = *(interface_kit*)(userPtr);

          std::lock_guard<std::mutex> l{self.m_ioMutex};
          if (self.m_sensorChange)
            self.m_sensorChange(index, sensorValue);
          return EPHIDGET_OK;
        },
        this);

    Phidget_open((PhidgetHandle)m_handle, serial);
  }

  ~interface_kit()
  {
    Phidget_close((PhidgetHandle)m_handle);
    Phidget_delete((PhidgetHandle)m_handle);
  }

  PhidgetHandle get_base_handle() const
  {
    return (PhidgetHandle)m_handle;
  }

  int get_sensor_count() const
  {
    int c;
    PhidgetInterfaceKit_getSensorCount(m_handle, &c);
    return c;
  }

  int get_sensor_value(int idx) const
  {
    int c;
    PhidgetInterfaceKit_getSensorValue(m_handle, idx, &c);
    return c;
  }

  int get_sensor_raw_value(int idx) const
  {
    int c;
    PhidgetInterfaceKit_getSensorRawValue(m_handle, idx, &c);
    return c;
  }

  int get_data_rate(int idx)
  {
    int ms;
    PhidgetInterfaceKit_getDataRate(m_handle, idx, &ms);
    return ms;
  }

  void set_data_rate(int idx, int ms)
  {
    PhidgetInterfaceKit_setDataRate(m_handle, idx, ms);
  }

  int inputs() const
  {
    int m;
    PhidgetInterfaceKit_getInputCount(m_handle, &m);
    return m;
  }

  int outputs() const
  {
    int m;
    PhidgetInterfaceKit_getOutputCount(m_handle, &m);
    return m;
  }

  std::function<void(int, int)> onInputChange;
  std::function<void(int, int)> onOutputChange;

  void set_sensor_change(std::function<void(int, int)> m)
  {
    std::lock_guard<std::mutex> l{m_ioMutex};
    m_sensorChange = std::move(m);
  }

private:
  PhidgetInterfaceKitHandle m_handle;
  std::function<void(int, int)> m_sensorChange;
  std::mutex m_ioMutex;
};

struct accelerometer
{
public:
  accelerometer(PhidgetAccelerometerHandle h) : m_handle{h}
  {
  }

private:
  PhidgetAccelerometerHandle m_handle;
};

struct advanced_servo
{
public:
  advanced_servo(PhidgetAdvancedServoHandle h) : m_handle{h}
  {
  }

private:
  PhidgetAdvancedServoHandle m_handle;
};

struct bridge
{
public:
  bridge(PhidgetBridgeHandle h) : m_handle{h}
  {
  }

private:
  PhidgetBridgeHandle m_handle;
};

struct frequency_counter
{
public:
  frequency_counter(PhidgetFrequencyCounterHandle h) : m_handle{h}
  {
  }

private:
  PhidgetFrequencyCounterHandle m_handle;
};

struct gps
{
public:
  gps(PhidgetGPSHandle h) : m_handle{h}
  {
  }

private:
  PhidgetGPSHandle m_handle;
};

struct ir
{
public:
  ir(PhidgetIRHandle h) : m_handle{h}
  {
  }

private:
  PhidgetIRHandle m_handle;
};

struct led
{
public:
  led(PhidgetLEDHandle h) : m_handle{h}
  {
  }

private:
  PhidgetLEDHandle m_handle;
};

struct motor_control
{
public:
  motor_control(PhidgetMotorControlHandle h) : m_handle{h}
  {
  }

private:
  PhidgetMotorControlHandle m_handle;
};

struct ph_sensor
{
public:
  ph_sensor(PhidgetPHSensorHandle h) : m_handle{h}
  {
  }

private:
  PhidgetPHSensorHandle m_handle;
};

struct rfid
{
public:
  rfid(PhidgetRFIDHandle h) : m_handle{h}
  {
  }

private:
  PhidgetRFIDHandle m_handle;
};

struct servo
{
public:
  servo(PhidgetServoHandle h) : m_handle{h}
  {
  }

private:
  PhidgetServoHandle m_handle;
};

struct spatial
{
public:
  spatial(PhidgetSpatialHandle h) : m_handle{h}
  {
  }

private:
  PhidgetSpatialHandle m_handle;
};

struct stepper
{
public:
  stepper(PhidgetStepperHandle h) : m_handle{h}
  {
  }

private:
  PhidgetStepperHandle m_handle;
};

struct temperature_sensor
{
public:
  temperature_sensor(PhidgetTemperatureSensorHandle h) : m_handle{h}
  {
  }

private:
  PhidgetTemperatureSensorHandle m_handle;
};

struct text_lcd
{
public:
  text_lcd(PhidgetTextLCDHandle h) : m_handle{h}
  {
  }

private:
  PhidgetTextLCDHandle m_handle;
};

struct text_led
{
public:
  text_led(PhidgetTextLEDHandle h) : m_handle{h}
  {
  }

private:
  PhidgetTextLEDHandle m_handle;
};

struct weight_sensor
{
public:
  weight_sensor(PhidgetWeightSensorHandle h) : m_handle{h}
  {
  }

private:
  PhidgetWeightSensorHandle m_handle;
};
*/
}
