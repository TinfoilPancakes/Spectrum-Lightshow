# A tool to extract spectrum data from system audio output.

## Please follow the README.md located inside the Visualiser/ directory.
### Special thanks to [Darby A Payne](https://github.com/dpayne) for his [open source cli-visualiser](https://github.com/dpayne/cli-visualizer)

### NOTES:
- The lightshow process uses GPIO pins 17 22 and 27 for PWM of LEDs. You can change these in main.cpp lines 32 - 34.
- Build lightshow using 'make' command.
- Start Visualizer before starting lightshow, or else shared memory will not be implemented correctly. I'll fix this later.
- Bugs are probably prevalent, this is just an initial Proof Of Concept.
- Enjoy :3