#!/bin/bash

sync
mount /dev/mmcblk0p1 mount
sync
cp kernel.img mount
sync
umount /dev/mmcblk0p1
sync
umount /dev/mmcblk0p2
sync
