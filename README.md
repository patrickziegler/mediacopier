#  :camera: MediaCopier

[![Build Status](https://api.travis-ci.com/patrickziegler/MediaCopier.svg?branch=master)](https://travis-ci.com/patrickziegler/MediaCopier)

### Features
This software searches for **tagged media files** in a given directory and copies or moves those files to a given destination while renaming them according to the specified format. The original creation date is used to **generate a folder structure** and unique filenames. Raw image files and videos are supported as well.

This library supports **lossless auto-rotation of JPEG files with known orientation** (on the fly) when image dimension are appropriate (divisible by 16). If the orientation is unkown or the dimensions are inconvenient they will be copied as they are.

The execution stage uses **parallelization via OpenMP** for better performance.

## Getting Started

### Prerequisites

This software needs on the following libraries at runtime:

* [Boost](https://www.boost.org/) (`filesystem`, `date_time`)
* [ffmpeg](https://ffmpeg.org/) (`libavformat`, `libavutil`)
* [libjpeg-turbo](https://www.libjpeg-turbo.org/)
* [Exiv2](http://exiv2.org/)

Additionally, you will need the following libraries for building and testing:

* [Boost](https://www.boost.org/) (`test`)
* [OpenMP](https://www.openmp.org/)
* [CMake](https://cmake.org/) >= 3.9
* A recent C++ compiler, such as [Clang](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/)

### :hammer: Build and Install

1. Clone this repository and create a build directory
```bash
git clone --recursive git@github.com:patrickziegler/MediaCopier.git
cd MediaCopier && mkdir build && cd build
```

2. Build and install to `/usr/local` *(default)*
```bash
cmake ..
make && make install
```

3. Build and install to another destination *(alternative)*
```bash
export INSTALL_PREFIX=~/.local

cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
make && make install

export $PATH=$PATH:$INSTALL_PREFIX/bin
export $LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PREFIX/lib
```

4. Build a RPM package to be used with your favorite package manager *(optional)*
```bash
make package
```

### Generated Files

After the installation is finished, you should find a file structure as seen below:

```
├── bin
│   ├── mcp
│   └── mmv
├── lib
│   └── libmediacopier.so
```

The binarys `mcp` and `mmv` provide the tools for copying or moving your files. They share some code in `libmediacopier.so`.

###  :rocket: Usage

The executables `mcp` and `mmv` are both implementing the following signature:

```
Usage: [-h] [-o] [-s] [-f FORMAT] [-l LOGFILE] SOURCE DESTINATION
```

Where `SOURCE` and `DESTINATION` are mandatory parameters specifying the source and destination folder for copying or moving the files. Relative paths are allowed. The character `~` will be replaced with the users home directory. The folder `SOURCE` will be searched recursively. Files without EXIF or video metadata are ignored. The output folder will be created if necessary.

Flag | Description
------------ | -------------
`-h` | print help text
`-o` | override existing files
`-s` | simulate the operation
`-f FORMAT` | format of new filenames
`-l LOGFILE` | write log to LOGFILE

The parameter `-f FORMAT` specifies how the filenames are derived from the information found in the input files. It could be something like `-f "%Y/%m/%d/IMG_%Y%m%d_%H%M%S_%f"`.

The replacement characters to be used for this are described in the boost [documentation](https://www.boost.org/doc/libs/1_69_0/doc/html/date_time/date_time_io.html). Please note that this parameter also allows to  specify the folder structure in the destination directory.

## Authors

*  Patrick Ziegler

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
