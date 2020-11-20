#!/bin/bash
SSM_PERSISTENCE_XML_IN="persistence_config_fsm_ssm.xml"
SSM_PERSISTENCE_XML_GZ_OUT="persistence_config_fsm_ssm.tar.gz"
echo "SSM_PERSISTENCE_XML_IN="$SSM_PERSISTENCE_XML_IN "SSM_PERSISTENCE_XML_GZ_OUT="$SSM_PERSISTENCE_XML_GZ_OUT
echo "Tar-ring xml..."
tar czf $SSM_PERSISTENCE_XML_GZ_OUT $SSM_PERSISTENCE_XML_IN
echo "Pushing gz to target..."
adb push $SSM_PERSISTENCE_XML_GZ_OUT /tmp/$SSM_PERSISTENCE_XML_GZ_OUT
echo "Execute pers-resource-installer on the target..."
adb shell pers-resource-installer /tmp/$SSM_PERSISTENCE_XML_GZ_OUT
