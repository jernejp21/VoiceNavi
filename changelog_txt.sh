#!/bin/bash
#
# usage: ./mkchangelog
#
# author: Marek Wywiał <onjinx@gmail.com>
#
# description:  Script creates CHANGELOG.txt according to current git tags. Also
#               supports commits without tags (git log HEAD...first_tag).
#               Lines starting with 'Merge' will be skipped.
#               Remember to add 'include CHANGELOG.txt' into your MANIFEST.in if
#               you need it
#

rm -f CHANGELOG.txt

FIRST_TAG=`git tag|sort -Vr| head -1`
LAST_TAG=`git tag|sort -Vr| tail -1`
TAGS=`git tag|wc -l`

VERSIONS=`git tag |sort -Vr; git log --pretty=format:"%H" | tail -1`

DEV_COMMITS=`git log HEAD...${FIRST_TAG} --oneline|wc -l`

if [ ${DEV_COMMITS} -gt 0 -o ${TAGS} -eq 0 ]; then
    date=`git log HEAD -1 --pretty=format:"%ci"`
    echo "HEAD / ${date}:" >> CHANGELOG.txt
    if [ ${TAGS} -eq 0 ]; then
        git log HEAD --pretty=format:"- %s%n  %b"|grep -v -i '^ - Merge' | grep -v 'BUMP VERSION' >> CHANGELOG.txt
    else
        git log HEAD...${FIRST_TAG} --pretty=format:"- %s%n  %b"|grep -v -i '^ - Merge' | grep -v 'BUMP VERSION' >> CHANGELOG.txt
    fi
    echo >> CHANGELOG.txt
	echo >> CHANGELOG.txt
fi

prev=''
for version in ${VERSIONS}; do
    if [ -z $prev ]; then
        prev=${version}
    else
        date=`git log ${prev} -1 --pretty=format:"%ci"`
        echo "${prev} / ${date}:" >> CHANGELOG.txt
        if [ ${prev} == ${LAST_TAG} ]; then
            git log ${prev} --pretty=format:"- %s%n  %b"|grep -v -i '^ - Merge' | grep -v 'BUMP VERSION' >> CHANGELOG.txt
        else
            git log ${prev}...${version} --pretty=format:"- %s%n  %b"|grep -v -i '^ - Merge' | grep -v 'BUMP VERSION' >> CHANGELOG.txt
        fi
        echo >> CHANGELOG.txt
		echo >> CHANGELOG.txt
        prev=${version}
    fi
done