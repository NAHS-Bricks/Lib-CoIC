#include <Wire.h>
#include <nahs-Bricks-Lib-CoIC.h>

void setup() {
  Serial.begin(115200);
  Serial.println();

  /*
  First start up the TwoWire port to be used
  */
  Wire.begin();

  /*
  Then start the component with .begin()
  This sets up the I²C Address, the TwoWire port to be used and returns the result of .isConnected()
  If this returns false something is wrong with your connection e.g. the used Address or Port or the physical connection
  */
  if (Latch.begin() == false) {
    Serial.println("Error on connecting to CoIC");
  }
  else {
    Serial.println("Connected");
  }

  /*
  We are going to pull all states based on queue Length, so disableing the interrupt
  */
  Latch.setInterrupt(NahsBricksLibCoIC_Latch::INT_CTL::DISABLE);

  /*
  Enable all triggers on all Latches
  */
  Latch.setAllTriggers();
}

void loop() {
  /*
  Make sure the CoIC is connected
  */
  if (Latch.isConnected()) {

    /*
    Before being able to fetch the states of the latches you have to start a conversion to tell the CoIC to prepare the data to be fetched
    */
    Latch.conversionBegin();

    /*
    After starting the conversion, wait till the CoIC is done preparing it's data
    */
    while(Latch.readyToFetchStates() == false) delay(1);

    /*
    Now it's time to fetch (download) the data from CoIC into an local array
    */
    Latch.fetchLatchStates();

    /*
    Form here it can be used for whatever you like. In this case printig it to Serial
    */
    for(uint8_t i = 0; i < Latch.latchCount(); ++i) {
      Serial.print(Latch.latchState(i));
    }
    Serial.println();
    
    /*
    After you are done processing the data, tell the CoIC about this.
    Please keep in mind: between .conversionBegin() and .conversionEnd() you can call .fetchLatchStates() as many times as you like, but it will allways return the same data.
    Or in other words: to get the next set of LatchStates you have to call .conversionEnd() followed by .conversionStart() (and wait for the States to be ready to fetched)
    */
    Latch.conversionEnd();

    /*
    Wait a bit for CoIC to update the queueLength
    */
    delay(50);

    /*
    If no data is left in queue wait five seconds till we are fetching states again
    */
    if (Latch.queueLength() == 0) delay(5000);
  } else delay(5000);
}
