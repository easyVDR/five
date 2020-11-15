echo Downloading latest from git repository...
set -e

DEB_SOURCE_PACKAGE=`egrep '^Source: ' debian/control | cut -f 2 -d ' '`
VERSION_DATE=`/bin/date --utc +%0Y%0m%0d`
VERSION_FULL="0.1.2+git${VERSION_DATE}"

git clone git://projects.vdr-developer.org/${DEB_SOURCE_PACKAGE}.git

cd ${DEB_SOURCE_PACKAGE}
GIT_SHA=`git show --pretty=format:"%h" --quiet || true`
cd ..

tar --exclude=.git -czf "../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.gz" ${DEB_SOURCE_PACKAGE}

rm -rf ${DEB_SOURCE_PACKAGE}

git-import-orig --pristine-tar "../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.gz"
dch -v "$VERSION_FULL-1" "New Upstream Snapshot (commit $GIT_SHA)"


