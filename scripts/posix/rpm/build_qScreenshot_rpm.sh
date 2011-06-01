#!/bin/bash
###########################################################################################
#									      #
# ВНИМАНИЕ! Для корректной работы скрипта необходим пакет zenity!!!!!		      #
#									      #
###########################################################################################
SOURCE_DIR="qScreenshot-read-only"					#папка с исходниками qScreenshot
RESULT_DIR="myrms"							#директория в которую будет скопирован готовый rpm-пакет в ней же будет создан локальный репозиторий
DEBUG_M="n"								# "y" - делать debug-пакеты, "n" - не делать
LOG_SAVE="y"								#сохранять("y") или нет("n") лог сборки при удачном завершении
DIST_SUF="mdv"								#определяет суффикс пакета
DIST_VER=`lsb_release -a | awk '{if ($1 == "Release:") print $2}'`
SOURCE_SVN="http://qscreenshot.googlecode.com/svn/trunk/qScreenshot"
UPLOAD_GOOGLE="n"							# "y" - загружать, "n" - не загружать готовый пакет на http://code.google.com/p/qscreenshot/downloads/list
GOOGLE_AKK=""							#ваш google-аккаунт. необходим для загрузки готового пакета на страницу загрузки проекта
GOOGLE_PASS=""						#для получения этого пароля посетите https://code.google.com/hosting/settings

export LANG=ru_RU.UTF-8
SVN_FETCH="svn co --trust-server-cert --non-interactive"
SVN_UP="svn up --trust-server-cert --non-interactive"
#последняя удачно собранная ревизия
LAST_VER=""
LAST_BUILD=""
LAST_TIME=""
LAST_UPLOAD=""
#######################
# FUNCTIONS / ФУНКЦИИ #
#######################
#функция обработки ошибок
die() {
  kill $zenity_pid;
  zenity --error --text="ERROR!!!/ОШИБКА!! \n$@" & exit 1;
}
version_vac(){
cd ~/
name_prog=`awk '{if ($2 == "APP_NAME") print $3}' ./${SOURCE_DIR}/src/defines.h | sed -e 's/"//g' | tr "[:upper:]" "[:lower:]"`
ver=`awk '{if ($2 == "APP_VERSION") print $3}' ./${SOURCE_DIR}/src/defines.h | sed -e 's/"//g'`
if [ "${LAST_VER}" = "" -o "${LAST_BUILD}" = "" -o "${LAST_TIME}" = "" ]; then
sed 's/^LAST_VER=".*"$/LAST_VER="'$ver'"/' $0 > $0.new
sed -e 's/^LAST_BUILD=".*"$/LAST_BUILD="'$rev'"/' -i $0.new
sed -e 's/^LAST_TIME=".*"$/LAST_TIME="'$daterev'"/' -i $0.new
B_MOD="-bb"
verprog=${ver}.${rev}
 else last_b=$LAST_BUILD
      if [ $ver != $LAST_VER ]; then echo "Новый релиз"
	      sed 's/^LAST_VER=".*"$/LAST_VER="'$ver'"/' $0 > $0.new
	      B_MOD="-ba"
	      D_M="%global _enable_debug_package 0
%global debug_package %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress %{nil}"
	      verprog=${ver}
	else echo $rev
	      echo $last_b
	      B_MOD="-bb"
	      if [ $daterev -gt $LAST_TIME -o $rev -ne $LAST_BUILD ]; then verprog=${ver}.${rev}
		  else kill $zenity_pid;
			echo "Сборка не требуется!";
			exit 1;
	  fi;
      fi;
fi;
}
#функция скачивания исходников из SVN-репозитория
dln_svn(){
$SVN_FETCH ${SOURCE_SVN} ~/${SOURCE_DIR}
}
vac_svn(){
$SVN_UP ~/${SOURCE_DIR}
 #получаем информацию о ревизии сборки и дату ревизии
cd ~/${SOURCE_DIR}
rev=`svn info | awk '{if ($3 == "Rev:") print $4}'`
daterev=`date --utc "--date=$(svn info | awk '{if ($3 == "Date:") print $4}' && svn info | awk '{if ($3 == "Date:") print $5}' && svn info | awk '{if ($3 == "Date:") print $6}')" +%Y%m%d%H%M`
version_vac
cd ~/
svn export ~/${SOURCE_DIR} ~/${name_prog}-${ver}
}
#функция скачивания их и обработки
dnl_src(){
	    dln_svn
	    vac_svn;
}
zenity --notification --window-icon="info" --text="Сборка qScreenshot" & zenity_pid=$!
#определение платформы для сборки и проверки зависимостей
B_T="`arch`"
[ "${B_T}" = "x86_64" ] && [ -d /usr/lib64 ] && C_LIB="lib64" || C_LIB="lib"

if [ "$DEBUG_M" == "y" ]; then D_M=""
			  echo "Checking for qt4-debug.. / Проверка наличия qt4-debug.."
			  v=`rpm -V qt4-debug 2>/dev/null` || die "You should install qt4-debug first. / Вы выбрали сборку с созданием debug-пакета! Сначала установите qt4-debug!!";
			  echo "Checking for glibc-debug.. / Проверка наличия glibc-debug.."
			  v=`rpm -V glibc-debug 2>/dev/null` || die "You should install glibc-debug first. / Вы выбрали сборку с созданием debug-пакета! Сначала установите glibc-debug!!";

   elif  [ "$DEBUG_M" == "n" ]; then D_M="%global _enable_debug_package 0
%global debug_package %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress %{nil}";
    else die 'Не знаю собирать debug-пакет или нет! Определите параметр DEBUG_M в начале скрипта!!';
fi
[ $C_LIB == "lib" ] && B_TARGET=`grep -i "${B_T}" ~/.rpmrc | awk '{print $3}'` || B_TARGET="$B_T"
echo $B_TARGET
#поиск исходников
if [ -d ${SOURCE_DIR} ]; then
	cd ${SOURCE_DIR}
	folder=`ls -d .svn 2>/dev/null`
	case $folder in
	".svn") info=`svn info | awk '{if ($1 == "URL:") print $2}'`
	  if [ "${info}" == "${SOURCE_SVN}" ]; then
	  echo 'Source qScreenshot found!/ Исходники qScreenshot найдены!'
	  vac_svn;
	  else  echo 'Source qScreenshot not found!/ Исходники qScreenshot не найдены! Ссылка не удовлетворяет условию!'
		echo "Directory $SOURCE_DIR will by renamed $SOURCE_DIR-old!/ Папка $SOURCE_DIR будет переименована в $SOURCE_DIR-old"
		cd ~/
		mv  ${SOURCE_DIR} ${SOURCE_DIR}-old
		dnl_src;
	  fi;;
	"") echo 'Source qScreenshot not found! / Исходники qScreenshot не найдены!'
		cd ~/
		mv  ${SOURCE_DIR} ${SOURCE_DIR}-old
		dnl_src;
	esac
else	dnl_src;
fi;

# пакуем в архив
tar -czf ${name_prog}-${verprog}.tar.gz ${name_prog}-${ver}

#определяем версию Qt
QT_VER=`qmake -v | awk '{if ($2 == "Qt") print $4}'`
echo $QT_VER


cat <<END >~/rpm/SPECS/${name_prog}.spec
${D_M}
Summary: Simple Screenshot Maker
Name: ${name_prog}
Version: ${verprog}
Release:  ${DIST_SUF}.%{dist}
License: GPL-2
Group: Productivity/Graphics/Other
URL: http://code.google.com/p/qscreenshot/
Source0: %{name}-%{version}.tar.gz
Requires: %{_lib}qtcore4 => 4:${QT_VER}
Requires: %{_lib}qtgui4 => 4:${QT_VER}
Requires: %{_lib}qtnetwork4 => 4:${QT_VER}
BuildRequires: gcc-c++
BuildRequires: zlib-devel
BuildRequires:	%{_lib}qt4-devel
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple screenshot maker tool

%prep
%setup -q -n %{name}-${ver}

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

mv ${name_prog}-${verprog}.tar.gz ~/rpm/SOURCES
rm -rf ~/${name_prog}-${ver}*
#собираем
cd ~/rpm/SPECS
rpmbuild ${B_MOD} --clean --rmspec --rmsource --target ${B_TARGET} ${name_prog}.spec > ~/gScrbuild.log 2>&1 || die "Сборка qScreenshot завершилась с ошибкой \nсмотрите лог сборки ~/gScrbuild.log \
								    \nи обратитесь в vacuum@conference.jabber.ru";

#чистим мусор от сборки и скидываем готовый пакет в ~/${RESULT_DIR}
[ $LOG_SAVE == "n" ] && rm -Rf ~/gScrbuild.log;
PACK_NAME="${name_prog}-${verprog}-${DIST_SUF}.${DIST_VER}.${B_TARGET}.rpm"

cd ~/rpm/RPMS/${B_TARGET}/
if [ -d ~/${RESULT_DIR} ]; then
mv ${PACK_NAME} ~/${RESULT_DIR}
else mkdir ~/${RESULT_DIR}
mv ${PACK_NAME} ~/${RESULT_DIR}
fi
[ "${B_MOD}" == "-ba" ] && mv ~/rpm/SRPMS/*.rpm ~/${RESULT_DIR}
cd ~/
#создаем или обновляем информацию о пакетах в локальном репозитории
b=${RESULT_DIR}
[ -d $b/media_info ] || mkdir $b/media_info
rm -f $b/media_info/*hdlist.cz
rm -f $b/media_info/*.lzma*
rm -f $b/media_info/*.tmp
rm -f $b/media_info/MD5SUM
# Создаём описания hdlist*.cz *.xml.lzma MD5SUM
genhdlist2 --clean --xml-info `pwd`/$b
# md5 суммы описаний
chmod 444 $b/*.rpm
touch $b/*.rpm
cd ~/
#записываем номер последней удачно собранной ревизии
if [ ! -w $0.new ]; then sed 's/^LAST_BUILD=".*"$/LAST_BUILD="'$rev'"/' $0 > $0.new
else
sed -e 's/^LAST_BUILD=".*"$/LAST_BUILD="'$rev'"/' -i $0.new
fi;
sed -e 's/^LAST_TIME=".*"$/LAST_TIME="'$daterev'"/' -i $0.new;
kill $zenity_pid;
zenity --info --text="Собран новый ${PACK_NAME}\n Зайдите в ${RESULT_DIR} и установите пакет!" &
if [ $UPLOAD_GOOGLE != "n" ]; then zenity --notification --window-icon="info" --text="Загрузка файла на http://code.google.com/p/qscreenshot" & zenity_pid=$!
python ./googlecode_upload.py -u ${GOOGLE_AKK} -w ${GOOGLE_PASS} -p "${name_prog}" -s "qScreenshot | Qt ${QT_VER} | r${rev}" -l "NightlyBuild,OpSys-Linux,Type-Package,Package-Mandriva,Qt-4.6.x" ~/${RESULT_DIR}/${PACK_NAME} > ~/upload.log 2>&1 && sed -e 's/^LAST_UPLOAD=".*"$/LAST_UPLOAD="'"${PACK_NAME}"'"/' -i $0.new || zenity --error --text="Загрузка на http://code.google.com/p/qscreenshot/downloads\n не удалась! Проверьте ~/upload.log"
kill $zenity_pid
else echo "Загрузка не нужна."
fi
chmod +x $0.new;
mv $0.new $0 & exit 1
