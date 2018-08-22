#!/bin/bash
ffmpeg -r 60 -f image2 -s 900x900 -i frame%d.png -vcodec libx264 -crf 25  -pix_fmt yuv420p test.mp4
