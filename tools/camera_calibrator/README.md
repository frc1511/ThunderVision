# Thunder Camera Calibrator

* [Description](#description)
* [Usage](#usage)

## Description
Using a number of images from a camera, Thunder Camera Calibrator calculates the camera's intrinsic matrix and distortion coefficients. These values can then be used by the Rolling Raspberry service to undistort images from the camera before vision processing.

## Usage
For input, the program requires images of a chessboard pattern taken by the camera from different angles. To produce the best results possible, 20 to 30 images is recommended.

To run the program, specify the paths to the images as arguments. You can also specify the number of squares the chess board using the `--columns` and `--rows` arguments. The `--square_size` argument specifies the size of each square on the chessboard in centimeters.

```bash
# 8x6 chessboard with 3cm squares
./ThunderCameraCalibrator --columns 8 --rows 6 --square_size 3 img1.jpg img2.jpg img3.jpg
```