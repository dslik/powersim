Hardware and software for simulating various components of a modern power generation and distribution network.

# Build Journal

_Feel free to ask questions by opening an issue!_

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
