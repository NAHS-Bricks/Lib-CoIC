#include <Wire.h>
#include <nahs-Bricks-Lib-CoIC.h>

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin();
  if (!Latch.begin()) return;
  Serial.println("Starting conversion");
  Latch.conversionBegin();
  Serial.println("Waiting for conversion");
  while(!Latch.readyToFetchStates()) delay(10);
  Latch.fetchLatchStates();
  for(uint8_t i = 0; i < Latch.latchCount(); ++i) {
    Serial.print(Latch.latchState(i));
  }
  Serial.println();
  Serial.flush();
  Latch.conversionEnd();
  delay(100);
  Serial.println("Disableing Interrupt");
  Latch.setInterrupt(NahsBricksLibCoIC_Latch::INT_CTL::DISABLE);
  Latch.setAllTriggers();
  Serial.println("Waiting 20 Seconds for you putting in some Latch changes...");
  for (uint8_t i = 0; i < 20; ++i) {
    delay(1000);
    Serial.print("Queue Length: ");
    Serial.println(Latch.queueLength());
  }
  Serial.println("Waiting done");
  Serial.println("Enableing Interrupt");
  Serial.flush();
  Latch.setInterrupt(NahsBricksLibCoIC_Latch::INT_CTL::LOW_SIG);
}

void loop() {
}