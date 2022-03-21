# WISPR tools for Xiegu X6100

* Receive only (so far)
* No rigctrl
* Uploads spots to wsprnet.org

## Installation

1. Unpack x6100-local.tgz in filesystem root.
2. Update /etc/profile to add /usr/local/bin to PATH.

## Use

1. Set a WSPR frequency on the X6100.
2. Edit FREQUENCY, CALL and GRID toward the top of spotter-loop.py.
3. Run spotter-loop.py.

## Tips

GNU Screen is included to allow the spotter loop to survive disconnecting SSH.
