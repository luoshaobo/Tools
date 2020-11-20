#  install :
#      how to install and use google test in ubuntu
#      please reference link: http://buic-confluence.conti.de:8090/display/GST/Using+googletest+to+do+unittest
#  clean test tmp files :
#      ./gCoverage.sh clean
#  run test and generate coverage file '${gTestResultFolder}/result_html/index.html' :
#      ./gCoverage.sh
#
#  2019-02-28 jianhui.li

# project name, used to output message only
project="VolvoOnCall"

# build succesd unit test program
testProgram="./unittest_voc"

# google test result folder
gTestResultFolder="./gTestResult"

# the file used by google test coverage
gTestXML="./unittest_voc.xml"

build_unittest_xxx="../build_unittest_voc/CMakeFiles/unittest_voc.dir/"

if [ "$1" == "clean" ]
then
  if [ -d "${gTestResultFolder}" ];then
    rm -rf ${gTestResultFolder}
  fi

  if [ -f "${gTestXML}" ];then
    rm -f ${gTestXML}
  fi
  exit 0
fi

# rm gTestResultFolder
if [ -d ${gTestResultFolder} ];then
  rm -rf ${gTestResultFolder}
fi

mkdir ${gTestResultFolder}
export GCOV_PREFIX=${gTestResultFolder}
export GCOV_PREFIX_STRIP=100

if [ -f "${gTestXML}" ];then
  rm -f ${gTestXML}
fi

if [ ! -f "${testProgram}" ];then
  echo "Run unit test converage failed. Please run make cmd to gererate '${testProgram}'. "
  exit 1
fi

# run test program
${testProgram}  --gtest_output=xml:${gTestXML}
# check test if run failed
if [ $? -eq 0 ]; then
  echo " === '${project}' run unit test '${testProgram}' succeed ==="
else
  echo " === '${project}' unit test '${testProgram}' failed ==="
  exit 1
fi

cd ${gTestResultFolder}
cp $(find ${build_unittest_xxx} -type f -name "*.gcno") ./
lcov -d . -t result_ut -o result_ut.info -b . -c --rc lcov_branch_coverage=1
genhtml --branch-coverage -o result_html result_ut.info

echo "google test coverage result output to '${gTestResultFolder}/result_html/index.html'"

exit 0;


