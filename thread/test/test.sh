#!/bin/bash
TEST_FILE_PATH="input/"
OUTPUT_PATH="output/"
RIGHT_PATH="right/"
ROOT_PATH=$(pwd)/test/
echo ${ROOT_PATH}

let zero=0
let index=0
let FAIL_INDEX=0
let SUCCESS_INDEX=0

for m in $(ls ${ROOT_PATH}/${TEST_FILE_PATH})
do
    let index+=1
    echo "正在运行第${index}个测试样例..."
    ./multisum ${ROOT_PATH}/${TEST_FILE_PATH}/input${index}.txt ${ROOT_PATH}${OUTPUT_PATH}/output${index}.txt

    diff ${ROOT_PATH}/${OUTPUT_PATH}/output${index}.txt ${ROOT_PATH}/${RIGHT_PATH}/right${index}.txt
    let state=$?
	if [[ ${state} -eq  ${zero} ]]
    then
        SUCCESS[${SUCCESS_INDEX}]=${index}
        let SUCCESS_INDEX+=1
    else
        FAIL[${FAIL_INDEX}]="${index}"
        let FAIL_INDEX+=1
    fi
done
# output test result
if [[ ${FAIL_INDEX} -eq  ${zero} ]]
then
    echo "${SUCCESS_INDEX}个样例全部通过"
else
    echo "测试成功样例: ${SUCCESS[@]}"
    echo "测试失败样例: ${FAIL[@]}"
fi

