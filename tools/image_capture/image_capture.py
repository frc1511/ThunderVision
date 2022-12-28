#!/usr/bin/python3

import cv2 as cv
import sys

try:
  video_id = int(sys.argv[1])
  img_dir = sys.argv[2]
except:
  print("Usage: python3 camera_capture.py <video id> <image directory>")
  exit()

# Open the camera
cap = cv.VideoCapture(video_id)

if not cap.isOpened():
  print("Could not open camera: {}".format(video_id))
  exit()

n = 0
while True:
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Display the resulting frame
    cv.imshow(str(video_id), frame)

    # Get key press
    k = cv.waitKey(1) & 0xFF

    # If the user presses the 's' key, save the image
    if k == ord('s'):
      cv.imwrite("{}/{}.jpg".format(img_dir, n), frame)
      n += 1

    # If the user presses the 'q' key, quit
    if k == ord('q'):
      break

cv.destroyAllWindows()
