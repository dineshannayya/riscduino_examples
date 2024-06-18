Debug connectivity program caravel flash

CARAVEL        RISCDUINO-ISP
mprj_io[1]     1 - ISP_MISO
X              2 - 3.3V
mprj_io[4]     3 - ISP_SCK
mprj_io[2]     4 - ISP_MOSI
mprj_io[3]     5 - ISP_SSN
GND            6 - GND


### To fix pyftdi   ##############################
     Traceback (most recent call last):
       File "/home/dinesha/workarea/opencore/git/riscduino_examples/bootloader/CI2306Q/xtal_29.4912mhz/dcache_enb1/../common/util/caravel_hkflash.py", line 3, in <module>
         from pyftdi.ftdi import Ftdi
     ModuleNotFoundError: No module named 'pyftdi'
     make: *** [Makefile:38: flash] Error 1


pip3 install pyftdi
