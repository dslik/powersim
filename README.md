Hardware and software for simulating various components of a modern power generation and distribution network.

# Build Journal

_Feel free to ask questions by opening an issue!_

## 2023-12-21

The long screws came in, and I was able to finish up the second prototype of the annunciator panel. I switched over to using longer rectangular openings instead of square ones, which allow the use of larger text that is more visible from a distance.

![image](https://github.com/dslik/powersim/assets/5757591/a9fd9343-61a8-4f9d-b508-430c4b3866f5)

Here is an illustration of how the spacer PCBs are assembled:

![image](https://github.com/dslik/powersim/assets/5757591/d12aef20-455c-44ed-96ba-56b48758feaf)

There still is a slight darker band down the middle of each illuminated rectangle, so I would either need to use a few more spacer PCBs, or update the LED PCB to add more LEDs. I would also like to switch over to using RGBW LEDs instead of RGB LEDs, but I would need to find a good part for that that wouldn't significantly increase the BoM cost.

Next steps are to make some final tweaks to the boards, and finish up the software.

## 2023-10-09

Now that I'm getting to the point where I'm implementing more complex system topologies, I am finding that I need a lightweight specification language that acts as an equivalent to the IEC 61850 SCL System Specification Description. There's no way that I would be able to get away with parsing multi-MB XML on my little pico microcontroller, but that certainly can happen on a coordination server that translates the SCL into a simpler configuration language that can then be pushed down to each edge device via SNON.

## 2023-10-08

Here is the first fully assembled front panel:

![image](https://github.com/dslik/powersim/assets/5757591/2cc6692f-43ea-4c92-9a2b-d384433c7197)

All of the front panel modules are up and running, with the next step being finalizing and coding up the remaining HMI screens for the generator excitation, generator protection and transformer protection subsystems.

## 2023-10-02

The updated rack front panel for the generator synchronize indicator/switch came in, and everything fits. Looks pretty nice in the rack:

![image](https://github.com/dslik/powersim/assets/5757591/9cb9a686-2768-4d84-b953-742f03ab40b8)

On the software front, all of the sensors for the front panel LEDs are implemented and tested, and the LCD update routines are now far faster, and only update when a value has changed. Fixed a bunch of memory leaks and some FIFO overflows that would occasionally happen due to a priority inversion between ISRs. Good progress all around.

Next steps are to test out and implement the rest of the boards for the generator front panel, and design the remainder of the front panel screens.

## 2023-09-24

Everything is now being rendered programatically.

Adding the B612 typeface was challenging, since despite it being fixed-width, the width of each character was not a natural number when measured in pixels, and some characters extended into the bounding box of other characters. I was able to adjust things, but there may still be some lingering kerning issues.

![image](https://github.com/dslik/powersim/assets/5757591/0793864d-49f8-45bf-8bbe-17a069dac962)

Next step is to connect all the values up to SNON sensors, and update the display when sensor values are changed.

## 2023-09-22

The next revision of the annunciator PCBs have gone off for manufacturing. While I'm waiting for those to be delivered, I've been working on the various UI screens for the front panels. I'm basing the UI on the work done on effective console operator HMI design by the ASM Consortium (where ASM stands for Abnormal Situation Management). This is a great resource if you're looking for study-driven research into these types of interfaces, and there are quite a few initially counter-intuitive things to learn from their findings.

Some references for this design language include:

* Effective Console Operator HMI Design, by the ASM Consortium. ISBN 978-1440431647
* The High Performance HMI Handbook, by Hollifield, Bill et al. ISBN 978-0-9778969-1-2

Here are several mockups for some of the screens:

Busbar voltage and current:

![image](https://github.com/dslik/powersim/assets/5757591/1bd84715-c0c9-4f59-ae7b-addbd6195856) ![image](https://github.com/dslik/powersim/assets/5757591/d43b8150-dedc-45ba-99d3-69f829ededdf)

This shows current bus voltage, normal voltage range, and alarm indications.

Disconnector voltage and current:

![image](https://github.com/dslik/powersim/assets/5757591/6509dfd3-c330-4180-ac09-c29f5b39e826)

Here's what it looks like on an actual LCD:

![image](https://github.com/dslik/powersim/assets/5757591/70d291aa-cf89-4099-94d4-4e3d373cca7d)

Next steps are to create a library that renders these screens based on SNON sensor values, including labels, PVs, setpoints, alarms, and alarm thresholds.

## 2023-09-14

Lots of bits and pieces coming together over the last couple of weeks.

Here's the current state of the simulator:

![image](https://github.com/dslik/powersim/assets/5757591/8e83007b-a742-45e0-a90e-a609191e5ef0)

Some changes since the last update:

* Basic time infrastructure is up and running. 
* Transformer protection relay is now racked up on the far left.
* New infrastructure server is now racked up on the far right.
* Network infrastructure is racked
* PDUs for protective relaying and support equipment is racked.

Still tons of work to do!

The annunciator PCBs came in, and as I suspected, the front panel is far too yellow. So rev 2 will use a frosted plexiglass layer as a diffusor.

Here's what a prototype looks like using a dark-tinted piece of plexiglass. Not perfect yet, but already looking really good.

![image](https://github.com/dslik/powersim/assets/5757591/f4a669b0-72e5-4089-bce4-fa2e29d45c2a)

Work on the software is also underway, with configurable foreground and background colours and blink patterns specifiable via SNON in accordance to VITA 40.

## 2023-09-04

Continuing to work on the panel end controller (1840A). Here's one displaying text on both LCDs, and driving the front panel LEDs. 

![image](https://github.com/dslik/powersim/assets/5757591/323b020a-1462-441e-bc31-fbd78668d20f)

The text is being rendered using the on-board font IC.

This module is being powered by a SEL-3610, which also acts as a serial gateway to allow SNON data exchange via TCP/IP.

![image](https://github.com/dslik/powersim/assets/5757591/55d5eb0f-c5f3-43cf-af5e-aea9adef7ac2)

Next steps for this board are all software, including defining what information should be displayed on each screen, what sensor data should be used, and how it should be rendered.

## 2023-08-22

The design for the annunciator panels at the top of the racks to show overall system status and active alarms is now ready for production:

Front panel PCB:

![image](https://github.com/dslik/powersim/assets/5757591/668561f4-d959-44cf-bca3-b16e6dda0a6d)

The yellow colour of the PCB substrate is somewhat of a problem, but illuminating them with a white colour with the yellow subtracted will make it be far less visible. What I am more concerned about is the manufacturer's watermark that is embedded into the FR4.

LED PCB:

![image](https://github.com/dslik/powersim/assets/5757591/0c31afb7-780b-46b9-bf65-76e7f4b9b3ae)

I'm starting out with nine RGB LEDs per indicator. Hopefully this will provide even lighting, and I can use more power than I can with other displays, since there are fewer LEDs. (up to 60/255 vs. a limit of 20/255 per LED colour channel).

If this doesn't work out, I will have to move to 16 LEDs per indicator, which will be a significant increase in cost and complexity.

Next steps is to send these out for manufacturing in the next PCB batch.

I was also able to get a SEL-487E Transformer Protection Relay for the main generator step-up transformer. This unit is pretty large, but I should be able to squeeze it in with just 1U to spare.

## 2023-08-15

The next revision of the PCBs have arrived

Panel end controller PCB:

![image](https://github.com/dslik/powersim/assets/5757591/8c1fbac7-dbb6-4ea1-ab5b-420603c39a34)

Panel centre controller PCB:

![image](https://github.com/dslik/powersim/assets/5757591/b12e94ab-f0db-4377-bd29-69e822a23317)

Here is the new end controller PCB all soldered up and attached to a front panel, ready for testing:

![image](https://github.com/dslik/powersim/assets/5757591/73eeef43-beab-40d7-a30b-3cd2e281e661)

Next step is to power this up, test the fixes, and see if the font IC is working.

## 2023-08-12

Been working on infrastructure (and software) while waiting for the next revision of the PCBs to arrive.

Here's the current state of the simulator:

![image](https://github.com/dslik/powersim/assets/5757591/aa8759cd-ccf7-462d-91af-00e99b720ef6)

Rack #1 on the far left is the generator section. I'm having to have a 1/4 DIN panel fabricated, since no one seems to make these available for sale.

Rack #2 represents the transmission line from the generation station to the substation.

Rack #3 represents a breaker-and-a-half substation.

Rack #4 represents more transmission lines from the substation.

Rack #5 on the far right will represent a large inductive load. Still need to create the panels for this one.

The four small racks on the top are for annunciator panels and IRIG/IEEE 1588/PTPv2 timing that tie everything together. 

## 2023-07-20

Both of the panel controller boards have been updated to fix the errata discovered with the previous revisions.

They'll go out for manufacturing in the next PCB batch.

Starting to get the protective relaying set up. I need to run some GPS cables for the PTP/IEEE 1588 master clock before I can start bringing everything online.

![image](https://github.com/dslik/powersim/assets/5757591/801d1c65-0a91-4d91-a07e-694834601baf)

## 2023-07-17

The panel controller boards are starting to come together. I have all of the LEDs and LCDs now responding:

![image](https://github.com/dslik/powersim/assets/5757591/8db1bc7a-96b4-4451-ba80-d3ea0a24ed4e)

Found some minor and easily fixed issues with the right-most buttons, but other than that, everything looks ready for a V1 release. All that remains is testing the serial I/O and that it can be successfully powered from a SEL-3610.

These are using the beautiful but unfortunately quite expensive Omron B3W-9000 buttons.
