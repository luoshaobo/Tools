#/bin/bash

OUTPUT_DIR="$1"
TAG_ID_NEW="$2"
TAG_ID_OLD="$3"

RESULT_FILE_NEW="new.zip"
RESULT_FILE_OLD="old.zip"

OUTPUT_PATH_NEW="$OUTPUT_DIR/$RESULT_FILE_NEW"
OUTPUT_PATH_OLD="$OUTPUT_DIR/$RESULT_FILE_OLD"

CURRENT_BRANCH=

function usage
{
    echo "Usage:"
    echo "  `basename $0` <output_dir> <tag_id_new> <tag_id_old>" >&2
}

if [ $# -lt 3 ]; then
    echo "*** Error: too few argments!" >&2
    usage
    exit 1
fi

if [ ! -d "$OUTPUT_DIR" ]; then
    echo "*** Error: not a valid directory: $OUTPUT_DIR" >&2
    exit 1
fi

rm -f "$OUTPUT_PATH_NEW"
rm -f "$OUTPUT_PATH_OLD"

CURRENT_BRANCH=`git branch | grep "\*" | tr "\*" " " | tr -d " " 2>/dev/null`
if [ -z "$CURRENT_BRANCH" ]; then
    echo "*** Error: unknown current branch" >&2
    exit 1
fi

git checkout "$TAG_ID_NEW" >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
    echo "*** Error: failed to execute: git checkout \"$TAG_ID_NEW\"" >&2
    exit 1
fi
git diff "$TAG_ID_NEW" "$TAG_ID_OLD" --name-only | xargs zip "$OUTPUT_PATH_NEW" >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
    echo "*** Error: failed to execute: git diff \"$TAG_ID_NEW\" \"$TAG_ID_OLD\" --name-only | xargs zip \"$OUTPUT_PATH_NEW\"" >&2
    exit 1
fi

git checkout "$TAG_ID_OLD" >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
    echo "*** Error: failed to execute: git checkout \"$TAG_ID_OLD\"" >&2
    exit 1
fi
git diff "$TAG_ID_OLD" "$TAG_ID_NEW" --name-only | xargs zip "$OUTPUT_PATH_OLD" >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
    echo "*** Error: failed to execute: git diff \"$TAG_ID_OLD\" \"$TAG_ID_NEW\" --name-only | xargs zip \"$OUTPUT_PATH_OLD\"" >&2
    exit 1
fi

git checkout "$CURRENT_BRANCH" >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
    echo "*** Error: failed to execute: git checkout \"$CURRENT_BRANCH\"" >&2
    exit 1
fi

git reflog delete HEAD@{0}
git reflog delete HEAD@{0}
git reflog delete HEAD@{0}
