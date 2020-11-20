#!/bin/sh

VAR_LOG_ENABLED="false"
VAR_TEST_MODE="false"
VAR_JOBS=${JOBS:-32}
VAR_VERBOSE=${VERBOSE:-0}
VAR_NEED_TO_CLEAN="false"
VAR_NEED_TO_CLOBBER="false"
VAR_NEED_TO_BUILD="true"
VAR_COMPLETE_CMDLINE="false"
VAR_USE_DEFAULT_PACKAGE_GROUP=0
VAR_PACKAGE_LIST=()

VAR_DEFAULT_PACKAGE_LIST_1=( \
    CommunicationManager \
    FoundationServiceManager \
    IPCommandBroker \
    LocalConfig \
    VehicleComm \
    VolvoOnCall \
)

VAR_DEFAULT_PACKAGE_LIST_2=( \
    RTP-Audio \
    AssistanceCall \
    BLEManager \
    BusinessServiceFramework \
    CommunicationManager \
    FoundationServiceManager \
    IPCommandBroker \
    LocalConfig \
    NAD_ThermalManager \
    ServiceFramework \
    SIMManager \
    tcam-diag \
    TcamSetup \
    UserManager \
    VehicleComm \
    VolvoPositioningManagerGDBusClient \
    VolvoConnectionManager \
    VolvoOnCall \
    VolvoPositioningManager \
    NAD_ThermalManager \
)

VAR_DEFAULT_PACKAGE_LIST_3=( \
    AssistanceCall \
    BLEManager \
    BusinessServiceFramework \
    CommunicationManager \
    FoundationServiceManager \
    IPCommandBroker \
    LocalConfig \
    NAD_ThermalManager \
    ServiceFramework \
    SIMManager \
    tcam-diag \
    TcamSetup \
    UserManager \
    VehicleComm \
    VolvoConnectionManager \
    VolvoOnCall \
    VolvoPositioningManager \
)

function log
{
    if [ "$VAR_LOG_ENABLED" = "true" ]; then
        echo "$@"
    fi
}

function output_package_list_in_usage
{
    local item
    local PACKAGE
        
    for item in ${@}; do
        eval PACKAGE=$item
        echo "            $PACKAGE"
    done
}

##########################################################################################
# usage examples
#
#   # to clean package group 1 (the default package group)
#   build_packages.sh -c -B
#   # to clobber package group 1 (the default package group)
#   build_packages.sh -C -B
#   # to clean and clobber package group 1 (the default package group)
#   build_packages.sh -c -C -B
#   # to build package group 1 (the default package group)
#   build_packages.sh
#   # to clean, clobber and build package group 1 (the default package group)
#   build_packages.sh -c -C
#
#   # to clean package group 2
#   build_packages.sh -c -B -g 2
#   # to clobber package group 2
#   build_packages.sh -C -B -g 2
#   # to clean and clobber package group 2
#   build_packages.sh -c -C -B -g 2
#   # to build package group 2
#   build_packages.sh -g 2
#   # to clean, clobber and build package group 2
#   build_packages.sh -c -C -g 2
#
#   # to clean package VehicleComm and VolvoOnCall
#   build_packages.sh -c -B VehicleComm VolvoOnCall
#   # to clobber package VehicleComm and VolvoOnCall
#   build_packages.sh -C -B VehicleComm VolvoOnCall
#   # to clean and clobber package VehicleComm and VolvoOnCall
#   build_packages.sh -c -C -B VehicleComm VolvoOnCall
#   # to build package VehicleComm and VolvoOnCall
#   build_packages.sh VehicleComm VolvoOnCall
#   # to clean, clobber and build package VehicleComm and VolvoOnCall
#   build_packages.sh -c -C VehicleComm VolvoOnCall

function usage
{
    echo "`basename $0` [--help|-H] [--test-mode|-t] [--verbose|-V] [--clean|-c] [--clobber|-C] [--not-build|-B] --package-group=|-g 1|2|3"
    echo "`basename $0` [--help|-H] [--test-mode|-t] [--verbose|-V] [--clean|-c] [--clobber|-C] [--not-build|-B] [<package1> [<package2> [<package3> ...]]]"
    echo "    --help|-H: to output this information"
    echo "    --test-mode|-t: to just show the steps without real actions, for test purpose"
    echo "    --verbose|-V: to output more information, for debug porpuse"
    echo "    --clean|-c: to clean packages"
    echo "    --clobber|-C: to clobber packages"
    echo "    --not-build|-B: not to build packages"
    echo "    --package-group=|-g 1|2|3"
    echo "        package-group 1, as the defalt, includes:"
    output_package_list_in_usage "${VAR_DEFAULT_PACKAGE_LIST_1[@]}"
    echo "        package-group 2 includes:"
    output_package_list_in_usage "${VAR_DEFAULT_PACKAGE_LIST_2[@]}"
    echo "        package-group 3 includes nothing, just for customization."
    echo "    <package1> [<package2> [<package3> ...]]: one to more packages"
}

function parse_args
{
    local ARGV
    local opt
    local CMD_PACKAGE_LIST

    ARGV=($(getopt -l help,test-mode,verbose,clean,clobber,not-build,package-group:complete -o 'HtVcCBg:T' -- "$@"))
    if [ $? -ne 0 ]; then
        usage
        exit 1
    fi
    for((i=0;i<${#ARGV[@]};i++)) {
        eval opt=${ARGV[$i]}
        case $opt in
            --help)
                log "--help"
                usage
                exit 0
                ;;
            -H)
                log "-H"
                usage
                exit 0
                ;;
            --test-mode)
                log "--test-mode"
                VAR_TEST_MODE="true"
                ;;
            -t)
                log "-t"
                VAR_TEST_MODE="true"
                ;;
            --verbose)
                log "--verbose"
                VAR_LOG_ENABLED="true"
                ;;
            -V)
                log "-V"
                VAR_LOG_ENABLED="true"
                ;;  
            --clean)
                log "--clean"
                VAR_NEED_TO_CLEAN="true"
                ;;
            -c)
                log "-c"
                VAR_NEED_TO_CLEAN="true"
                ;;
            --clobber)
                log "--clobber";
                VAR_NEED_TO_CLOBBER="true"
                ;;
            -C)
                log "-C";
                VAR_NEED_TO_CLOBBER="true"
                ;;
            --not-build)
                log "--not-build";
                VAR_NEED_TO_BUILD="false"
                ;;
            -B)
                log "-B";
                VAR_NEED_TO_BUILD="false"
                ;;
            --package-group)
                ((i++))
                eval VAR_USE_DEFAULT_PACKAGE_GROUP=${ARGV[$i]};
                log "--package-group: $VAR_USE_DEFAULT_PACKAGE_GROUP"
                ;;
            -g)
                ((i++))
                eval VAR_USE_DEFAULT_PACKAGE_GROUP=${ARGV[$i]};
                log "-g: $VAR_USE_DEFAULT_PACKAGE_GROUP"
                ;;
            -complete)
                log "-complete";
                VAR_COMPLETE_CMDLINE="true"
                ;;
            -T)
                log "-T";
                VAR_COMPLETE_CMDLINE="true"
                ;;
            --)
                ((i++))
                break
                ;;
        esac
    }
        
    #
    # When no packages and no package list group given, force set to use package list 1.
    #
    CMD_PACKAGE_LIST=${ARGV[@]:$i}
    if [ -z "$CMD_PACKAGE_LIST" -o ${#CMD_PACKAGE_LIST[@]} -eq 0 ]; then
        if [ $VAR_USE_DEFAULT_PACKAGE_GROUP -eq 0 ]; then
            VAR_USE_DEFAULT_PACKAGE_GROUP=1
        fi
    else
        if [ $VAR_USE_DEFAULT_PACKAGE_GROUP -ne 0 ]; then
            usage
            exit 1
        fi
    fi
    
    case $VAR_USE_DEFAULT_PACKAGE_GROUP in
        0)
            VAR_PACKAGE_LIST=${ARGV[@]:$i}
            ;;
        1)
            VAR_PACKAGE_LIST=${VAR_DEFAULT_PACKAGE_LIST_1[@]}
            ;;
        2)
            VAR_PACKAGE_LIST=${VAR_DEFAULT_PACKAGE_LIST_2[@]}
            ;;
        3)
            VAR_PACKAGE_LIST=${VAR_DEFAULT_PACKAGE_LIST_3[@]}
            ;;
        *)
            usage
            exit 1
            ;;
    esac
    
    log "$VAR_PACKAGE_LIST"
    
}

function clean_package
{
    local PACKAGE
    
    PACKAGE="$1"
    make J=$VAR_JOBS V=$VAR_VERBOSE CAS_TARGET_HW=gly-tcam $PACKAGE-clean
}

function clobber_package
{
    local PACKAGE

    PACKAGE="$1"
    make J=$VAR_JOBS V=$VAR_VERBOSE CAS_TARGET_HW=gly-tcam $PACKAGE-clobber
}

function build_package
{
    local PACKAGE
    
    PACKAGE="$1"
    make J=$VAR_JOBS V=$VAR_VERBOSE CAS_TARGET_HW=gly-tcam $PACKAGE
}

function complete_cmdline
{
    local cur prev STRINGS1 STRINGS2 STRINGS3 STRINGS
    
    COMPREPLY=()
    
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    
    STRINGS1=`echo ${VAR_DEFAULT_PACKAGE_LIST_1[@]}`
    STRINGS2=`echo ${VAR_DEFAULT_PACKAGE_LIST_2[@]}`
    STRINGS3=`echo ${VAR_DEFAULT_PACKAGE_LIST_2[@]}`
    STRINGS="$STRINGS1 $STRINGS2 $STRINGS3"
    
    if [[ ${cur} == * ]] ; then
        COMPREPLY=( $(compgen -W "$STRINGS" -- ${cur}) )
        return 0
    fi
}

function main
{
    local PACKAGE
    local item
    local THIS_CMD CMD
    
    parse_args $@
    
    if [ "$VAR_COMPLETE_CMDLINE" == "true" ]; then
        THIS_CMD="`basename ${BASH_SOURCE[0]}`"
        complete -F complete_cmdline $THIS_CMD
        complete -F complete_cmdline make_tcam
        complete -F complete_cmdline make_tcam_j
        return 0
    fi
    
    log "VAR_JOBS=$VAR_JOBS"
    log "VAR_VERBOSE=$VAR_VERBOSE"
        
    if [ "$VAR_NEED_TO_CLEAN" = "true" ]; then
        for item in ${VAR_PACKAGE_LIST[@]}; do
            eval PACKAGE=$item
            echo "============ To clean $PACKAGE ============"
            if [ "$VAR_TEST_MODE" != "true" ]; then
                clean_package "$PACKAGE"
            fi            
            if [ $? -ne 0 ]; then
                exit 1
            fi
        done
    fi
    
    if [ "$VAR_NEED_TO_CLOBBER" = "true" ]; then
        for item in ${VAR_PACKAGE_LIST[@]}; do
            eval PACKAGE=$item
            echo "============ To clobber $PACKAGE ============"
            if [ "$VAR_TEST_MODE" != "true" ]; then
                clobber_package "$PACKAGE"
            fi
            if [ $? -ne 0 ]; then
                exit 1
            fi
        done
    fi
    
    if [ "$VAR_NEED_TO_BUILD" = "true" ]; then
        for item in ${VAR_PACKAGE_LIST[@]}; do
            eval PACKAGE=$item
            echo "============ To build $PACKAGE ============"
            if [ "$VAR_TEST_MODE" != "true" ]; then
                build_package "$PACKAGE"
            fi
            if [ $? -ne 0 ]; then
                exit 1
            fi
        done
    fi
}

main $@
