### Spaghettis: Yet Another Fork of Pure Data
Spaghettis is a fork of [Pure Data](https://www.puredata.info) (pd), the visual programming environment created by Miller Puckette. Spaghettis is an attempt to address some of long-standing usability issues with Pure Data by offering an improved user interface, implementing a portable framework for graphics projects, and providing many of the popular pd external objects natively.

#### Platforms
Currently, Spaghettis has been tested on Ubuntu 20.04 LTS, MacOS 10.15, Manjaro and Raspian Buster.

#### Dependencies
On Debian-based distros, the following packages are required:

* tk8.6
* tcl8.6
* libasound2-dev
* libjack-jackd2-dev

On Arch-based distros, the following packages are required:

* tk
* jack

#### Build
1. Open a terminal and run the `Spaghettis/build.sh` script.
2. Nothing will be installed on your system.
3. On GNU/Linux, execute `Spaghettis/build/bin/spaghettis`.

#### Credits
* [Miller Puckette & others](http://msp.ucsd.edu/Software/pd-README.txt)
* [William Andrew Burnson](https://github.com/burnson)
* [Lewis Van Winkle](https://github.com/codeplea/tinyexpr)
* [T. Nishimura & M. Matsumoto](http://www.math.sci.hiroshima-u.ac.jp/~m-mat)
* [Takuya Ooura](http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html)

##### Licensed under the [BSD-3-Clause license](https://opensource.org/licenses/BSD-3-Clause).
