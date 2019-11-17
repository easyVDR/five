#!/bin/sh

cdrdao disk-info --driver generic-mmc --device ATA:1,0,0
play -v 5 /VDR/common/ready.wav
