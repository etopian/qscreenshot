#!/bin/bash
progname=qscreenshot
homedir=$HOME
builddir=${homedir}/build
rpmbuild_dir=${builddir}/${progname}
gitdir=${homedir}/github/${progname}
srcpath=${homedir}/rpmbuild/SOURCES
specpath=${homedir}/rpmbuild/SPECS

if [ -d "${rpmbuild_dir}" ]; then
	rm -r -f ${rpmbuild_dir}
fi
mkdir -p ${rpmbuild_dir}
mkdir -p ${srcpath}
mkdir -p ${specpath}

cd ${homedir}/github
git clone https://code.google.com/p/qscreenshot/ ${gitdir}

cp -rf ${gitdir}/qScreenshot/* ${rpmbuild_dir}/
defines=${rpmbuild_dir}/src/defines.h
ver_s=$(grep APP_VERSION $defines)
ver=$(eval echo $(echo $ver_s | cut -d ' ' -f 3))
package_name=${progname}-${ver}.tar.gz
srctmp=${builddir}/${progname}-${ver}
if [ -d "${srctmp}" ]
then
	rm -r -f ${srctmp}
fi
mkdir -p ${srctmp}
cp -r -f ${gitdir}/qScreenshot/* ${srctmp}/
cd ${builddir}
tar -pczf ${package_name} ${progname}-${ver}
cat <<END >${specpath}/${progname}.spec
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
mkdir -p %{buildroot}/usr/share/qscreenshot
mkdir -p %{buildroot}/usr/share/qscreenshot/translations

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
%{_datadir}/qscreenshot
%{_datadir}/qscreenshot/translations
END
cp -f ${package_name} ${srcpath}/
cd ${specpath}
rpmbuild -ba --clean --rmspec --rmsource ${progname}.spec
echo "Cleaning..."
rm -rf ${rpmbuild_dir} ${srctmp}
