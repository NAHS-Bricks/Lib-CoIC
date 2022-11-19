#include <Wire.h>
#define NO_GLOBAL_COIC
#include <nahs-Bricks-Lib-CoIC.h>

/*
This time we have defined NO_GLOBAL_COIC before including CoIC Library, therefore we have to instantiate the needed object ourself
Except of the different name we use this time, everything else is the same like in LSimple example
*/
NahsBricksLibCoIC_Latch myLatch;

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
  if (myLatch.begin() == false) {
    Serial.println("Error on connecting to CoIC");
  }
}

void loop() {
  /*
  Make sure the CoIC is connected
  */
  if (myLatch.isConnected()) {
    Serial.println();

    /*
    Before being able to fetch the states of the latches you have to start a conversion to tell the CoIC to prepare the data to be fetched
    */
    Serial.println("Starting conversion");
    myLatch.conversionBegin();

    /*
    After starting the conversion, wait till the CoIC is done preparing it's data
    */
    Serial.println("Waiting for conversion");
    while(myLatch.readyToFetchStates() == false) delay(10);

    /*
    Now it's time to fetch (download) the data from CoIC into an local array
    */
    Serial.println("Fetching Data");
    myLatch.fetchLatchStates();
    Serial.println();

    /*
    Form here it can be used for whatever you like. In this case printig it to Serial
    */
    for(uint8_t i = 0; i < myLatch.latchCount(); ++i) {
      Serial.print("Latch ");
      Serial.print(i);
      Serial.print(" is in state: ");
      Serial.println(myLatch.latchState(i));
    }
    Serial.println();
    
    /*
    After you are done processing the data, tell the CoIC about this.
    Please keep in mind: between .conversionBegin() and .conversionEnd() you can call .fetchLatchStates() as many times as you like, but it will allways return the same data.
    Or in other words: to get the next set of LatchStates you have to call .conversionEnd() followed by .conversionStart() (and wait for the States to be ready to fetched)
    */
    Serial.println("Stopping Conversion");
    myLatch.conversionEnd();
  }

  /*
  Wait two seconds till we are fetching States again
  */
  delay(2000);
}
