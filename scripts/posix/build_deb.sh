#!/bin/bash
home=/home/$USER
progdir=${home}/build_qscreenshot
builddir=${progdir}/qScreenshot
scripts=${progdir}/scripts/posix
debfiles=${scripts}/debian
PREFIX=/usr
#
changelog_template='qscreenshot (VER-1) unstable; urgency=low

  * NEW_CHANGES

 -- USERNAME <USERNAME@HOSTNAME>  DDD, NN MMM YYYY HH:MM:SS +ZZZZ'
#
cd ${home}
echo "Downloading qscreenshot sources"
svn co http://qscreenshot.googlecode.com/svn/trunk/ ${progdir}
cd ${progdir}
svnver=`svnversion`
defines=${builddir}/src/defines.h
ver_s=`grep APP_VERSION $defines`
ver=`eval echo $(echo $ver_s | cut -d ' ' -f 3)`
data=`LANG=en date +'%a, %d %b %Y %T %z'`
user=`echo $USERNAME`
host=`echo $HOSTNAME`
builddeb=${progdir}/qscreenshot-${ver}
mkdir ${builddeb}
cp -r ${builddir}/* ${builddeb}
cp -r ${scripts}/* ${builddeb}
cd ${builddeb}
changefile=${builddeb}/debian/changelog
echo "${changelog_template}" > ${changefile}
sed "s/VER-1/$ver-1/" -i "${changefile}"
changes="New $svnver release"
sed "s/NEW_CHANGES/$changes/" -i "${changefile}"
sed "s/USERNAME/$user/" -i "${changefile}"
sed "s/<USERNAME@HOSTNAME>/<$user@$host>/" -i "${changefile}"
sed "s/DDD, NN MMM YYYY HH:MM:SS +ZZZZ/${data}/" -i "${changefile}"
./configure --prefix=${PREFIX}
dpkg-buildpackage -rfakeroot
sudo dpkg -i ../qscreenshot_${ver}*.deb
