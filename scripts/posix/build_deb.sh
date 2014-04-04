#!/bin/bash
home=${HOME:-/home/$USER}
reponame=qscreenshot
progdir=${home}/build/${reponame}
builddir=${progdir}/qScreenshot
scripts=${progdir}/scripts/posix
PREFIX=/usr
echo "checking for installed packages needed to build qscreenshot package"
sudo apt-get install build-essential fakeroot dpkg-dev autotools-dev cdbs
#
changelog_template='qscreenshot (VER-1) unstable; urgency=low
'
#
changelog_endline='
 -- USERNAME <WHOCHANGE>  DDD, NN MMM YYYY HH:MM:SS +ZZZZ'
#
package_sh='#!/bin/bash
set -e

debuild -us -uc
debuild -S -us -uc
su -c pbuilder build ../qscreenshot-X.X.dsc'
#
rules='#!/usr/bin/make -f
# -*- makefile -*-
# Sample debian/rules that uses debhelper.
# This file was originally written by Joey Hess and Craig Small.
# As a special exception, when this file is copied by dh-make into a
# dh-make output file, you may use that output file without restriction.
# This special exception was added by Craig Small in version 0.37 of dh-make.

# Uncomment this to turn on verbose mode.
#export DH_VERBOSE=1
config.status: configure
	dh_testdir

	# Add here commands to configure the package.
	./configure --host=$(DEB_HOST_GNU_TYPE)
	--build=$(DEB_BUILD_GNU_TYPE)
	--prefix=/usr 
  
include /usr/share/cdbs/1/rules/debhelper.mk
include /usr/share/cdbs/1/class/qmake.mk

# Add here any variable or target overrides you need.
QMAKE=qmake-qt4
CFLAGS=-O3
CXXFLAGS=-O3'
#
control='Source: qscreenshot
Section: x11
Priority: extra
Maintainer: Vitaly Tonkacheyev <thetvg@gmail.com>
Build-Depends: debhelper (>= 7), cdbs, libqt4-dev
Standards-Version: 3.8.3
Homepage: http://code.google.com/p/qscreenshot/

Package: qscreenshot
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}, libc6 (>=2.7-1), libgcc1 (>=1:4.1.1), libqtcore4 (>=4.6), libqtgui4 (>=4.6), libstdc++6 (>=4.1.1), libx11-6, zlib1g (>=1:1.1.4)
Description: Create and modify screenshots
 Simple tool written on Qt to create and modify screenshots, with ability to send image to popular image share servers.'
#
dirs='usr/bin
usr/lib
usr/share/icons/hicolor/16x16/apps
usr/share/icons/hicolor/32x32/apps
usr/share/icons/hicolor/48x48/apps
usr/share/qscreenshot
usr/share/qscreenshot/translations
usr/share/applications'
#
compat='7'
#
copyright='This work was packaged for Debian by:

    USER <USER@MAIL> on DOW, DD MMM YYYY HH:MM:SS +ZZZZ

It was downloaded from:

    http://code.google.com/p/qscreenshot/

Upstream Author(s):

     Dealer_WeARE <wadealer@gmail.com>

Copyright:

    <Copyright (C) YYYY Dealer_WeARE>

License:

### SELECT: ###
    This package is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
### OR ###
   This package is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.
##########

    This package is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this package; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

On Debian systems, the complete text of the GNU General
Public License version 2 can be found in "/usr/share/common-licenses/GPL-2".

The Debian packaging is:

    Copyright (C) YYYY USER <USERNAME@MAIL>

you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

# Please also look if there are files or directories which have a
# different copyright/license attached and list them here.'
#
if [ ! -d "${home}/build" ]; then
  mkdir -p ${home}/build
fi
cd ${home}/build
if [ ! -d "${progdir}" ]; then
  mkdir -p ${progdir}
  echo "Downloading qscreenshot sources" 
  git clone https://code.google.com/p/qscreenshot/ ${progdir}
else
  cd ${progdir}
  git reset --hard
  git pull
fi
cd ${progdir}

defines=${builddir}/src/defines.h
ver_s=$(grep APP_VERSION $defines)
ver=$(eval echo $(echo $ver_s | cut -d ' ' -f 3))
data=$(LANG=en date +'%a, %d %b %Y %T %z')
year=$(date +'%Y')
user=$(echo $USERNAME)
host=$(echo $HOSTNAME)
builddeb=${progdir}/qscreenshot-${ver}
if [ -d ${builddeb} ]; then
  rm -r -f ${builddeb}
fi
mkdir ${builddeb}
cp -r ${builddir}/* ${builddeb}
mkdir ${builddeb}/debian
#control files
changefile=${builddeb}/debian/changelog
rulesfile=${builddeb}/debian/rules
controlfile=${builddeb}/debian/control
dirsfile=${builddeb}/debian/dirs
compatfile=${builddeb}/debian/compat
copyrightfile=${builddeb}/debian/copyright
package_sh_file=${builddeb}/package.sh
#parsing files
echo "${changelog_template}" > ${changefile}
echo "${package_sh}" > ${package_sh_file}
echo "${rules}" > ${rulesfile}
echo "${control}" > ${controlfile}
echo "${dirs}" > ${dirsfile}
echo "${compat}" > ${compatfile}
echo "${copyright}" > ${copyrightfile}
#modifying changelog
sed "s/VER-1/$ver-1/" -i "${changefile}"
cd ${progdir}
startlog=$(cat ${builddeb}/Changelog.txt | sed -n '/[0-9]\{4\}$/{n;p;q;}')
endlog=$(cat ${builddeb}/Changelog.txt | sed -n '/^$/{g;1!p;q;};h')
middlelog=$(cat ${builddeb}/Changelog.txt | sed -n '/'"${startlog}"'/,/'"${endlog}"'/p')
changes=$(echo "$middlelog" | sed -n "s/\s*-\s*/  \* /p")
echo "$changes" >> ${changefile}
echo "${changelog_endline}" >> ${changefile}
sed 's/WHOCHANGE/thetvg@gmail\.com/' -i "${changefile}"

sed 's/USERNAME/'"$user"'/' -i "${changefile}"

sed "s/DDD, NN MMM YYYY HH:MM:SS +ZZZZ/${data}/" -i "${changefile}"
#
sed 's/X.X/'"$ver"'/' -i "${package_sh_file}"
#
sed 's/USER/'"$user"'/' -i "${copyrightfile}"
sed "s/USER@MAIL/$user@$host/" -i "${copyrightfile}"
sed "s/DOW, DD MMM YYYY HH:MM:SS +ZZZZ/${data}/" -i "${copyrightfile}"
sed "s/YYYY/$year/" -i "${copyrightfile}"
sed "s/USERNAME/$user/" -i "${copyrightfile}"
sed "s/MAIL/$host/" -i "${copyrightfile}"
cd ${builddeb}
./configure --prefix=${PREFIX}
dpkg-buildpackage -rfakeroot
sudo dpkg -i ../qscreenshot_${ver}*.deb
