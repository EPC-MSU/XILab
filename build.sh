#!/bin/sh
set -e

param1=$1

# update to hg revision
[ -n "$MERCURIAL" ] || MERCURIAL=hg

# skip checkout magic for pipeline builds
if [ -z "$BUILDOS" ] ; then

if [ -n "$XIMC_REVISION" ] ; then
	echo Use prefered XIMC_REVISION
	$MERCURIAL update --clean --rev "$XIMC_REVISION"
else
	$MERCURIAL pull
	if [ -z "$MERCURIAL_REVISION" ] ; then
		$MERCURIAL update --clean
	else
		echo Use old-style MERCURIAL_REVISION
		$MERCURIAL update --clean --rev "$MERCURIAL_REVISION"
	fi
	if [ "$param1" = "refresh" ] ; then
		shift
		echo Refreshing done
	else
		echo Launch a refreshed version of the script
		$0 refresh $*
		exit $?
	fi
fi
echo effective version:
$MERCURIAL identify
fi

if [ -n "$BUILDOS" ] ; then
	BUILD_SUFFIX=$BUILDOS
else
	BUILD_SUFFIX=$NODE_NAME
fi

clean_some_stuff()
{
	rm -rvf data.tar.gz control.tar.gz debian-binary usr lib ximc.h ximc-[0-9\.]*[0-9] release_64* release_32* GeneratedFiles* xiresource macosx
}

# Bring Qt/Qwt versions as environment variables
. ./VERSIONS
QT_VER_MAJOR=$(echo $QT_VER |  awk -F '.' '{print$1;}' )
QWT_VER_MAJOR=$(echo $QWT_VER |  awk -F '.' '{print$1;}' )

# do not place a trailing slash
QTDIR=/usr/local/Trolltech/Qt-${QT_VER}
PATH=$QTDIR/bin:$PATH
LD_LIBRARY_PATH=$QTDIR/lib:./usr/lib:$LD_LIBRARY_PATH
export QTDIR PATH LD_LIBRARY_PATH

major="7" #major version, autodetection will be added later
libximc_linux="libximc.so.$major.0.0"
VER=$(grep VERSION src/main.h | sed 's/#define XILAB_VERSION "\([0-9a-zA-Z\ \.]*\)*"/\1/' | tr -d ' \r')
r_dir="xilab-$VER"

# cleanup first
clean_some_stuff

# save received libximc artifacts
mv ximc-[0-9\.]*[0-9].tar.gz ../

# load received libximc artifacts
mv ../ximc-[0-9\.]*[0-9].tar.gz ./
tar -xf ximc-[0-9\.]*[0-9].tar.gz

# pull resources
if [ -z "$URL_XIRESOURCE" ] ; then
  URL_XIRESOURCE="https://gitlab.ximc.ru/ximc-public/xiresource.git"
fi
$GIT clone "$URL_XIRESOURCE"

package_for_linux () {
# function parameters
arch=$1
bits=$2
pkgtype=$3
archpath=${4}-linux-gnu

# clean and symlink
rm -rf ../$r_dir
mkdir ../$r_dir
cd ../$r_dir
ln -s $libximc_linux libximc.so.$major
cd -

# unpack libximc artifact
find . -name "libximc[0-9]_[0-9-_\.]*${arch}.deb" | xargs -n 1 -r ar vx
tar -xf data.tar.gz
cp ./ximc-*/ximc/ximc.h ./
cp ./usr/lib/libximc.so.$major ./usr/lib/libximc.so
cp ./usr/lib/$libximc_linux ../$r_dir/

# copy extra files
cp xilabdefault.cfg ../$r_dir/

# add scripts and profiles
mkdir -p ../$r_dir/Library/XILab/
cp -R ./xiresource/scripts ../$r_dir/Library/XILab/
cp -R ./xiresource/profiles ../$r_dir/Library/XILab/
cp -R ./xiresource/schemes/. ../$r_dir/Library/XILab/profiles

# add qwt
cp /usr/local/qwt-${QWT_VER}/lib/libqwt.so.${QWT_VER_MAJOR} ../$r_dir/

# rewrite qwt version in .pro build files (linux)
sed "s/%qwtver/$QWT_VER/" --in-place ./linux_*_XILab_*.pro

# compile
if [ "$param1" = "add_service_build" ] ; then
	qmake linux_servicemode_XILab_${bits}.pro
	make
	mv release_${bits}/XILab_${bits}_service ../$r_dir
	strip ../$r_dir/XILab_${bits}_service
	ls -l ../$r_dir/XILab_${bits}_service
	
	make clean
fi

qmake linux_usermode_XILab_${bits}.pro
make
mv release_${bits}/XILab_${bits}_user ../$r_dir
strip ../$r_dir/XILab_${bits}_user
ls -l ../$r_dir/XILab_${bits}_user

wd=`pwd`
cd ..
tar -czf xilab-$BUILD_SUFFIX.tar.gz ./$r_dir
mv ./$r_dir/XILab_${bits}_user ./
rm -r ./$r_dir/*
mv ./XILab_${bits}_user ./$r_dir/
cd -

# prepare files for AppImage
cp /usr/local/bin/AppRun_${bits} ../$r_dir/AppRun
cp ./appimg/xilab.png ../$r_dir/
cp ./appimg/xilab.desktop ../$r_dir/
cd ..
mkdir -p ./$r_dir/usr/bin/ ./$r_dir/usr/lib/$archpath/ ./$r_dir/usr/share/libximc/ ./$r_dir/usr/share/xilab/ ./$r_dir/lib/$archpath/
cp $wd/xilabdefault.cfg ./$r_dir/usr/share/xilab/
for file in libbindy.so libximc.so.${major} libxiwrapper.so ; do cp $wd/usr/lib/$file ./$r_dir/usr/lib/ ; done
cp $wd/keyfile.sqlite ./$r_dir/usr/share/xilab/default_keyfile.sqlite
cp /usr/local/qwt-${QWT_VER}/lib/libqwt.so.${QWT_VER_MAJOR} ./$r_dir/usr/lib/
cp /lib/$archpath/libpng12.so.0 ./$r_dir/lib/$archpath/

mv ./$r_dir/XILab_${bits}_user ./$r_dir/usr/bin/xilab
sed 's#/usr/share/#././/share/#' --in-place ./$r_dir/usr/bin/xilab  # patch xilab binary - relative paths
sed "s#%ver#$VER#" --in-place ./$r_dir/xilab.desktop  # patch desktop file - version
for name in Core DBus Gui Network Script Svg Xml ; do cp /usr/lib/$archpath/libQt${name}.so.${QT_VER_MAJOR} ./$r_dir/usr/lib/$archpath/ ; done
cp /usr/lib/$archpath/libaudio.so.2 ./$r_dir/usr/lib/$archpath/


# create AppImage
appimagetool-${pkgtype}.AppImage -n ./$r_dir/
if [ -f "xilab.appdata.xml" ]; then  # imagetool on 32-bit Linux is bugged without "-n" switch
  mv "xilab.appdata.xml" "XILab-Intel_80386.AppImage"
fi
mv XILab-*.AppImage "xilab-${VER}-${pkgtype}.AppImage"

# add scripts and profiles
cd -
mkdir -p ../$r_dir/AppImage
cp -R ./xiresource/scripts ../$r_dir/AppImage
cp -R ./xiresource/profiles ../$r_dir/AppImage
cp -R ./xiresource/schemes/. ../$r_dir/AppImage/profiles
mv ../xilab-${VER}-${pkgtype}.AppImage ../$r_dir/AppImage
cd ../$r_dir/AppImage
tar -czf ../xilab-${VER}-${pkgtype}.tar.gz ./
cd -
mv ../$r_dir/xilab-${VER}-${pkgtype}.tar.gz ../
rm -r ../$r_dir/AppImage

# move artifacts
cd "$wd"
mv ../xilab-$BUILD_SUFFIX.tar.gz ./
mv ../xilab-${VER}-${pkgtype}.tar.gz ./

# clean up
rm -rf ../xilab-${VER}
clean_some_stuff
}

package_for_macosx () {
# clean
rm -rf ../$r_dir
mkdir ../$r_dir

# unpack libximc artifact
cp ./ximc-*/ximc/ximc.h ./
cp -R ./ximc-*/ximc/macosx ./

# rewrite qwt version in .pro build files (mac)
sed -i '' "s/%qwtver/$QWT_VER/" ./mac_XILab.pro

# compile
qmake mac_XILab.pro -spec unsupported/macx-clang-libc++
make

# rewrite Qt paths inside Xilab with Qt tool
macdeployqt release/XILab.app
mv release/XILab.app/ ../$r_dir/
strip ../$r_dir/XILab.app/Contents/MacOS/XILab
ls -l ../$r_dir/XILab.app

# add qwt to the bundle
mkdir -p ../$r_dir/XILab.app/Contents/Frameworks/qwt.framework/Versions/${QWT_VER_MAJOR}/
cp /usr/local/qwt-${QWT_VER}/lib/qwt.framework/Versions/${QWT_VER_MAJOR}/qwt ../$r_dir/XILab.app/Contents/Frameworks/qwt.framework/Versions/${QWT_VER_MAJOR}/qwt

# rewrite Qt paths inside Qwt with our own hands
qt_root="$QTDIR/lib"
for sublib in QtCore QtGui QtSvg ; do
  subpath="${sublib}.framework/Versions/${QT_VER_MAJOR}/$sublib"
  install_name_tool -change "$qt_root/$subpath" "@executable_path/../Frameworks/$subpath" ../$r_dir/XILab.app/Contents/Frameworks/qwt.framework/Versions/${QWT_VER_MAJOR}/qwt
done

# add libximc framework (with xiwrapper and bindy inside) to the bundle
cp -r ./macosx/libximc.framework ../$r_dir/XILab.app/Contents/Frameworks/

# add bindy dylib (needed since Xilab 1.13) to the bundle
cp -r ./macosx/libximc.framework/Versions/${major}/Frameworks/libbindy.dylib ../$r_dir/XILab.app/Contents/Frameworks/

# add xilabdefault.cfg to program dir
cp xilabdefault.cfg ../$r_dir/XILab.app/Contents/MacOS/

# add default libximc keyfile (for bindy) to program dir
cp keyfile.sqlite ../$r_dir/XILab.app/Contents/MacOS/default_keyfile.sqlite

# fix permissions for framework and bindy because original permissions are proabably missing
chmod -R ugo+rX ../$r_dir/XILab.app/Contents/Frameworks/libximc.framework ../$r_dir/XILab.app/Contents/Frameworks/libbindy.dylib ../$r_dir/XILab.app/Contents/MacOS/default_keyfile.sqlite

# add scripts and profiles
mkdir -p ../$r_dir/Library/XILab/
cp -R ./xiresource/scripts ../$r_dir/Library/XILab/
cp -R ./xiresource/profiles ../$r_dir/Library/XILab/
cp -R ./xiresource/schemes/. ../$r_dir/Library/XILab/profiles

# package profiles
XIMC_DIR=./ximc-*/ximc
CFG_DIR=../$r_dir/Library/XILab/profiles/STANDA
mkdir -p ../$r_dir/Library/XILab
ARCHIVE_DIR=.
. ./profiles.sh

for path in $XIMC_DIR/c-profiles/*; do
	filename="${path##*/}"
	if [ "$filename" = "STANDA" ]; then
		echo "STANDA already compressed"
	else
		echo "Compressing $filename"
		tar -rvf "profile-$filename.tar" -C $XIMC_DIR "c-profiles/$filename"
		tar -rvf "profile-$filename.tar" -C $XIMC_DIR "python-profiles/$filename"
		tar -rvf "profile-$filename.tar" -C ../$r_dir/Library/XILab "profiles/$filename"
		gzip "profile-$filename.tar"
		if [ ! $ARCHIVE_DIR -ef . ]; then
			mv "profile-$filename.tar.gz" $ARCHIVE_DIR
		fi
	fi
done

# package and create .dmg volume to hold the installer
cd ..
rm -rf installer.pkg
rm -rf dmg xilab-*.dmg
mkdir ./$r_dir/Applications
mv ./$r_dir/XILab.app ./$r_dir/Applications/
plist="component.plist"
pkgbuild --analyze --root "./$r_dir/" "$plist"
pkgbuild --root "./$r_dir/" --version "$VER" --component-plist "$plist" --identifier "com.ximc.xilab" --install-location "/"  installer.pkg
rm "$plist"
mkdir -p dmg
cp -pR installer.pkg dmg
hdiutil create xilab-${VER}.dmg -volname "XILab-${VER}" -fs HFS+ -srcfolder dmg
tar -czf xilab-${VER}-osx64.tar.gz ./xilab-${VER}.dmg
cd - && mv ../xilab-${VER}-osx64.tar.gz ./

# cleanup macosx
rm -rf ../installer.pkg ../xilab-${VER}.dmg ../xilab-${VER} ../dmg
clean_some_stuff
}

case "`uname -s`" in
	Darwin)
		package_for_macosx
		;;
	Linux)
		ARCH=$(uname -m)
		if [ "$ARCH" = "x86_64" ] ; then
			ARCHNAME=amd64
			DISTNAME=x86_64
			LIBPREFIX=x86_64
			BITS=64
		else
			ARCHNAME=i386
			DISTNAME=ia32
			LIBPREFIX=i386
			BITS=32
		fi
		package_for_linux $ARCHNAME $BITS $DISTNAME $LIBPREFIX
		;;
	FreeBSD)
		;;
	*)
		echo "Wrong distribution"
		exit 1
		;;
esac

echo Success
exit 0
