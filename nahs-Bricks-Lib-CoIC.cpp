#include "nahs-Bricks-Lib-CoIC.h"


NahsBricksLibCoIC::NahsBricksLibCoIC(const int8_t *selected_cmd_code) {
  cmd_code = selected_cmd_code;
}

bool NahsBricksLibCoIC::begin(uint8_t addr, TwoWire &wirePort) {
  this->_wirePort = &wirePort;
  this->_addr = addr;
  return isConnected();
}

bool NahsBricksLibCoIC::isConnected() {
  _wirePort->beginTransmission(_addr);
  uint8_t r = _wirePort->endTransmission();
  delay(3);  // required -- else CoIC gets into a not-responding state -- 1 would be enough 3 is for safety
  if(r == 0) {
    return true;
  }
  return false;
}

uint8_t NahsBricksLibCoIC::getAddr() {
  return this->_addr;
}

uint8_t NahsBricksLibCoIC::getDate(CMD_t cmd) {
  int8_t code = cmd_code[static_cast<uint8_t>(cmd)];
  if(code < 0) return 0;
  
  _wirePort->beginTransmission(_addr);
  _wirePort->write(code);
  _wirePort->endTransmission();

  _wirePort->requestFrom(_addr, (uint8_t) 1);
  uint8_t result = 0;
  while(_wirePort->available()) {
    result = _wirePort->read();
  }
  return result;
}

void NahsBricksLibCoIC::setDate(CMD_t cmd, uint8_t date) {
  int8_t code = cmd_code[static_cast<uint8_t>(cmd)];
  if(code < 0) return;

  _wirePort->beginTransmission(_addr);
  _wirePort->write(code);
  _wirePort->write(date);
  _wirePort->endTransmission();
}

NahsBricksLibCoIC_Latch::NahsBricksLibCoIC_Latch() : NahsBricksLibCoIC(cmd_codes_latch) {
}

bool NahsBricksLibCoIC_Latch::begin(uint8_t addr, TwoWire &wirePort) {
  if (NahsBricksLibCoIC::begin(addr, wirePort)) {
    _latch_count = getDate(CMD::LATCH_COUNT);
    return (_latch_count > 0);
  }
  return false;
}

uint8_t NahsBricksLibCoIC_Latch::latchCount() {
  return _latch_count;
}

uint8_t NahsBricksLibCoIC_Latch::latchState(uint8_t latch_index) {
  if (latch_index >= MAX_LATCHES) return 0;
  return _latch_state[latch_index];
}

void NahsBricksLibCoIC_Latch::setInterrupt(INT_CTL_t mode) {
  setDate(CMD::INTERRUPT_CTL, static_cast<uint8_t>(mode));
}

void NahsBricksLibCoIC_Latch::conversionBegin() {
  setDate(CMD::CONVERSION_STATE, 1);
}

void NahsBricksLibCoIC_Latch::conversionEnd() {
  setDate(CMD::CONVERSION_STATE, 0);
}

bool NahsBricksLibCoIC_Latch::readyToFetchStates() {
  if(getDate(CMD::CONVERSION_STATE) == 2) return true;
  else return false;
}

uint8_t NahsBricksLibCoIC_Latch::queueLength() {
  return getDate(CMD::STATE_QUEUE_LENGTH);
}

void NahsBricksLibCoIC_Latch::fetchLatchStates() {
  int8_t code = cmd_code[static_cast<uint8_t>(CMD::OLDEST_STATE)];

  _wirePort->beginTransmission(_addr);
  _wirePort->write(code);
  _wirePort->endTransmission();

  _wirePort->requestFrom(_addr, _latch_count);
  uint8_t latch = 0;
  while(_wirePort->available()) {
    _latch_state[latch++] = _wirePort->read();
  }
}

void NahsBricksLibCoIC_Latch::setTrigger(uint8_t latch, TRIGGER_t trigger) {
  if (latch >= _latch_count) return;
  uint8_t trigger_id = static_cast<uint8_t>(trigger);
  CMD_t cmd = static_cast<CMD_t>(static_cast<uint8_t>(CMD::FALLING_EDGE_TRIGGER) + trigger_id);

  uint8_t t = getDate(cmd);
  t |= (1<<latch);
  setDate(cmd, t);
}

void NahsBricksLibCoIC_Latch::setAllTriggers(uint8_t latch) {
  if (latch >= _latch_count) return;
  for (uint8_t i = 0; i < 4; ++i) {
    setTrigger(latch, static_cast<TRIGGER>(i));
  }
}

void NahsBricksLibCoIC_Latch::setAllTriggers() {
  _wirePort->beginTransmission(_addr);
  for (uint8_t i = 0; i < 4; ++i) {
    int8_t code = cmd_code[static_cast<uint8_t>(CMD::FALLING_EDGE_TRIGGER) + i];
    _wirePort->write(code);
    _wirePort->write(0xFF);
  }
  _wirePort->endTransmission();
}

void NahsBricksLibCoIC_Latch::clearTrigger(uint8_t latch, TRIGGER_t trigger) {
  if (latch >= _latch_count) return;
  uint8_t trigger_id = static_cast<uint8_t>(trigger);
  CMD_t cmd = static_cast<CMD_t>(static_cast<uint8_t>(CMD::FALLING_EDGE_TRIGGER) + trigger_id);

  uint8_t t = getDate(cmd);
  t &= ~(1<<latch);
  setDate(cmd, t);
}

void NahsBricksLibCoIC_Latch::clearAllTriggers(uint8_t latch) {
  if (latch >= _latch_count) return;
  for (uint8_t i = 0; i < 4; ++i) {
    clearTrigger(latch, static_cast<TRIGGER>(i));
  }
}

void NahsBricksLibCoIC_Latch::clearAllTriggers() {
  _wirePort->beginTransmission(_addr);
  for (uint8_t i = 0; i < 4; ++i) {
    int8_t code = cmd_code[static_cast<uint8_t>(CMD::FALLING_EDGE_TRIGGER) + i];
    _wirePort->write(code);
    _wirePort->write(0x00);
  }
  _wirePort->endTransmission();
}

uint8_t NahsBricksLibCoIC_Latch::getTriggers(TRIGGER_t trigger) {
  uint8_t trigger_id = static_cast<uint8_t>(trigger);
  CMD_t cmd = static_cast<CMD_t>(static_cast<uint8_t>(CMD::FALLING_EDGE_TRIGGER) + trigger_id);
  return getDate(cmd);
}

NahsBricksLibCoIC_Expander::NahsBricksLibCoIC_Expander() : NahsBricksLibCoIC(cmd_codes_expander) {
  
}

bool NahsBricksLibCoIC_Expander::begin(uint8_t addr, TwoWire &wirePort) {
  if (NahsBricksLibCoIC::begin(addr, wirePort)) {
    _expander_count = getDate(CMD::EXPANDER_COUNT);
    return (_expander_count > 0);
  }
  return false;
}

uint8_t NahsBricksLibCoIC_Expander::expanderCount() {
  return _expander_count;
}

void NahsBricksLibCoIC_Expander::setInput(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t dir = getDate(CMD::EX_DIRECTION);
  dir &= ~(1<<expander);
  setDate(CMD::EX_DIRECTION, dir);
}

void NahsBricksLibCoIC_Expander::setOutput(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t dir = getDate(CMD::EX_DIRECTION);
  dir |= (1<<expander);
  setDate(CMD::EX_DIRECTION, dir);
}

void NahsBricksLibCoIC_Expander::setPullup(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t pu = getDate(CMD::EX_PULLUP);
  pu |= (1<<expander);
  setDate(CMD::EX_PULLUP, pu);
}

void NahsBricksLibCoIC_Expander::clearPullup(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t pu = getDate(CMD::EX_PULLUP);
  pu &= ~(1<<expander);
  setDate(CMD::EX_PULLUP, pu);
}

void NahsBricksLibCoIC_Expander::setHiz(uint8_t expander) {
  clearPullup(expander);
}

void NahsBricksLibCoIC_Expander::clearHiz(uint8_t expander) {
  setPullup(expander);
}

void NahsBricksLibCoIC_Expander::enableInvert(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t inv = getDate(CMD::EX_INVERT);
  inv |= (1<<expander);
  setDate(CMD::EX_INVERT, inv);
}

void NahsBricksLibCoIC_Expander::disableInvert(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t inv = getDate(CMD::EX_INVERT);
  inv &= ~(1<<expander);
  setDate(CMD::EX_INVERT, inv);
}

uint8_t NahsBricksLibCoIC_Expander::getOutputs() {
  if (!_outputs_read) {
    _outputs = getDate(CMD::EX_OUTPUT);
    _outputs_read = true;
  }
  return _outputs;
}

void NahsBricksLibCoIC_Expander::writeOutput(uint8_t expander, bool state) {
  if (expander >= _expander_count) return;
  uint8_t out = getOutputs();
  if (state) out |= (1<<expander);
  else out &= ~(1<<expander);
  setDate(CMD::EX_OUTPUT, out);
  _outputs = out;
}

void NahsBricksLibCoIC_Expander::writeOutput(uint8_t expander, uint8_t state) {
  writeOutput(expander, (state > 0));
}

void NahsBricksLibCoIC_Expander::writeOutputs(uint8_t states) {
  setDate(CMD::EX_OUTPUT, states);
  _outputs = states;
}

void NahsBricksLibCoIC_Expander::toggleOutput(uint8_t expander) {
  if (expander >= _expander_count) return;
  uint8_t out = getOutputs();
  out ^= (1<<expander);
  setDate(CMD::EX_OUTPUT, out);
  _outputs = out;
}

uint8_t NahsBricksLibCoIC_Expander::readInput(uint8_t expander) {
  if (expander >= _expander_count) return 0;
  uint8_t inp = getDate(CMD::EX_INPUT);
  inp &= (1<<expander);
  if (inp == 0) return 0;
  else return 1;
}

NahsBricksLibCoIC_LatchExpander::NahsBricksLibCoIC_LatchExpander() : NahsBricksLibCoIC(cmd_codes_latchexpander) {
  
}

bool NahsBricksLibCoIC_LatchExpander::begin(uint8_t addr, TwoWire &wirePort) {
  return (NahsBricksLibCoIC_Latch::begin(addr, wirePort) && NahsBricksLibCoIC_Expander::begin(addr, wirePort));
}


//------------------------------------------
// globally predefined variables
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_COIC)
NahsBricksLibCoIC_Latch Latch;
NahsBricksLibCoIC_Expander Expander;
NahsBricksLibCoIC_LatchExpander LatchExpander;
#endif
