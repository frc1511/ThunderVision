#!/usr/bin/python3

import numpy as np
import cv2 as cv
import glob
import sys

SQUARE_SIZE = 30 # mm
X_CORNERS = 7
Y_CORNERS = 5

if len(sys.argv) != 2:
  print("Usage: python3 camera_calibrator.py <image directory>")
  exit()

# Directory with calibration images
img_dir = sys.argv[1]
img_paths = glob.glob(img_dir + "/*.jpg")

if not len(img_paths):
  print("No images found in directory: " + img_dir)
  exit()

# Termination criteria
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, SQUARE_SIZE, 0.001)

# Prepare object points
objp = np.zeros((X_CORNERS * Y_CORNERS, 3), np.float32)
objp[:, :2] = np.mgrid[0:X_CORNERS, 0:Y_CORNERS].T.reshape(-1, 2)

objpts = [] # 3d points in real world space
imgpts = [] # 2d points in image plane

h, w = (0, 0)

for path in img_paths:
  img = cv.imread(path)
  h, w = img.shape[:2]

  # Convert to grayscale
  gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

  # Find the chess board corners
  ret, corners = cv.findChessboardCorners(gray, (X_CORNERS, Y_CORNERS), None)

  # If found
  if ret:
    # Add object points.
    objpts.append(objp)

    # Refine the corners.
    ref_corners = cv.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)

    # Add image points.
    imgpts.append(ref_corners)

    # Draw and display the corners
    cv.drawChessboardCorners(img, (X_CORNERS, Y_CORNERS), ref_corners, ret)
    cv.imshow('img', img)
    cv.waitKey(1500)

cv.destroyAllWindows()

# Calibrate the camera
ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpts, imgpts, gray.shape[::-1], None, None)
new_mtx, roi = cv.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

print("Camera matrix:\n", new_mtx, "\n")
print("Distortion coefficients:\n", dist, "\n")
