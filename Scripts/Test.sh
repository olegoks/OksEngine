#!/bin/bash
set -x
cd ../

cd ./Build

# rm ./Testing/Temporary/LastTestsFailed.log

ctest --output-on-failure -C Debug

# echo $?
# value=`cat ./Testing/Temporary/LastTestsFailed.log`
# echo "$value"


# read -p "Press enter to continue"
if [$? -ne 0]
then
    echo "UNIT TESTS WERE NOT PASSED!" 
    exit -1
fi

cd ..
cd Scripts/

