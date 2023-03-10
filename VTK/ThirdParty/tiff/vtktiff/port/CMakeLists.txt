# CMake build for libtiff
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

set(port_HEADERS libport.h)
set(port_SOURCES dummy.c)
set(port_optional_SOURCES
  getopt.c
  lfind.c
  strcasecmp.c
  strtol.c
  strtoll.c
  strtoul.c
  strtoull.c)

set(port_USED_FILES ${port_SOURCES} ${port_HEADERS})

# XXX(kitware): Some are only required for tools, so skip them.
if(NOT HAVE_GETOPT AND FALSE)
  list(APPEND port_USED_FILES getopt.c)
endif()
if(NOT HAVE_LFIND AND FALSE)
  list(APPEND port_USED_FILES lfind.c)
endif ()
if(MSVC AND NOT HAVE_SNPRINTF)
  list(APPEND port_USED_FILES snprintf.c)
endif()
if(NOT HAVE_STRCASECMP AND FALSE)
  list(APPEND port_USED_FILES strcasecmp.c)
endif()
if(NOT HAVE_STRTOL AND FALSE)
  list(APPEND port_USED_FILES strtol.c)
endif()
if(NOT HAVE_STRTOLL AND FALSE)
  list(APPEND port_USED_FILES strtoll.c)
endif()
if(NOT HAVE_STRTOUL AND FALSE)
  list(APPEND port_USED_FILES strtoul.c)
endif()
if(NOT HAVE_STRTOULL AND FALSE)
  list(APPEND port_USED_FILES strtoull.c)
endif()

if (FALSE) # XXX(kitware): Not building tools for VTK; skip the library.
add_library(port STATIC ${port_USED_FILES})
endif ()

foreach(file ${port_USED_FILES})
  list(APPEND tiff_port_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()
set(tiff_port_SOURCES ${tiff_port_SOURCES} PARENT_SCOPE)
