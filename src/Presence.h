#pragma once
#include "OpenKNX.h"
#include "PresenceChannel.h"
#include "Sensor.h"
#include "SensorLD2410.h"
#include "SensorMR24xxB1.h"
#include "hardware.h"
#include "knxprod.h"

// maps on KO to an Other KO
// Used for internal KO infrastructure
// working with KO indexes
// originalKoIndex is the index of the KO which is accessed by the firmware
// internalKoIndex is the index of the KO which should be returned instead
struct sKoMap
{
    uint16_t koNumber; // absolute internal KO number as entered in ETS
    uint8_t channelIndex; // channel which uses this internal KO
    uint8_t koIndex; // channel index (within channel) which uses this internal KO
};

class Presence : public OpenKNX::Module
{
public:
  enum LedCaller
  {
      CallerNone,
      CallerMove,
      CallerPresence,
      CallerLock,
      CallerKnxMove,
      CallerKnxPresence
  };

  Presence();
  ~Presence();

  // static
  static void switchHfSensor(bool iOn);

  // instance
  bool PresenceTrigger = false;
  bool MoveTrigger = false;
  bool getHardwarePresence();
  float getHardwareBrightness();
  bool getHardwareMove();
  void processLED(bool iOn, LedCaller iCaller);
  void processReadRequests();
  void processInputKo(GroupObject &iKo);
  void showHelp() override;
  bool processCommand(const std::string iCmd, bool iDebugKo) override;
  void debug();
  void setup();
  void loop();

  void addKoMap(uint16_t iKoNumber, uint8_t iChannelId, uint8_t iKoIndex);
  bool mapKO(uint16_t iKoNumber, sKoMap **iKoMap);
  const std::string name() override;
  const std::string version() override;

private:
  static const uint16_t cCountKoMap = PM_ChannelCount * 6;
  sKoMap mKoMap[cCountKoMap];  // in average 6 internal KO per Channel (4*6*30=720 Byte)
  uint8_t mNumKoMap = 0;

  // support presence hardware
  float mPresenceCombined = 0;
  bool mPresence = false;
  uint8_t mMove = 0;
  float mMoveSpeed = 0;
  uint32_t mPresenceDelay = 0;
  bool mPresenceLedOn = false;
  bool mInfoLedOn = false;
  bool mPresenceChanged = false;
  int8_t mScenario = -1;
  int8_t mSensitivity = 1;
  // brightness is missing
#ifdef HF_SENSOR_MR24xxB1
  SensorMR24xxB1 *mPresenceSensor;
#endif
#ifdef HF_SENSOR_LD2410
  SensorLD2410 *mPresenceSensor;
#endif
  Sensor *mBrightnessSensor;
  uint32_t mBrightnessDelay = 0;
  uint32_t mBrightnessProcess = 0;
  float mLux = NO_NUM;
  float mLuxLast = NO_NUM;
  uint32_t mHfPowerCycleDelay = 0;
  bool mDoPresenceHardwareCycle = 0;

  void startSensors();

  // channel handling
  PresenceChannel *mChannel[PM_ChannelCount];
  uint8_t mNumChannels;
  uint8_t mChannelIterator = 0;
  PresenceChannel *getChannel(uint8_t iChannelId);
  uint8_t getChannelId(PresenceChannel *iChannel);

  void processAfterStartupDelay() override;
  void processHardwarePresence();
  void processHardwareLux();
  void startPowercycleHfSensor();
  void processPowercycleHfSensor();
};
