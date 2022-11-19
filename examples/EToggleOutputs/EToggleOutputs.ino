#include <Wire.h>
#include <nahs-Bricks-Lib-CoIC.h>

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin();
  if (!Expander.begin()) {
    Serial.println("Not connected");
    return;
  }
  Serial.println("Connected");

  for (uint8_t expander = 0; expander < Expander.expanderCount(); ++expander) {
    Expander.setOutput(expander);
  }

  Expander.writeOutputs(0x0);

  while(true) {
    for (uint8_t expander = 0; expander < Expander.expanderCount(); ++expander) {
      Expander.toggleOutput(expander);
      delay(1000);
    }
  }
}

void loop() {
}