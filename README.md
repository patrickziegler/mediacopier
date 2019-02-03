#  :camera: mcp_cc

### Features
* using **Exiv2** to read fields `Exif.Image.DateTime*`, `Exif.Photo.DateTime*` and `Exif.Photo.SubSecTime*`
* using **ffmpeg** to read `creation_time` in video files
* ~~using **libjpeg** for automatic rotation of JPEG files according to EXIF image orientation~~ **(WIP)** 
* support for **copying** and **moving** files
* support for **simulation mode** to test operation before execution
* provide **override flag** to force copying / moving of already existing files
* ~~provide **skip log flag**~~ to skip writing log files **(WIP)**

## Getting Started

### Prerequisites

This software depends on the following librarys:

* [Boost](https://www.boost.org/) (`libboost_filesystem`, `libboost_date_time`) [[Boost Software License](https://www.boost.org/users/license.html)]
* [ffmpeg](https://ffmpeg.org/) (`libavformat`, `libavutil`) [[LGPL](https://ffmpeg.org/legal.html)]
* [Exiv2](http://exiv2.org/) [[GPL](https://github.com/Exiv2/exiv2/blob/master/license.txt)]

Additionally, you will need:

* [OpenMP](https://www.openmp.org/)
* [CMake](https://cmake.org/) >= 3.9
* A recent C++ compiler, such as [Clan](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/)

### :hammer: Build and Install

The build process consists of four simple steps:

1. Define your preferred install location
```bash
export INSTALL_PREFIX=~/.local
```
2. Build and install the package
```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
make -j4 && make install
```
3. Make the package available with the following commands
```bash
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
├── include
│   └── mcp_cc
│       ├── *.hpp
├── lib
│   └── libmcp_cc.so
```

The binarys `mcp` and `mmv` provide the tools for copying or moving your files. They share some code in `libmcp_cc.so`.

The header files in `include/mcp_cc` are only needed for linking your own software to the code provided in `libmcp_cc.so`. So you probably won't need them.

###  :rocket: Usage

The executables `mcp` and `mmv` are both implementing the following signature:

```
Usage: [-h] [-o] [-s] [-f FORMAT] [-l LOGFILE] SOURCE DESTINATION
```

Where `SOURCE` and `DESTINATION` are mandatory parameters specifying the source and destination folder for copying or moving the files. Relative paths are allowed. The character `~` will be replaced with the users home directory. The folder `SOURCE` will be searched recursively. Files that are neither image nor video files are ignored. The output folder will be created if necessary.

Flag | Description
------------ | -------------
`-h` | print help text
`-o` | override existing files
`-s` | simulate the operation
`-f FORMAT` | renaming scheme
`-l LOGFILE` | name of log file

The parameter `-f FORMAT` specifies, how the filenames are derived from the information found in the input files. It could be something like `-f "%Y/%m/%d/IMG_%Y%m%d_%H%M%S_%f"`. You can see that this parameter also allows us to  specify the folder structure in the destination directory. The replacement characters to be used for this are described in the boost [documentation](https://www.boost.org/doc/libs/1_69_0/doc/html/date_time/date_time_io.html).

After all, calling `mcp ~/in ~/out` should result in some output similar to the following:

```
Input dir:      /home/user/in
Output dir:     /home/user/out

Searching for media files ... Done (24 files found)

Copying files [========================================>] 100.00 %

Operation finished successfully!
```

## Authors

*  Patrick Ziegler - *Initial work* - [Homepage](https://patrickziegler.github.io)

## License

This project is licensed under the GPL License - see the [LICENSE](LICENSE) file for details
