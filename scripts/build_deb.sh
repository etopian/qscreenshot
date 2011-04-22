#!/bin/bash
home=/home/$USER
progdir=${home}/build_qscreenshot
builddir=${progdir}/qScreenshot
builddeb=${progdir}/qscreenshot-0.2
scripts=${progdir}/scripts/posix
debfiles=${scripts}/debian
PREFIX=/usr
#
cd ${home}
echo "Downloading qscreenshot sources"
svn co http://qscreenshot.googlecode.com/svn/trunk/ ${progdir}
cd ${progdir}
mkdir ${builddeb}
cp -r ${builddir}/* ${builddeb}
cp -r ${scripts}/* ${builddeb}
cd ${builddeb}
./configure --prefix=${PREFIX}
dpkg-buildpackage -rfakeroot

