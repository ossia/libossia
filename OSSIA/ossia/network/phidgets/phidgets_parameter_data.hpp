#pragma once
#if defined(__APPLE__)
#include <Phidget22/phidget22.h>
#else
#include <phidget22.h>
#endif
#include <string>
namespace ossia
{
struct phidget_handle_t
{
    PhidgetHandle phid;
    phidget_handle_t(PhidgetHandle h): phid{h}
    {

    }

    int32_t get_serial() const
    {
      int32_t sn;
      Phidget_getDeviceSerialNumber(phid, &sn);
      return sn;
    }

    operator PhidgetHandle() const
    {
      return phid;
    }

    phidget_handle_t get_parent() const
    {
      PhidgetHandle par;
      Phidget_getParent(phid, &par);
      return par;
    }
    auto get_device_class() const
    {
      Phidget_DeviceClass dc;
      Phidget_getDeviceClass(phid, &dc);
      return dc;
    }
    auto get_channel() const
    {
      int dc;
      Phidget_getChannel(phid, &dc);
      return dc;
    }
    auto get_channel_class() const
    {
      Phidget_ChannelClass dc;
      Phidget_getChannelClass(phid, &dc);
      return dc;
    }
    auto get_channel_subclass() const
    {
      Phidget_ChannelSubclass dc;
      Phidget_getChannelSubclass(phid, &dc);
      return dc;
    }
    auto get_device_id() const
    {
      Phidget_DeviceID dc;
      Phidget_getDeviceID(phid, &dc);
      return dc;
    }
    auto get_hub_port() const
    {
      int dc;
      Phidget_getHubPort(phid, &dc);
      return dc;
    }
    auto get_hub_port_count() const
    {
      int dc;
      Phidget_getHubPortCount(phid, &dc);
      return dc;
    }
    bool is_attached() const
    {
      int dc;
      Phidget_getAttached(phid, &dc);
      return dc;
    }
    bool is_hub_port_device() const
    {
      int dc;
      Phidget_getIsHubPortDevice(phid, &dc);
      return dc;
    }
    bool is_channel() const
    {
      int dc;
      Phidget_getIsChannel(phid, &dc);
      return dc;
    }

    std::string get_device_classname()
    {
      return phidget_name(get_device_class());
    }
    std::string get_channel_classname()
    {
      return phidget_name(get_channel_class());
    }
    std::string get_channel_subclassname()
    {
      return phidget_name(get_channel_subclass());
    }
    std::string get_device_id_name()
    {
      return phidget_id(get_device_id());
    }

    const char* phidget_id(Phidget_DeviceID cls)
    {
      switch(cls)
      {
        case PHIDID_NOTHING: return "PHIDID_NOTHING";
        case PHIDID_INTERFACEKIT_4_8_8: return "PHIDID_INTERFACEKIT_4_8_8";
        case PHIDID_1000: return "PHIDID_1000";
        case PHIDID_1001: return "PHIDID_1001";
        case PHIDID_1002: return "PHIDID_1002";
        case PHIDID_1008: return "PHIDID_1008";
        case PHIDID_1010_1013_1018_1019: return "PHIDID_1010_1013_1018_1019";
        case PHIDID_1011: return "PHIDID_1011";
        case PHIDID_1012: return "PHIDID_1012";
        case PHIDID_1014: return "PHIDID_1014";
        case PHIDID_1015: return "PHIDID_1015";
        case PHIDID_1016: return "PHIDID_1016";
        case PHIDID_1017: return "PHIDID_1017";
        case PHIDID_1023: return "PHIDID_1023";
        case PHIDID_1024: return "PHIDID_1024";
        case PHIDID_1030: return "PHIDID_1030";
        case PHIDID_1031: return "PHIDID_1031";
        case PHIDID_1032: return "PHIDID_1032";
        case PHIDID_1040: return "PHIDID_1040";
        case PHIDID_1041: return "PHIDID_1041";
        case PHIDID_1042: return "PHIDID_1042";
        case PHIDID_1043: return "PHIDID_1043";
        case PHIDID_1044: return "PHIDID_1044";
        case PHIDID_1045: return "PHIDID_1045";
        case PHIDID_1046: return "PHIDID_1046";
        case PHIDID_1047: return "PHIDID_1047";
        case PHIDID_1048: return "PHIDID_1048";
        case PHIDID_1049: return "PHIDID_1049";
        case PHIDID_1051: return "PHIDID_1051";
        case PHIDID_1052: return "PHIDID_1052";
        case PHIDID_1053: return "PHIDID_1053";
        case PHIDID_1054: return "PHIDID_1054";
        case PHIDID_1055: return "PHIDID_1055";
        case PHIDID_1056: return "PHIDID_1056";
        case PHIDID_1057: return "PHIDID_1057";
        case PHIDID_1058: return "PHIDID_1058";
        case PHIDID_1059: return "PHIDID_1059";
        case PHIDID_1060: return "PHIDID_1060";
        case PHIDID_1061: return "PHIDID_1061";
        case PHIDID_1062: return "PHIDID_1062";
        case PHIDID_1063: return "PHIDID_1063";
        case PHIDID_1064: return "PHIDID_1064";
        case PHIDID_1065: return "PHIDID_1065";
        case PHIDID_1066: return "PHIDID_1066";
        case PHIDID_1067: return "PHIDID_1067";
        case PHIDID_1202_1203: return "PHIDID_1202_1203";
        case PHIDID_1204: return "PHIDID_1204";
        case PHIDID_1215__1218: return "PHIDID_1215__1218";
        case PHIDID_1219__1222: return "PHIDID_1219__1222";
        case PHIDID_ADP1000: return "PHIDID_ADP1000";
        case PHIDID_ADP1001: return "PHIDID_ADP1001";
        case PHIDID_DAQ1000: return "PHIDID_DAQ1000";
        case PHIDID_DAQ1200: return "PHIDID_DAQ1200";
        case PHIDID_DAQ1300: return "PHIDID_DAQ1300";
        case PHIDID_DAQ1301: return "PHIDID_DAQ1301";
        case PHIDID_DAQ1400: return "PHIDID_DAQ1400";
        case PHIDID_DAQ1500: return "PHIDID_DAQ1500";
        case PHIDID_DCC1000: return "PHIDID_DCC1000";
        case PHIDID_DST1000: return "PHIDID_DST1000";
        case PHIDID_DST1200: return "PHIDID_DST1200";
        case PHIDID_ENC1000: return "PHIDID_ENC1000";
        case PHIDID_HIN1000: return "PHIDID_HIN1000";
        case PHIDID_HIN1001: return "PHIDID_HIN1001";
        case PHIDID_HIN1100: return "PHIDID_HIN1100";
        case PHIDID_HUB0000: return "PHIDID_HUB0000";
        case PHIDID_HUB0001: return "PHIDID_HUB0001";
        case PHIDID_HUB0002: return "PHIDID_HUB0002";
        case PHIDID_HUB0004: return "PHIDID_HUB0004";
        case PHIDID_HUB0005: return "PHIDID_HUB0005";
        case PHIDID_HUM1000: return "PHIDID_HUM1000";
        case PHIDID_LCD1100: return "PHIDID_LCD1100";
        case PHIDID_LED1000: return "PHIDID_LED1000";
        case PHIDID_LUX1000: return "PHIDID_LUX1000";
        case PHIDID_MOT1100: return "PHIDID_MOT1100";
        case PHIDID_MOT1101: return "PHIDID_MOT1101";
        case PHIDID_OUT1000: return "PHIDID_OUT1000";
        case PHIDID_OUT1001: return "PHIDID_OUT1001";
        case PHIDID_OUT1002: return "PHIDID_OUT1002";
        case PHIDID_OUT1100: return "PHIDID_OUT1100";
        case PHIDID_PRE1000: return "PHIDID_PRE1000";
        case PHIDID_RCC1000: return "PHIDID_RCC1000";
        case PHIDID_REL1000: return "PHIDID_REL1000";
        case PHIDID_REL1100: return "PHIDID_REL1100";
        case PHIDID_REL1101: return "PHIDID_REL1101";
        case PHIDID_SAF1000: return "PHIDID_SAF1000";
        case PHIDID_SND1000: return "PHIDID_SND1000";
        case PHIDID_STC1000: return "PHIDID_STC1000";
        case PHIDID_TMP1000: return "PHIDID_TMP1000";
        case PHIDID_TMP1100: return "PHIDID_TMP1100";
        case PHIDID_TMP1101: return "PHIDID_TMP1101";
        case PHIDID_TMP1200: return "PHIDID_TMP1200";
        case PHIDID_TMP1300: return "PHIDID_TMP1300";
        case PHIDID_VCP1000: return "PHIDID_VCP1000";
        case PHIDID_VCP1001: return "PHIDID_VCP1001";
        case PHIDID_VCP1002: return "PHIDID_VCP1002";
        case PHIDID_DIGITALINPUT_PORT: return "PHIDID_DIGITALINPUT_PORT";
        case PHIDID_DIGITALOUTPUT_PORT: return "PHIDID_DIGITALOUTPUT_PORT";
        case PHIDID_VOLTAGEINPUT_PORT: return "PHIDID_VOLTAGEINPUT_PORT";
        case PHIDID_VOLTAGERATIOINPUT_PORT: return "PHIDID_VOLTAGERATIOINPUT_PORT";
        case PHIDID_GENERICUSB: return "PHIDID_GENERICUSB";
        case PHIDID_GENERICVINT: return "PHIDID_GENERICVINT";
        case PHIDID_FIRMWARE_UPGRADE_USB: return "PHIDID_FIRMWARE_UPGRADE_USB";
        case PHIDID_FIRMWARE_UPGRADE_STM32F0: return "PHIDID_FIRMWARE_UPGRADE_STM32F0";
        case PHIDID_FIRMWARE_UPGRADE_STM8S: return "PHIDID_FIRMWARE_UPGRADE_STM8S";
        case PHIDID_FIRMWARE_UPGRADE_SPI: return "PHIDID_FIRMWARE_UPGRADE_SPI";
        case PHIDID_VCP1100: return "PHIDID_VCP1100";
        case PHIDID_VINTSERVO_S2313M: return "PHIDID_VINTSERVO_S2313M";
        case PHIDID_DCC1100: return "PHIDID_DCC1100";
        case PHIDID_HIN1101: return "PHIDID_HIN1101";
        case PHIDID_DCC1001: return "PHIDID_DCC1001";
        case PHIDID_DICTIONARY: return "PHIDID_DICTIONARY";
        case PHIDID_VINTSERVO_S3317M: return "PHIDID_VINTSERVO_S3317M";
        case PHIDID_VINTSERVO_S4309M: return "PHIDID_VINTSERVO_S4309M";
        case PHIDID_VINTSERVO_S4309R: return "PHIDID_VINTSERVO_S4309R";
        case PHIDID_STC1001: return "PHIDID_STC1001";
        case PHIDID_USBSWITCH: return "PHIDID_USBSWITCH";
      }
      return "NONE";
    }


    const char* phidget_name(Phidget_DeviceClass cls)
    {
      switch(cls)
      {
        case PHIDCLASS_ACCELEROMETER: return "ACCELEROMETER";
        case PHIDCLASS_ADVANCEDSERVO: return "ADVANCEDSERVO";
        case PHIDCLASS_ANALOG: return "ANALOG";
        case PHIDCLASS_BRIDGE: return "BRIDGE";
        case PHIDCLASS_ENCODER: return "ENCODER";
        case PHIDCLASS_FREQUENCYCOUNTER: return "FREQUENCYCOUNTER";
        case PHIDCLASS_GPS: return "GPS";
        case PHIDCLASS_HUB: return "HUB";
        case PHIDCLASS_INTERFACEKIT: return "INTERFACEKIT";
        case PHIDCLASS_IR : return "IR";
        case PHIDCLASS_LED: return "LED";
        case PHIDCLASS_MESHDONGLE: return "MESHDONGLE";
        case PHIDCLASS_MOTORCONTROL: return "MOTORCONTROL";
        case PHIDCLASS_PHSENSOR: return "PHSENSOR";
        case PHIDCLASS_RFID: return "RFID";
        case PHIDCLASS_SERVO: return "SERVO";
        case PHIDCLASS_SPATIAL: return "SPATIAL";
        case PHIDCLASS_STEPPER: return "STEPPER";
        case PHIDCLASS_TEMPERATURESENSOR: return "TEMPERATURESENSOR";
        case PHIDCLASS_TEXTLCD: return "TEXTLCD";
        case PHIDCLASS_VINT: return "VINT";
        case PHIDCLASS_GENERIC: return "GENERIC";
        case PHIDCLASS_FIRMWAREUPGRADE: return "FIRMWAREUPGRADE";
        case PHIDCLASS_DICTIONARY: return "DICTIONARY";
        case PHIDCLASS_NOTHING: return "NOTHING";
      }
      return "none";
    }

    const char* phidget_name(Phidget_ChannelClass cls)
    {
      switch(cls)
      {
        case PHIDCHCLASS_NOTHING: return "NOTHING";
        case PHIDCHCLASS_ACCELEROMETER: return "ACCELEROMETER";
        case PHIDCHCLASS_CURRENTINPUT: return "CURRENTINPUT";
        case PHIDCHCLASS_DATAADAPTER: return "DATAADAPTER";
        case PHIDCHCLASS_DCMOTOR: return "DCMOTOR";
        case PHIDCHCLASS_DIGITALINPUT: return "DIGITALINPUT";
        case PHIDCHCLASS_DIGITALOUTPUT: return "DIGITALOUTPUT";
        case PHIDCHCLASS_DISTANCESENSOR: return "DISTANCESENSOR";
        case PHIDCHCLASS_ENCODER: return "ENCODER";
        case PHIDCHCLASS_FREQUENCYCOUNTER: return "FREQUENCYCOUNTER";
        case PHIDCHCLASS_GPS: return "GPS";
        case PHIDCHCLASS_LCD: return "LCD";
        case PHIDCHCLASS_GYROSCOPE: return "GYROSCOPE";
        case PHIDCHCLASS_HUB: return "HUB";
        case PHIDCHCLASS_CAPACITIVETOUCH: return "CAPACITIVETOUCH";
        case PHIDCHCLASS_HUMIDITYSENSOR: return "HUMIDITYSENSOR";
        case PHIDCHCLASS_IR: return "IR";
        case PHIDCHCLASS_LIGHTSENSOR: return "LIGHTSENSOR";
        case PHIDCHCLASS_MAGNETOMETER: return "MAGNETOMETER";
        case PHIDCHCLASS_MESHDONGLE: return "MESHDONGLE";
        case PHIDCHCLASS_PHSENSOR: return "PHSENSOR";
        case PHIDCHCLASS_POWERGUARD: return "POWERGUARD";
        case PHIDCHCLASS_PRESSURESENSOR: return "PRESSURESENSOR";
        case PHIDCHCLASS_RCSERVO: return "RCSERVO";
        case PHIDCHCLASS_RESISTANCEINPUT: return "RESISTANCEINPUT";
        case PHIDCHCLASS_RFID: return "RFID";
        case PHIDCHCLASS_SOUNDSENSOR: return "SOUNDSENSOR";
        case PHIDCHCLASS_SPATIAL: return "SPATIAL";
        case PHIDCHCLASS_STEPPER: return "STEPPER";
        case PHIDCHCLASS_TEMPERATURESENSOR: return "TEMPERATURESENSOR";
        case PHIDCHCLASS_VOLTAGEINPUT: return "VOLTAGEINPUT";
        case PHIDCHCLASS_VOLTAGEOUTPUT: return "VOLTAGEOUTPUT";
        case PHIDCHCLASS_VOLTAGERATIOINPUT: return "VOLTAGERATIOINPUT";
        case PHIDCHCLASS_FIRMWAREUPGRADE: return "FIRMWAREUPGRADE";
        case PHIDCHCLASS_GENERIC: return "GENERIC";
        case PHIDCHCLASS_MOTORPOSITIONCONTROLLER: return "MOTORPOSITIONCONTROLLER";
        case PHIDCHCLASS_BLDCMOTOR: return "BLDCMOTOR";
        case PHIDCHCLASS_DICTIONARY: return "DICTIONARY";
      }
      return "none";
    }

    const char* phidget_name(Phidget_ChannelSubclass cls)
    {
      switch(cls)
      {
        case PHIDCHSUBCLASS_NONE: return "NONE";
        case PHIDCHSUBCLASS_DIGITALOUTPUT_DUTY_CYCLE : return "DIGITALOUTPUT_DUTY_CYCLE";
        case PHIDCHSUBCLASS_DIGITALOUTPUT_LED_DRIVER : return "DIGITALOUTPUT_LED_DRIVER";
        case PHIDCHSUBCLASS_TEMPERATURESENSOR_RTD: return "TEMPERATURESENSOR_RTD";
        case PHIDCHSUBCLASS_TEMPERATURESENSOR_THERMOCOUPLE: return "TEMPERATURESENSOR_THERMOCOUPLE";
        case PHIDCHSUBCLASS_VOLTAGEINPUT_SENSOR_PORT : return "VOLTAGEINPUT_SENSOR_PORT";
        case PHIDCHSUBCLASS_VOLTAGERATIOINPUT_SENSOR_PORT: return "VOLTAGERATIOINPUT_SENSOR_PORT";
        case PHIDCHSUBCLASS_VOLTAGERATIOINPUT_BRIDGE : return "VOLTAGERATIOINPUT_BRIDGE";
        case PHIDCHSUBCLASS_LCD_GRAPHIC: return "LCD_GRAPHIC";
        case PHIDCHSUBCLASS_LCD_TEXT : return "LCD_TEXT";
        case PHIDCHSUBCLASS_ENCODER_MODE_SETTABLE: return "ENCODER_MODE_SETTABLE";
        case PHIDCHSUBCLASS_RCSERVO_EMBEDDED: return "RCSERVO_EMBEDDED";
      }
      return "none";
    }
};

struct phidget_id {
    phidget_id() = default;
    phidget_id(ossia::phidget_handle_t);
    int serialNumber;
    int hubPort;
    int channel;
    int isHubPort;
};
inline bool operator==(phidget_id lhs, phidget_id rhs)
{
  return lhs.serialNumber == rhs.serialNumber &&
      lhs.hubPort == rhs.hubPort &&
      lhs.channel == rhs.channel &&
      lhs.isHubPort == rhs.isHubPort;
}
}

namespace std
{
template<>
class hash<ossia::phidget_id>
{

    inline void hash_combine(std::size_t& seed, int v)
    {
        std::hash<int> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
  public:

    std::size_t operator()(const ossia::phidget_id& id)
    {
      std::size_t s = 0;
      hash_combine(s, id.serialNumber);
      hash_combine(s, id.hubPort);
      hash_combine(s, id.channel);
      hash_combine(s, id.isHubPort);
      return s;
    }

};
}
