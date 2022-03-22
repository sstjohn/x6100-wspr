# WSPR tools for Xiegu X6100

* Receive only (so far)
* No rig control
* Uploads spots to wsprnet.org

## Installation

1. Unpack x6100-local.tgz into the X6100 filesystem root.
2. Update /etc/profile to add /usr/local/bin to PATH.
3. Ensure /etc/resolv.conf has a nameserver.

## Use

1. Set a WSPR frequency on the X6100 UI.
2. Edit FREQUENCY, CALL and GRID toward the top of spotter-loop.py.
3. Run spotter-loop.py on the X6100.

## Tips

* GNU Screen is included to allow the spotter loop to survive disconnecting SSH.
