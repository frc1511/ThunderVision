# Thunder Camera Calibrator

* [Description](#description)
* [Usage](#usage)

## Description
By utilizing a number of images of a chessboard pattern taken by the camera, Thunder Camera Calibrator can calculate the camera's intrinsic matrix and distortion coefficients. These values can then be used by the Rolling Raspberry service to undistort frames from the camera and calculate accurate distances to vision targets.

## Usage
For input, the program requires images of a chessboard pattern taken by the camera from different angles. To produce the best results possible, 20 to 30 images is recommended. A 7x6 chessboard pattern that can be printed on a standard 8 1/2" x 11" sheet of paper can be found [here](https://github.com/frc1511/RollingRaspberry/blob/main/tools/camera_calibrator/7x5_chessboard.pdf). When printed, the squares should be 3cm x 3cm. When preparing to take the calibration images, make sure that the chessboard is placed on a flat surface so that the camera can see the entire board. The chessboard should also be placed in a location where the lighting is consistent.

The program is written in Python 3 and utilizes OpenCV and numpy.

To calculate the camera's intrinsic matrix and distortion coefficients, run the following command:

```bash
python3 camera_calibrator.py ./path/to/input/directory
```

The program will use all '.jpg' images in the specified directory in the calibration process.

When the program is finished, it will output the camera's intrinsic matrix and distortion coefficients to the terminal. 
