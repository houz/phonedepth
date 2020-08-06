#!/usr/bin/env bash

root_folder="$PWD"
base_folder=$(realpath "$1")

COLOR_RED="\033[0;31m"
COLOR_GREEN="\033[0;32m"
COLOR_NONE="\033[0m"

if [[ -z "${base_folder}" ]]; then
  echo "Usage: $0 <base folder>"
  exit 1
fi

if [[ ! -d "${base_folder}" ]]; then
  echo "'${base_folder}' is not a directory"
  exit 1
fi

cd "${base_folder}"

echo "camera;image;success"

total=0
failed=0

for folder in */; do
  cd "${folder}"
  folder=${folder%/}
  for image in *; do
    output_folder="${root_folder}/test_results/${folder}/${image}"
    mkdir -p "${output_folder}"
    cd "${output_folder}"
    "${root_folder}/extract_depthmap" "${base_folder}/${folder}/${image}" > result.txt 2>&1
    if [[ $? == 0 ]]; then
      res=1
      echo -en "${COLOR_GREEN}"
    else
      res=0
      echo -en "${COLOR_RED}"
    fi
    echo "$folder;$image;$res"
    echo -en "${COLOR_NONE}"
    total=$((total+1))
    failed=$((failed+res))
    cd "${OLDPWD}"
  done
  cd ..
done

echo
echo -e "${COLOR_RED}${failed}${COLOR_NONE}/${COLOR_GREEN}${total}${COLOR_NONE} succeeded"
