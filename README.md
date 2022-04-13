# WSPR tools for Xiegu X6100

* Receive only (so far)
* Uploads spots to wsprnet.org when online

## Installation

1. Unpack x6100-local.tgz into the X6100 filesystem root.
2. Edit CALL and GRID toward the top of /usr/local/bin/spotter-loop.py.
3. (optional) Edit BANDS or HOPPING\_SCHEDULE to taste.

## Use

Run /usr/local/bin/spotter-loop.py on the X6100.

## Tips

* GNU Screen is included to allow the spotter loop to survive disconnecting SSH.
