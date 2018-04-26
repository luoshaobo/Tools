#/bin/bash

(
    cd . && rm -fr Debug Release *.ncb *.suo
)

(
    cd ChineseStrSort && rm -fr Debug Release *.user tmp/* plugins/*
)

(
    cd PhrasePinyinTableData && rm -fr Debug Release *.user
)

(
    cd UnicodePinyinTableData && rm -fr Debug Release *.user
)

