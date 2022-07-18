#!/bin/sh

chmod 600 $1/etc/monitrc
chmod 600 $1/etc/NetworkManager/system-connections/* || true
chmod 600 $1/etc/ssh/*_key || true
