#!/bin/bash
set -xv

echo "Usage:"$0" HOST_EXECUTION_DIR TARGET_VIRTUAL_DIR KEYSTORE_DIR PRIVAT_KEYSTORE_DIR"
echo ""
#cp $0 ../../..

UNITTEST_VOC="../VolvoOnCall/test/unittest/src/unittest_voc"
HOST_BUILDROOT_DIR="../../../release/images/devel/2K"

#TARGET executions vars
if [ -z "$1" ]; then
    HOST_EXECUTION_DIR="."
else
    HOST_EXECUTION_DIR="$1"
fi
echo "HOST_EXECUTION_DIR set to $HOST_EXECUTION_DIR"

#TARGET executions vars
if [ -z "$2" ]; then
    TARGET_VIRTROOT_DIR="."
else
    TARGET_VIRTROOT_DIR="$2"
fi
echo "TARGET_VIRTROOT_DIR set to $TARGET_VIRTROOT_DIR"

#KEYSTORE_DIR var default execution dir is
if [ -z "$3" ]; then
    KEYSTORE_DIR="./test/unittest/ccm/pems"
else
    KEYSTORE_DIR="$3"
fi
echo "KEYSTORE_DIR set is $KEYSTORE_DIR"

if [ -z "$4" ]; then
    PRIVAT_KEYSTORE_DIR="."
else
    PRIVAT_KEYSTORE_DIR="$4"
fi
echo "PRIVAT_KEYSTORE_DIR set is $PRIVAT_KEYSTORE_DIR"

TARGET_TELEMATIC_DIR="/data/telematics"
TARGET_TMP_DIR="/data/tmp"
TARGET_VOC_TEST_DIR="/data"
TARGET_CARDOMAIN_DIR="/data/certs/cardomain"

echo "Creating $TARGET_TELEMATIC_DIR"
adb shell mkdir $TARGET_TELEMATIC_DIR

echo "Push-ing tcam.config to $TARGET_TELEMATIC_DIR"
adb push $HOST_EXECUTION_DIR/service_discovery/mock_cloud/tcam.config $TARGET_TELEMATIC_DIR/tcam.config

if [ -e  $HOST_BUILDROOT_DIR/tp-pers-install.tar.gz ]; then
    echo "Push-ing tp-pers-install.tar.gz to $TARGET_TMP_DIR"
    adb push $HOST_BUILDROOT_DIR/tp-pers-install.tar.gz $TARGET_TMP_DIR/tp-pers-install.tar.gz

    echo "pers-resource-installer-ing tp-pers-install.tar.gz as $TARGET_TMP_DIR/tp-pers-install.tar.gz"
    adb shell pers-resource-installer $TARGET_TMP_DIR/tp-pers-install.tar.gz

fi

if [ -e $UNITTEST_VOC ]; then
    echo "Push-ing unittest_voc to $TARGET_VOC_TEST_DIR"
    adb push $UNITTEST_VOC $TARGET_VOC_TEST_DIR/unittest_voc
    echo "Chmod-ing unittest_voc to 777"
    adb shell chmod 777 $TARGET_VOC_TEST_DIR/unittest_voc
fi

echo "Creating $TARGET_CARDOMAIN_DIR"
adb shell mkdir -p $TARGET_CARDOMAIN_DIR

echo "Push-ing $KEYSTORE_DIR/ca.cert.pem to $TARGET_CARDOMAIN_DIR/car_root_cert_pem.txt"
adb push $KEYSTORE_DIR/ca.cert.pem $TARGET_CARDOMAIN_DIR/car_root_cert_pem.txt

echo "Push-ing $KEYSTORE_DIR/tcam.cert.pem to $TARGET_CARDOMAIN_DIR/mclient_cert_pem.txt"
adb push $KEYSTORE_DIR/tcam.cert.pem $TARGET_CARDOMAIN_DIR/mclient_cert_pem.txt

echo "Push-ing $KEYSTORE_DIR/tcam.key.pem to $TARGET_CARDOMAIN_DIR/mclient_key_pem.txt"
adb push $KEYSTORE_DIR/tcam.key.pem $TARGET_CARDOMAIN_DIR/mclient_key_pem.txt

echo "Push-ing $KEYSTORE_DIR/client.key.pem to $TARGET_CARDOMAIN_DIR/car_key_pem.txt"
adb push $KEYSTORE_DIR/client.key.pem $TARGET_CARDOMAIN_DIR/car_key_pem.txt

echo "Push-ing $KEYSTORE_DIR/ca.cert.pem to $TARGET_CARDOMAIN_DIR/car_cert_pem.txt"
adb push $KEYSTORE_DIR/client.cert.pem $TARGET_CARDOMAIN_DIR/car_cert_pem.txt

echo "Push-ing $PRIVAT_KEYSTORE_DIR/ca_certs_TCAMCONTISPASE003_20160705_vcc_6be2555e1b233148.txt to $TARGET_TELEMATIC_DIR/keys/CAinfo.txt"
adb push $PRIVAT_KEYSTORE_DIR/ca_certs_TCAMCONTISPASE003_20160705_vcc_6be2555e1b233148.txt  $TARGET_TELEMATIC_DIR/keys/CAinfo.txt
echo "Push-ing $PRIVAT_KEYSTORE_DIR/cert_TCAMCONTISPASE003_20160705_vcc_6be2555e1b233148.cer to $TARGET_TELEMATIC_DIR/keys/ccCert.cer"
adb push $PRIVAT_KEYSTORE_DIR/cert_TCAMCONTISPASE003_20160705_vcc_6be2555e1b233148.cer $TARGET_TELEMATIC_DIR/keys/ccCert.cer
echo "Push-ing $PRIVAT_KEYSTORE_DIR/client.key to $TARGET_TELEMATIC_DIR/keys/ccCert.cer"
adb push $PRIVAT_KEYSTORE_DIR/client.key $TARGET_TELEMATIC_DIR/keys/ccCert.key

set +xv
