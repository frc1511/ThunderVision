# Thunder Image Capture

* [Description](#description)
* [Usage](#usage)

## Description

Thunder Image Capture is a simple program that can be used to quickly capture images from a camera and save them to a directory.

## Usage

The program is written in Python 3 and utilizes OpenCV.

To start the program, run the following command:

```bash
python3 image_capture.py 0 ./path/to/output/directory
```
The first argument is the video index. This is the number that is used to identify which camera to use.

The second argument is the path to the directory where the images will be saved.

When the program is running, press the 's' key to capture an image. Images will be saved as #.jpg, where # is the number of images that have been captured. Note that images will be overwritten without warning if they already exist in the specified directory. Press the 'q' key to quit the program.
