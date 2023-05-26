NAME = Programmer
CHIP = esp32
BOARD = nodemcu-32s
BUILD_DIR = build.$(BOARD)
SKETCH = $(NAME).ino
USE_CCACHE = 1
VERBOSE = 1
include ~/makeEspArduino/makeEspArduino.mk
