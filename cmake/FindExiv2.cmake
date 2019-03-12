find_package(PkgConfig)
pkg_check_modules(PC_EXIV2 QUIET exiv2)
set(EXIV2_DEFINITIONS ${PC_EXIV2_CFLAGS_OTHER})

find_path(EXIV2_INCLUDE_DIR NAMES exiv2/exif.hpp
    HINTS
    ${PC_EXIV2_INCLUDEDIR}
    ${PC_EXIV2_INCLUDE_DIRS}
    )

find_library(EXIV2_LIBRARY NAMES exiv2 libexiv2
    HINTS
    ${PC_EXIV2_LIBDIR}
    ${PC_EXIV2_LIBRARY_DIRS}
    )
