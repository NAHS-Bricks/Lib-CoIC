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

  Expander.setOutput(0);
  for (uint8_t expander = 1; expander < Expander.expanderCount(); ++expander) {
    Expander.setInput(expander);
    Expander.setPullup(expander);
  }

  while(true) {
    delay(1000);
    Expander.toggleOutput(0);
    for (uint8_t expander = 1; expander < Expander.expanderCount(); ++expander) {
      Serial.print(Expander.readInput(expander));
    }
    Serial.println();
  }
}

void loop() {
}