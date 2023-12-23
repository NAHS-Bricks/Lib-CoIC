#ifndef NAHS_BRICKS_LIB_COIC_H
#define NAHS_BRICKS_LIB_COIC_H

#include <Arduino.h>
#include <Wire.h>

const int8_t cmd_codes_latch[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, -1, -1, -1};
const int8_t cmd_codes_expander[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5};
const int8_t cmd_codes_latchexpander[] = {0, 1, 2, 3, 8, 9, 10, 12, 14, 4, 5, 6, 7, 11, 13};
#define COIC_DEFAULT_ADDR 45

class NahsBricksLibCoIC {
  public:
    NahsBricksLibCoIC(const int8_t *selected_cmd_code);
    bool begin(uint8_t addr = COIC_DEFAULT_ADDR, TwoWire &wirePort = Wire);
    bool isConnected();
    uint8_t getAddr();

  protected:
    typedef enum class CMD : uint8_t {
      FALLING_EDGE_TRIGGER,
      RISING_EDGE_TRIGGER,
      FALLING_BUMP_TRIGGER,
      RISING_BUMP_TRIGGER,
      CONVERSION_STATE,
      INTERRUPT_CTL,
      STATE_QUEUE_LENGTH,
      LATCH_COUNT,
      OLDEST_STATE,
      EX_DIRECTION,
      EX_PULLUP,
      EX_INVERT,
      EX_OUTPUT,
      EX_INPUT,
      EXPANDER_COUNT
    } CMD_t;
    const int8_t *cmd_code;
    uint8_t _addr;
    TwoWire *_wirePort;
    uint8_t getDate(CMD_t cmd);
    void setDate(CMD_t cmd, uint8_t date);
};

class NahsBricksLibCoIC_Latch: virtual public NahsBricksLibCoIC {
  protected:
    static const uint8_t MAX_LATCHES = 8;
    uint8_t _latch_count;
    uint8_t _latch_state[MAX_LATCHES];
    
  public:
    NahsBricksLibCoIC_Latch();
    bool begin(uint8_t addr = COIC_DEFAULT_ADDR, TwoWire &wirePort = Wire);
    typedef enum class TRIGGER : uint8_t {
      FALLING_EDGE,
      RISING_EDGE,
      FALLING_BUMP,
      RISING_BUMP
    } TRIGGER_t;
    typedef enum class INT_CTL : uint8_t {
      DISABLE,
      LOW_SIG,
      HIGH_SIG,
      LOW_PULSE,
      HIGH_PULSE
    } INT_CTL_t;

  public:
    uint8_t latchCount();
    uint8_t latchState(uint8_t latch_index);
    void setInterrupt(INT_CTL_t mode);
    void conversionBegin();
    void conversionEnd();
    bool readyToFetchStates();
    uint8_t queueLength();
    void fetchLatchStates();
    void setTrigger(uint8_t latch, TRIGGER_t trigger);
    void setAllTriggers(uint8_t latch);
    void setAllTriggers();
    void clearTrigger(uint8_t latch, TRIGGER_t trigger);
    void clearAllTriggers(uint8_t latch);
    void clearAllTriggers();
    uint8_t getTriggers(TRIGGER_t trigger);
};

class NahsBricksLibCoIC_Expander: virtual public NahsBricksLibCoIC {
  protected:
    uint8_t _expander_count;
    uint8_t _outputs;  // holds cache of current outputs-states
    bool _outputs_read = false;  // indicates wether outputs-states have been pulled from CoIC

  public:
    NahsBricksLibCoIC_Expander();
    bool begin(uint8_t addr = COIC_DEFAULT_ADDR, TwoWire &wirePort = Wire);
    void setInput(uint8_t expander);
    void setOutput(uint8_t expander);
    void setPullup(uint8_t expander);
    void clearPullup(uint8_t expander);
    void setHiz(uint8_t expander);
    void clearHiz(uint8_t expander);
    void enableInvert(uint8_t expander);
    void disableInvert(uint8_t expander);
    uint8_t getOutputs();
    void writeOutput(uint8_t expander, bool state);
    void writeOutput(uint8_t expander, uint8_t state);
    void writeOutputs(uint8_t states);
    void toggleOutput(uint8_t expander);
    uint8_t readInput(uint8_t expander);

  public:
    uint8_t expanderCount();
};

class NahsBricksLibCoIC_LatchExpander: public NahsBricksLibCoIC_Latch, public NahsBricksLibCoIC_Expander {
  public:
    NahsBricksLibCoIC_LatchExpander();
    bool begin(uint8_t addr = COIC_DEFAULT_ADDR, TwoWire &wirePort = Wire);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_COIC)
extern NahsBricksLibCoIC_Latch Latch;
extern NahsBricksLibCoIC_Expander Expander;
extern NahsBricksLibCoIC_LatchExpander LatchExpander;
#endif

#endif // NAHS_BRICKS_LIB_COIC_H
