![VTK - The Visualization Toolkit](vtkBanner.gif)

Introduction
============

VTK is an open-source software system for image processing, 3D
graphics, volume rendering and visualization. VTK includes many
advanced algorithms (e.g., surface reconstruction, implicit modelling,
decimation) and rendering techniques (e.g., hardware-accelerated
volume rendering, LOD control).

VTK is used by academicians for teaching and research; by government
research institutions such as Los Alamos National Lab in the US or
CINECA in Italy; and by many commercial firms who use VTK to build or
extend products.

The origin of VTK is with the textbook "The Visualization Toolkit, an
Object-Oriented Approach to 3D Graphics" originally published by
Prentice Hall and now published by Kitware, Inc. (Third Edition ISBN
1-930934-07-6). VTK has grown (since its initial release in 1994) to a
world-wide user base in the commercial, academic, and research
communities.

Learning Resources
==================

* General information is available at the [VTK Homepage][].

* Community discussion takes place on the [VTK Discourse][] forum.

* Commercial [support][Kitware Support] and [training][Kitware Training]
  are available from [Kitware][].

* Doxygen-generated nightly reference documentation is
  available [online][Doxygen].

[VTK Homepage]: https://www.vtk.org/
[Doxygen]: https://www.vtk.org/doc/nightly/html
[VTK Discourse]: https://discourse.vtk.org/
[Kitware]: https://www.kitware.com/
[Kitware Support]: https://www.kitware.com/what-we-offer/#support
[Kitware Training]: https://www.kitware.com/what-we-offer/#training

Reporting Bugs
==============

If you have found a bug:

1. If you have a patch, please read the [CONTRIBUTING.md][] document.

2. Otherwise, please join the [VTK Discourse][] forum and ask
   about the expected and observed behaviors to determine if it is
   really a bug.

3. Finally, if the issue is not resolved by the above steps, open
   an entry in the [VTK Issue Tracker][].

[VTK Issue Tracker]: https://gitlab.kitware.com/vtk/vtk/issues

Requirements
============

In general VTK tries to be as portable as possible; the specific configurations below are known to work and tested.

VTK supports the following C++11 compilers:
1. Microsoft Visual Studio 2015 or newer
2. gcc 4.8.3 or newer
3. Clang 3.3 or newer
4. Apple Clang 5.0 (from Xcode 5.0) or newer
5. Intel 14.0 or newer

VTK supports the following operating systems:
1. Windows Vista or newer
2. Mac OS X 10.7 or newer
3. Linux (ex: Ubuntu 12.04 or newer, Debian 4 or newer)

Contributing
============

See [CONTRIBUTING.md][] for instructions to contribute.

[CONTRIBUTING.md]: CONTRIBUTING.md

License
=======

VTK is distributed under the OSI-approved BSD 3-clause License.
See [Copyright.txt][] for details.

[Copyright.txt]: Copyright.txt
