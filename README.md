# NAHS-Bricks-Lib-CoIC

For having some more IO on an ESP8266 I build a project around ATtiny212 or ATtiny414 to give me some static IO (Expander) or triggered inputs (Latch).

This library now interfaces this, so called CoIC, via I²C to fetch the inputs and set the outputs.

## Preinstantiated Objects

By including the Library you have the following Objects preintantiated:

*  `Latch` as instance of NahsBricksLibCoIC_Latch
*  `Expander` as instance of NahsBricksLibCoIC_Expander
*  `LatchExpander` as instance of NahsBricksLibCoIC_LatchExpander

If you don't want this you can either define `NO_GLOBAL_COIC` or `NO_GLOBAL_INSTANCES` before you include the library for the first time. In this case you can (and have to) define the desired variables by your self e.g. with: `NahsBricksLibCoIC_Latch latch;` or as a pointer `NahsBricksLibCoIC_Latch *latch = new NahsBricksLibCoIC_Latch();`

## Examples

There are a bunch of examples contained in this library. This overview is giving you an idea what they do.

### Expander

#### EAllInput

All IOs of an Expander (or LatchExpander) are configured as inputs and their pullups are activated. Every second all inputs are now fetched and displayed on Serial as 1 or 0.

#### EMixed

The first IO of an Expander (or LatchExpander) is configured as an output all others as inputs with their pullups activated. Every second the output is toggled (altered between high and low) and the inputs are fetched and displayed on Serial as 1 or 0.

#### EToggleOutputs

All IOs of an Expander (or LatchExpander) are configured as outputs. Every second all outputs are toggled (altered between high and low)

### Latch

#### LSimple

The Latch (or LatchExpander) is used in polling mode. Every two seconds all Latch IO states are fetched from CoIC and displayed on Serial. For how this polling (conversation) is done, please refer to the inline comments.

#### LInterrupt

This example now uses the interrupt line of Latch (or LatchExpander) it is designed for the interrupt to be connected to the reset of the MainIC.

In the setup first one conversation is done with the CoIC where the oldest states are pulled from CoIC and displayed on Serial. After this you are given 20 Seconds to genereate some changes. The length of the queue is shown during this time.

After this 20 Seconds the interrupt is activated. If there are some data in the queue the interrupt is executed and therefore the MainIC reset and the process starts over.

#### LPulling TriggeredStates

This example is similar to to LSimple but this time not just the hard states (0 or 1) are display but also all triggered states of Latch (or LatchExpander). Please refer to the inline comments on how this works.

### LatchExpander


#### LEFullyMixed

This example uses all features of LatchExpander at the same time. The Latches are used in polling mode. The first IO of Expander is set as an output all other Expander IOs are set as input (with pullup).

Every second the output is toggled (altered between high and low) and every five seconds all inputs and Latches are pulled and displayed on Serial.

Please refer to the inline comments for a detailed description of what is done.

### General

#### NoPreinstatiatedObjects

This example is identical to LSimple but this time the Latch object is not instantiated globally but through the main program.