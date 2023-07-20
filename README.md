Hardware and software for simulating various components of a modern power generation and distribution network.

# Build Journal

_Feel free to ask questions by opening an issue!_

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
