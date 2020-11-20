#!/bin/bash

SERVICE_INFO_FILE="/tmp/sp.tmp"

cat >${SERVICE_INFO_FILE} <<EOF
	mtp (01), -- TEM Provision
	mtu (02), -- TEM Unprovision
	mcq (03), -- Tem config query
 	mcu (04), -- Configuration update
	mpm (05), -- TEM power mode mgmt.
	mta (06), -- TEM activation
	mul (07), -- Upload TEM log
	mrs (08), -- TEM factory reset
	rin (11), -- Remote  control inhibition       
	rdl (13), -- Remote  door lock
	rdu (14), -- Remote  door unlock
	rtl (15), -- Remote  trunk lock
	rtu (16), -- Remote  trunk unlock
	res (17), -- Remote  engine start
	rvi (18), -- Remote  vehicle immobilization
	rcc (19), -- Remote  climate control (non-engine)
	rce (20), -- Remote  climate control
	rhl (21), -- Remote  honk & light
	rsh (22), -- Remote  seat heat
	rws (23), -- Remote  close/open window/sunroof
	rpc (24), -- Remote  PM 2.5 clean
	rpp (25), -- Remote  Pull PM 2.5
	ren (26), -- Remote  event notification
	rec (27), -- Remote EV charging, schedule
	
	cec (31), -- E Call
	cep (32), -- PSAP E Call
	cbc (33), -- B Call
	cic (34), -- I Call
	svt (41), -- SVT
	stn (44), -- Theft Notification
	rvs (51), -- Remote status & dashboard
	trs (52), -- Triggered Remote status
	jou (61), -- Journey log
	hec (62), -- Health check
	dia (63), -- Remote diagnostics
 	ota (67), -- OTA upgrade
	osc (68), -- Send to car
	vun (69), -- Vehicle unlock notification
	dkb (80), -- Digital key booking
	bks (89), -- Blue tooth key sharing
	bdc (90), -- Big Data collect
	
	wcr (71), -- windows close reminder
	rsv (72), -- Remote seat ventilate
	

	mcp (101), -- Certificate provision
	msr (102), -- message sequence reset
	fota (200),  -- FOTA upgrade
	rct (201), -- Remote Charge location and timer
	vdr (202), -- Vin Discrepancy report
	rdo (203), -- Remote open trunk/hood/Tank Flag/Charge Lid
	rdc (204), -- Remote close trunk/hood/Tank Flag/Charge Lid
	sra (205), -- Remote Service Reminder
	rcr (206), -- Remote Connectivity Report
	rfd (207), -- Remote fragrance dispensing
	rqt (208), -- Remote Query towing status
	rcs (209), -- Remote charge settings
	uvs (210), -- Remote update vehicle settings
	rwr (211), -- Remote wake up/sleep TEM
	rsm (212), -- Remote switch management
	aic (230), -- Artificial Intelligence Control 
	dvr (240), -- Digital video recorder
	avm (241), -- Around view monitor
	rvc (250),  -- RVDC
	
	pvs (300), -- Part of vehicle status
	rms (500) -- Remote monitor service
EOF

function print_bare_key_item
{
    local SERVICE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "${CAPTITAL_PER_KEY},"
}

function print_enum_key_item
{
    local SERVICE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "${CAPTITAL_PER_KEY},"
}

function print_full_enum_key_item
{
    local SERVICE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "lcfg::ConfigResource::${CAPTITAL_PER_KEY},"
}

function print_service_value_table_item
{
    local SERVICE
    local VALUE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"
    VALUE="$2"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "{ \"${SERVICE}\", ${VALUE} },"
}

function print_value_key_table_item
{
    local SERVICE
    local VALUE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"
    VALUE="$2"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "{ ${VALUE}, \"${SERVICE}\", lcfg::ConfigResource::${CAPTITAL_PER_KEY} },"
}

function print_xml_persistency_item
{
    local SERVICE
    
    SERVICE="$1"

    echo "    <resource>"
    echo "        <res_type>key-value</res_type>"
    echo "        <name>svc_prvn_${SERVICE}_enabled</name>"
    echo "        <application>ServiceProvision</application>"
    echo "        <storage_type>public</storage_type>"
    echo "        <permission>rw</permission>"
    echo "        <policy>cached</policy>"
    echo "        <user>root</user>"
    echo "        <group>root</group>"
    echo "        <data_type>integer</data_type>"
    echo "        <default>1</default>"
    echo "        <default_size>4</default_size>"
    echo "        <max_data_size>4</max_data_size>"
    echo "    </resource>"
}

function print_hpp_persistency_item
{
    local SERVICE
    local PER_KEY
    local CAPTITAL_PER_KEY
    
    SERVICE="$1"

    PER_KEY="svc_prvn_${SERVICE}_enabled"
    CAPTITAL_PER_KEY="`echo \"${PER_KEY}\" | tr [a-z] [A-Z]`"
    
    echo "    std::pair<ConfigResource,ResourceKeysInfo>(ConfigResource::${CAPTITAL_PER_KEY}, {\"ServiceProvision\", \"${PER_KEY}\",E_PCL_LDBID_PUBLIC,  E_PCL_DATA_TYPE_INTEGER,    CachedResource::NotCached, false}),"
}

function main
{
    local ARG_NUM
    local ARG1
    local SERVICE_NAME_ARRARY
    local SERVICE_VALUE_ARRARY
    local SERVICE
    local VALULE
    
    ARG_NUM=$#
    ARG1="$1"

    SERVICE_NAME_ARRARY=(`cat ${SERVICE_INFO_FILE} | tr [\(\)] [\ \ ] | awk '{ print $1 }'`)
    SERVICE_VALUE_ARRARY=(`cat ${SERVICE_INFO_FILE} | tr [\(\)] [\ \ ] | awk '{ print $2 }'`)
    
    if [ ${#SERVICE_NAME_ARRARY[@]} -ne ${#SERVICE_VALUE_ARRARY[@]} ]; then
        echo "*** ERROR: the number of service name is not equal to the number of service value!" >&2
        exit 1
    fi
    #echo "len of SERVICE_NAME_ARRARY: ${#SERVICE_NAME_ARRARY[@]}" >&2
    
    for ((i=0;i<${#SERVICE_NAME_ARRARY[@]};i++));do
        VALUE=${SERVICE_VALUE_ARRARY[$i]}
        SERVICE_VALUE_ARRARY[$i]=${VALUE##0}
    done

    for ((i=0;i<${#SERVICE_NAME_ARRARY[@]};i++));do
        SERVICE=${SERVICE_NAME_ARRARY[$i]}
        VALUE=${SERVICE_VALUE_ARRARY[$i]}
    
        if [ $ARG_NUM -ge 1 ]; then
            case "$ARG1" in
            bare_key*)
                print_bare_key_item $SERVICE
                ;;
            enum_key*)
                print_enum_key_item $SERVICE
                ;;
            full_enum_key*)
                print_full_enum_key_item $SERVICE
                ;;
            service_value_table*)
                print_service_value_table_item $SERVICE $VALUE
                ;;
            value_key_table*)
                print_value_key_table_item $SERVICE $VALUE
                ;;
            xml_per*)
                print_xml_persistency_item $SERVICE
                ;;
            hpp_per*)
                print_hpp_persistency_item $SERVICE
                ;;
            *)
                echo "*** ERROR: unknown type!" >&2
                exit 1
                ;;
            esac
        else
            echo "$SERVICE $VALULE"
        fi
    done
}

main $@
