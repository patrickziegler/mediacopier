#!/bin/bash
#
# Help text (accessible via -h):
#>
#> Usage: ./generate_test_data.sh OUTPUT_DIRECTORY
#>
#> The files 'lena512color.tiff' and 'roundhay_garden_scene.mp4' are
#> assumed to exist in the same directory as this script resides in.
#>
#> Dependencies: ImageMagick libjpeg-turbo exiftool ffmpeg-4
#>
#> Options:
#>    -h          show this help message and exit
#>
#
# [1] https://www.daveperrett.com/articles/2012/07/28/exif-orientation-handling-is-a-ghetto/
# [2] https://stackoverflow.com/questions/14612809/jpegtran-exe-not-correctly-rotating-image

while getopts "h" OPT; do
    case $OPT in
        h)
            cat `readlink -f "$0"` | awk '$0 ~ /^#>/ {print substr($0, 4, length($0)-1)}'
            exit;;
        \?)
            exit;;
    esac
done

shift $(($OPTIND - 1))

set -x

SCRIPT=`readlink -f "$0"`
SCRIPT_PATH=`dirname "${SCRIPT}"`

# --- Variables

DATA_PATH=$1

IMG_ORIGINAL="${SCRIPT_PATH}"/lena512color.tiff
VID_ORIGINAL="${SCRIPT_PATH}"/roundhay_garden_scene.mp4

IMG_64_ROT0="${DATA_PATH}"/lena64_rot0.jpg
IMG_64_ROT90="${DATA_PATH}"/lena64_rot90.jpg
IMG_64_ROT180="${DATA_PATH}"/lena64_rot180.jpg
IMG_64_ROT270="${DATA_PATH}"/lena64_rot270.jpg

IMG_64_ROT180_COPY="${DATA_PATH}"/lena64_rot180_copy.jpg
IMG_64_ROT270_COPY="${DATA_PATH}"/lena64_rot270_copy.png

IMG_50_ROT0="${DATA_PATH}"/lena50_rot0.jpg
IMG_50_ROT90="${DATA_PATH}"/lena50_rot90.jpg

VID_MOV="${DATA_PATH}"/roundhay_garden_scene.mov
VID_MKV="${DATA_PATH}"/roundhay_garden_scene.webm

# --- Clean-Up

rm -drf "${DATA_PATH}"
mkdir -p "${DATA_PATH}"

# --- Create images with orientations as in [1]

convert "${IMG_ORIGINAL}" -resize 64x64 -quality 25 -colorspace GRAY "${IMG_64_ROT0}"

jpegtran -optimize -rotate 90 "${IMG_64_ROT0}" > "${IMG_64_ROT90}"
jpegtran -optimize -rotate 180 "${IMG_64_ROT0}" > "${IMG_64_ROT180}"
jpegtran -optimize -rotate 270 "${IMG_64_ROT0}" > "${IMG_64_ROT270}"

exiftool -overwrite_original -DateTimeOriginal="2019-02-05 12:10:32" "${IMG_64_ROT0}"
exiftool -overwrite_original -n -Orientation=1 "${IMG_64_ROT0}" -SubSecTimeOriginal=123456

exiftool -overwrite_original -DateTimeOriginal="2019-02-05 12:11:32" "${IMG_64_ROT90}"
exiftool -overwrite_original -n -Orientation=8 "${IMG_64_ROT90}" -SubSecTimeOriginal=12345

exiftool -overwrite_original -DateTimeOriginal="2019-02-05 12:12:32" "${IMG_64_ROT180}"
exiftool -overwrite_original -n -Orientation=3 "${IMG_64_ROT180}" -SubSecTimeOriginal=1234

exiftool -overwrite_original -DateTimeOriginal="2019-02-05 12:13:32" "${IMG_64_ROT270}"
exiftool -overwrite_original -n -Orientation=6 "${IMG_64_ROT270}" -SubSecTimeOriginal=123

cp -u "${IMG_64_ROT180}" "${IMG_64_ROT180_COPY}"

convert "${IMG_64_ROT270}" "${IMG_64_ROT270_COPY}"

# --- Create image with corrupted rotation as described in [2]

jpegtran -optimize -rotate 90 "${IMG_64_ROT0}" > "${IMG_50_ROT90}"
convert "${IMG_50_ROT90}" -resize 50x50 -quality 25 "${IMG_50_ROT90}"

jpegtran -optimize -rotate 270 "${IMG_50_ROT90}" > "${IMG_50_ROT0}" # IMG_50_ROT0 will be corrupted!

exiftool -overwrite_original -DateTimeOriginal="2018-05-05 06:10:32" "${IMG_50_ROT0}"
exiftool -overwrite_original -n -Orientation=1 "${IMG_50_ROT0}" -SubSecTimeOriginal=12

exiftool -overwrite_original -DateTimeOriginal="2018-05-05 06:11:32" "${IMG_50_ROT90}"
exiftool -overwrite_original -n -Orientation=8 "${IMG_50_ROT90}"

# --- Create video files in various formats

cp "${VID_ORIGINAL}" "${DATA_PATH}"
ffmpeg -i "${VID_ORIGINAL}" -map_metadata 0 -metadata creation_time="2017-10-13 09:29:46" -codec copy "${VID_MOV}"
ffmpeg -i "${VID_ORIGINAL}" -map_metadata 0 -metadata creation_time="2017-10-13 09:30:46" -codec vp9 -crf 63 "${VID_MKV}"