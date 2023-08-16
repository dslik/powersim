Hardware and software for simulating various components of a modern power generation and distribution network.

# Build Journal

_Feel free to ask questions by opening an issue!_

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
