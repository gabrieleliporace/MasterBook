#! /bin/bash

ret=0
red='\033[0;31m'
green='\033[0;32m'
clear='\033[0m'

for ((i=0;i<1000;i++))
do
./"$1" > /tmp/"$1".tmp

if [ $? -ne 0 ]
    then
        
        echo -e ${red}$i "TEST FAILED"${clear}
        exit 1;
fi
echo $i
done

echo -e ${green}"TEST PASSED"${clear}

exit 0;

