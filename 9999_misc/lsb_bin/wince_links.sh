#!/bin/bash

#
# NOTE:
#
# 1)  The targets of these links:
#         ./import/_MMP_PDK
#         ./import/_MMP_PROD
#     may be changed by hand!
# 2)  You can excute this script by:
#         `basename $0` -c
#     then these files:
#          ./$CE_UNLINK_SCRIPT
#          ./$CE_RELINK_SCRIPT
#     will be created.
#     Before you copy all of the WinCE tree, you must excute:
#          ./$CE_UNLINK_SCRIPT
#     After you have copy all of the WinCE tree, you must excute:
#          ./$CE_RELINK_SCRIPT
          
    # ./import/_MMP_PDK -> E:\casdev\C1.04.04_ie\MMP_PDK_9.49.0_PROD
    # ./import/_MMP_PROD -> E:\casdev\C1.04.04_ie\C1.04.04\DPCA_iMX\Delivery\MMP_GENERIC
    # ./platform/common/src/soc/COMMON_FSL_V3 -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_BSP\IMX53_ARD\SOC\COMMON_FSL_V3
    # ./platform/common/src/soc/MX53_FSL_V3 -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_BSP\IMX53_ARD\SOC\MX53_FSL_V3
    # ./platform/IMX53_ARD -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_BSP\IMX53_ARD
    # ./public/MMP_PROD/OAK/INC/_AV -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\AV\inc
    # ./public/MMP_PROD/OAK/INC/_BSP -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\BSP\inc
    # ./public/MMP_PROD/OAK/INC/_CVTO -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\CVTO\inc
    # ./public/MMP_PROD/OAK/INC/_DIAG -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\DIAG\inc
    # ./public/MMP_PROD/OAK/INC/_DICTA -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\DICTA\inc
    # ./public/MMP_PROD/OAK/INC/_ELY -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\ELY\inc
    # ./public/MMP_PROD/OAK/INC/_HS -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\HS\inc
    # ./public/MMP_PROD/OAK/INC/_LCM -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\LCM\inc
    # ./public/MMP_PROD/OAK/INC/_MED -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\MED\inc
    # ./public/MMP_PROD/OAK/INC/_NAV -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\NAV\inc
    # ./public/MMP_PROD/OAK/INC/_RAD -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\RAD\inc
    # ./public/MMP_PROD/OAK/INC/_SYS -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\SYS\inc
    # ./public/MMP_PROD/OAK/INC/_TELE -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_PUBLIC\TELE\inc
    # ./public/MMP_PROD/OAK/tools -> D:\CASDEV\WINCE\import\_MMP_PROD\tools
    # ./public/MMP_PROD/QA -> D:\CASDEV\WINCE\import\_MMP_PROD\QA
    # ./public/MMP_PROD/_AV -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_AV\src
    # ./public/MMP_PROD/_CVTO -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_CVTO\src
    # ./public/MMP_PROD/_DIAG -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_DIAG\src
    # ./public/MMP_PROD/_DICTA -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_DICTA\src
    # ./public/MMP_PROD/_ELY -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_ELY\src
    # ./public/MMP_PROD/_HS -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_HS\src
    # ./public/MMP_PROD/_LCM -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_LCM\src
    # ./public/MMP_PROD/_MED -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_MED\src
    # ./public/MMP_PROD/_NAV -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_NAV\src
    # ./public/MMP_PROD/_RAD -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_RAD\src
    # ./public/MMP_PROD/_SYS -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_SYS\src
    # ./public/MMP_PROD/_TELE -> D:\CASDEV\WINCE\import\_MMP_PROD\GEN_TELE\src

CE_UNLINK_SCRIPT="_ce_unlink.sh"
CE_RELINK_SCRIPT="_ce_relink.sh"

usage()
{
    echo "Usage:"
    echo "  `basename $0` -H|--help"
    echo "  `basename $0` -c"
    echo "  `basename $0`"
    echo ""
}

if [ $# -ge 1 ]; then
    if [ "$1" = "-h" -o "$1" = "-H" -o "$1" = "--help" ]; then
        usage
        exit 0
    fi
fi

IS_CREATE_SCRIPT=1

if [ $# -ge 1 -a "$1" = "-c" ]; then
    IS_CREATE_SCRIPT=0
fi

if [ $IS_CREATE_SCRIPT -eq 0 ]; then
    echo "#/bin/bash" > $CE_UNLINK_SCRIPT
    echo "" >> $CE_UNLINK_SCRIPT

    echo "#/bin/bash" > $CE_RELINK_SCRIPT
    echo "" >> $CE_RELINK_SCRIPT
fi

LINKS=$(find . -type l)

for LINK in $LINKS; do
    JUNCTION_SRC="$LINK"
    JUNCTION_DST=$(junction $LINK | grep "Substitute Name:" | sed -E 's/^   Substitute Name: //')
    echo "$JUNCTION_SRC -> $JUNCTION_DST"
    
    if [ $IS_CREATE_SCRIPT -eq 0 ]; then
        echo "junction -D $JUNCTION_SRC" >> $CE_UNLINK_SCRIPT
        echo "junction $JUNCTION_SRC '$JUNCTION_DST'" >> $CE_RELINK_SCRIPT
    fi
done


