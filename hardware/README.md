# Hardware Implemention

A dedicated hardware has to be designed in order to adjust the amplitude and
the offset of the generated signal. These adjustments should be done with
potentiometers or trimmers. The developed prototype has to be wired up on a
breadboard. The table below illustrates the requirements of the hardware design.

| Signal Generator Requirements |             |
|-------------------------------|-------------|
| Property                      | Value       |
| Supply Voltage                | +5V, -5V    |
| Amplitude                     | ±5Vpp       |
| Offset                        | ±5Vpp       |
| Budget                        | approx. 10€ |

The prototype has to be supplied with +5 V and -5 V. The +5 V can be used from
the USB, whereas an additional converter will be necessary for the -5 V supply.
The required components need to be precisely selected. In case the component
selection phase is done, a component list needs to be created and submitted to
the instructors.

## Comments

Use of analog POTIs or SPI POTIs possible.

![Schaltung](https://github.com/SeDi343/bel5_dms_project/blob/master/hardware/Schaltung.jpg)
