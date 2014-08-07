proj-tran
=========

Simple projective transformation tool using Gaussian elimination to obtain a homography matrix.
This tool partially uses OpenCV to load and show images,
but does not use the function (`cv::findHomography()`) of OpenCV which calculates homography matrices.
To solve a system of linear equations the Gaussian elimination have been used.

![screenshot](http://isw3.naist.jp/~kazuki-mi/img/proj-tran.png)

## Usage

    $ proj input_image [output_width] [output_height]

The `Input image` window will appears, then click four corners clockwise from the upper left to the lower left.

## Requirement
OpenCV 2.4

## Installation

    $ git clone git@github.com:kmiya/proj-tran.git
    $ cd proj-tran
    $ make
