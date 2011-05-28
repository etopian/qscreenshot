#!/bin/bash
progname=qscreenshot
homedir=$HOME
rpmbuild_dir=${homedir}/build_${progname}
if [ -d "${rpmbuild_dir}" ]
then
rm -r -f ${rpmbuild_dir}
fi
mkdir ${rpmbuild_dir}
svndir=${rpmbuild_dir}/${progname}
srcpath=/usr/src/packages/SOURCES
cd $homedir
svn checkout http://qscreenshot.googlecode.com/svn/trunk/qScreenshot ${homedir}/${progname}
if [ -d "${svndir}" ]
then
rm -r -f ${svndir}
fi
mkdir ${svndir}
cp -r -f ${homedir}/${progname}/* ${svndir}/
defines=${svndir}/src/defines.h
ver_s=`grep APP_VERSION $defines`
ver=`eval echo $(echo $ver_s | cut -d ' ' -f 3)`
package_name=${progname}-${ver}.tar.gz
builddir=${rpmbuild_dir}/${progname}-${ver}
if [ -d "${builddir}" ]
then
rm -r -f ${builddir}
fi
mkdir ${builddir}
cp -r -f ${svndir}/* ${builddir}/
cd ${rpmbuild_dir}
tar -pczf ${package_name} ${progname}-${ver}
cat <<END >/usr/src/packages/SPECS/${progname}.spec
Summary: Simple Screenshot Maker
Name: $progname
Version: $ver
Release: 1
License: GPL-2
Group: Productivity/Graphics/Other
URL: http://code.google.com/p/qscreenshot/
Source0: $package_name
BuildRequires: gcc-c++, zlib-devel
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple screenshot maker tool

%prep
%setup

%build
./configure --prefix="%{_prefix}" --bindir="%{_bindir}" --qtdir=$QTDIR
%{__make} %{?_smp_mflags} 

%install
[ "%{buildroot}" != "/"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT="%{buildroot}"

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/icons/hicolor/16x16/apps
mkdir -p %{buildroot}/usr/share/icons/hicolor/22x22/apps
mkdir -p %{buildroot}/usr/share/icons/hicolor/24x24/apps
mkdir -p %{buildroot}/usr/share/icons/hicolor/32x32/apps
mkdir -p %{buildroot}/usr/share/icons/hicolor/48x48/apps

%clean
[ "%{buildroot}" != "/" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/qScreenshot
%{_datadir}/applications/
%{_datadir}/icons/hicolor/16x16/apps/
%{_datadir}/icons/hicolor/22x22/apps/
%{_datadir}/icons/hicolor/24x24/apps/
%{_datadir}/icons/hicolor/32x32/apps/
%{_datadir}/icons/hicolor/48x48/apps/
END
cp -f ${package_name} ${srcpath}
cd /usr/src/packages/SPECS
rpmbuild -ba --clean --rmspec --rmsource ${progname}.spec
echo "Cleaning..."
rm -r -f ${rpmbuild_dir}

