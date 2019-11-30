echo Downloading latest from git repository...
set -e

DEB_SOURCE_PACKAGE=`egrep '^Source: ' debian/control | cut -f 2 -d ' '`

# VERSION_UPSTREAM=`dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e s/-[^-]*$// -e s/\.git.*//`
VERSION_UPSTREAM=`grep 'static const char \*VERSION *=' HISTORY.h | awk '{ print $6 }' | sed -e 's/[";]//g'`

if [ -d ".git" ] ; then
	git pull
fi
VERSION_DATE=`/bin/date --utc +%0Y%0m%0d`

GITHEAD=`git rev-list HEAD | wc -l`
GITBUILD="$(printf '%04d' "$GITHEAD")"

BUILD=`/bin/date --utc +%H%M`

VERSION_FULL="${VERSION_UPSTREAM}.git${VERSION_DATE}.${BUILD}"

git clone https://github.com/horchi/vdr-plugin-osd2web ${DEB_SOURCE_PACKAGE}

cd ${DEB_SOURCE_PACKAGE}
GIT_SHA=`git show --pretty=format:"%h" --quiet | head -1 || true`
cd ..

ARCHTYPEN="xz:J bz2:j gz:z"
for archtyp in  ${ARCHTYPEN}
do
	arch=`echo $archtyp | cut -d: -f1`
	pack=`echo $archtyp | cut -d: -f2`
	DEBSRCPKGFILE="../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.${arch}"
	DEBSRCPKGFILEBAK="${DEBSRCPKGFILE}.1"

	if [ -f ${DEBSRCPKGFILE} ] ; then
		mv ${DEBSRCPKGFILE} ${DEBSRCPKGFILEBAK}
	fi

	if [ -f ${DEBSRCPKGFILE} -o -f ${DEBSRCPKGFILEBAK} ] ; then
		echo "$DEBSRCPKGFILE exists";
		echo "$DEBSRCPKGFILEBAK exists";
		continue;
	else
		echo $DEBSRCPKGFILE
		tar --exclude=.git --exclude=debian -c${pack}f ${DEBSRCPKGFILE} ${DEB_SOURCE_PACKAGE}
		rm -rf ${DEB_SOURCE_PACKAGE}

#		git-import-orig --pristine-tar ${DEBSRCPKGFILE}
		dch -v "$VERSION_FULL-0yavdr0~precise" "New Upstream Snapshot (commit ${GIT_SHA}), build ${GITBUILD}"
		break;
	fi
done


