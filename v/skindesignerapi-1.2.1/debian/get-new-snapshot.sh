#!/bin/bash
# set -x

DISTRIBUTION=trusty
BUILDPREFIX=frodo
FORCEVERSION=''

DEB_SOURCE_PACKAGE=$(egrep '^Source: ' debian/control | cut -f 2 -d ' ')

OLDDEBIANVERSION=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/\.git.*//' -e 's/~.*//g')
PKGVERSION=`dpkg-parsechangelog | sed -n 's/^Version: \(.*\)-.*/\1/p'`
SUBVERSION=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/~.*//g' | awk -F'-' '{ print $4 }' | sed -e "s/${BUILDPREFIX}.*//g")
if [ "$SUBVERSION" = '' ] ; then
	SUBVERSION=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/~.*//g' | awk -F'-' '{ print $2 }' | sed -e "s/${BUILDPREFIX}.*//g")
fi

APIMAJOR=$(cat Makefile | grep 'MAJOR =' | awk '{ print $3 }')
APIMINOR=$(cat Makefile | grep 'MINOR =' | awk '{ print $3 }')
APIVERSION="${APIMAJOR}.${APIMINOR}"
cd ../vdr-plugin-skindesigner
GIT_SHA=$(git show --pretty=format:"%h" --quiet | head -1 || true)
GIT_COMMITS=$(git log --pretty=format:"%h" | wc -l)
cd -

if [[ $PKGVERSION == $APIVERSION ]] ; then
	SUBVERSION=$((SUBVERSION +1))
else
	SUBVERSION=0
fi

BUILDVERSION=${APIVERSION}-${GIT_COMMITS}-${GIT_SHA}
NEWVERSION=${BUILDVERSION}-${SUBVERSION}${BUILDPREFIX}0~${DISTRIBUTION}

if [ ! -f ../${DEB_SOURCE_PACKAGE}_${BUILDVERSION}.orig.tar.xz ] ; then
	pwd
	tar --exclude=debian -cJvf ../${DEB_SOURCE_PACKAGE}_${BUILDVERSION}.orig.tar.xz ../lib${DEB_SOURCE_PACKAGE}
fi

dch -v ${FORCEVERSION}${NEWVERSION} -u medium -D ${DISTRIBUTION} --force-distribution "new upstream snapshot"
[ $? -ne 0 ] && exit 1
while read -r line ; do dch -a "${line}" ; done <<< "${CHANGES}"
debuild -S -sa

OTHERDISTRIBUTION=precise
OTHERVERSION=${BUILDVERSION}-${SUBVERSION}${BUILDPREFIX}0~${OTHERDISTRIBUTION}
dch -v ${FORCEVERSION}${OTHERVERSION} --force-bad-version -u medium -D ${OTHERDISTRIBUTION} --force-distribution "rebuild for ${OTHERDISTRIBUTION}"
while read -r line ; do dch -a "${line}" ; done <<< "${CHANGES}"
debuild -S -sa

exit 0
