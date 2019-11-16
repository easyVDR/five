echo Downloading latest from git repository...
set -e

DEB_SOURCE_PACKAGE=`egrep '^Source: ' debian/control | cut -f 2 -d ' '`
VERSION_DATE=`/bin/date --utc +%0Y%0m%0d`
VERSION_FULL="0.2.0+git${VERSION_DATE}"

git clone --depth 1 git://projects.vdr-developer.org/${DEB_SOURCE_PACKAGE}.git

cd ${DEB_SOURCE_PACKAGE}
GIT_SHA=`git show --pretty=format:"%h" --quiet || true`
cd ..

tar --exclude=.git -czf "../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.gz" ${DEB_SOURCE_PACKAGE}

dch -v "$VERSION_FULL-1" "New Upstream Snapshot (commit $GIT_SHA)"

rm -rf ${DEB_SOURCE_PACKAGE}
