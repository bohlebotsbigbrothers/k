#define PIXY_ADDRESS    0x54
#define IR_ADDRESS      0x55
#define KOMPASS_ADDRESS 0x60

ERRORS BohleBots::_i2csync() {
  ERRORS r;
  int    lauf;
  int    ledwert;
  int    tread;
  int    irpaket;
  int    light;
  int    error;

  ZEROFILL(r);
  for (lauf = 0; lauf < 8; lauf++) {
    if (this->_portena[lauf]) {
      ledwert = 255 - this->_led1Array[lauf] - this->_led2Array[lauf];
      Wire.beginTransmission(this->_btnLedID[lauf]);
      Wire.write(ledwert);
      error = Wire.endTransmission();
      if(error) {
        r = (r | CONNECT_FAILED | ERROR_BREAK_OUT);
        DEBUG_ERROR_CODE(r);
        DEBUG_BLOCK("i2c btn led connect fehlgeschlagen!", 1000);
        return r;
      }
      error = Wire.requestFrom(this->_btnLedID[lauf], 1);
      if(error) {
        r = (r | CONNECT_FAILED | ERROR_BREAK_OUT);
        DEBUG_ERROR_CODE(r);
        DEBUG_BLOCK("i2c btn led request fehlgeschlagen!", 1000);
        return r;
      }
      if (Wire.available()) {
        tread = 255 - Wire.read();
        tread = tread % 128;
        this->_button2Array[lauf] = (tread > 63);
        tread = tread % 2;
        this->_button1Array[lauf] = (tread > 0);
      }
      else {
        r = (r | CONNECT_FAILED | ERROR_BREAK_OUT);
        DEBUG_ERROR_CODE(r);
        DEBUG_BLOCK("i2c btn led antwortet nicht!", 1000);
        return r;
      }
    }
  }  // ENDE TastLED
  this->_compassValue = (this->_usesCompass ? this->_readCompass() : 0);
  irpaket = 0;
  error = Wire.requestFrom(IR_ADDRESS, 1);
  if(error) {
    r = (r | CONNECT_FAILED | ERROR_BREAK_OUT);
    DEBUG_ERROR_CODE(r);
    DEBUG_BLOCK("i2c irring connect fehlgeschlagen!", 1000);
    return r;
  }
  if (Wire.available()) {
    irpaket = Wire.read();
  }
  else {
    r = (r | CONNECT_FAILED | ERROR_BREAK_OUT);
    DEBUG_ERROR_CODE(r);
    DEBUG_BLOCK("i2c irring antwortet nicht!", 1000);
    return r;
  }
  this->_ballDirection   = (irpaket % 16) - 7;
  this->_ballDistance =  irpaket / 16;
  this->_ballExists  = (this->_ballDistance != 0);
  if (!this->_ballExists) this->_ballDirection = 0;
  // Lichtschranke (nicht wirklich i2c...)
  light = this->input(this->_lightPin);
  this->_hasBall = (light > this->_ballThreshold);
  return r;
}
