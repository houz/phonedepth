# phonedepth

This repository hosts some small utilities I wrote while looking into the way modern phones store depth data inside their JPEGs.

## Example

This are the color image and depth map from [a photo taken with a Samsung S9+](20200806_111654.jpg).

![color view](cv.jpg "color view")

![depth map](dm.jpg "depth map")

## Usage

The most useful tool is `extract_depthmap.c` which was tested to work with these phones:

- Apple iPhone X
- Apple iPhone XR
- Apple iPhone XS
- Huawei Mate 20
- Huawei Mate 20 Lite
- Huawei Mate 20 Pro
- Huawei P20 Pro
- Huawei P30 Pro
- Samsung A7
- Samsung A9
- Samsung S10
- Samsung S10e
- Samsung S9+

In order to use it you have to compile it:

    gcc -o extract_depthmap extract_depthmap.c

and then run it:

    ./extract_depthmap /path/to/file.jpg

It will create some files in the current directory:

- `cv.jpg` -- the color image
- `dm.pgm` or `dm.jpg` -- the depth map
- `extra.jpg` -- some iPhones store an additional mask. If this file is garbage you should just ignore it.

Feel free to contact me if you have a phone that stores depth data and want it supported.
