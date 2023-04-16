#!/usr/bin/python3

import cv2 as cv
import sys

try:
  img_dir = str(sys.argv[1])
  fps = int(sys.argv[2])
  video_path = str(sys.argv[3])
except:
  print("Usage: python3 images_to_video.py <image directory> <fps> <video path>")
  exit()

video_writer = None

n = 0;
while True:
  frame = cv.imread(img_dir + "/" + str(n) + ".jpg")
  n += 1

  try:
    frame.any()
  except:
    break

  if not video_writer:
    frame_height = frame.shape[0]
    frame_width = frame.shape[1]
    size = (frame_width, frame_height)

    video_writer = cv.VideoWriter(video_path, cv.VideoWriter_fourcc(*'MJPG'), fps, size)

  video_writer.write(frame)

if video_writer:
  video_writer.release()
else:
  print("No images converted")