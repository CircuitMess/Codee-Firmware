#!/bin/bash

set -e
shopt -s globstar
shopt -s extglob

for arch in spiffs_archives/*; do
  name=$(basename $arch)

  data="spiffs_image/$name.sz"
  rm -f $data

  files=($arch/**/*)

  count=0
  for file in ${files[@]}; do
      if [[ -f $file ]]; then
        count=$((count+1))
      fi
  done

  printf "0: %.8x" $count | sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r -g0 >> $data

  for file in ${files[@]}; do
    if [[ -d $file ]]; then continue; fi

    name=$(sed 's/spiffs_archives\/[a-zA-Z0-9]*//' <<< $file)
    size=$(du -b $file | cut -f1)

    printf "%s\0" $name >> $data
    printf "0: %.8x" $size | sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r -g0 >> $data
  done

  printf "\0" >> $data

  for file in ${files[@]}; do
    if [[ -d "$file" ]]; then continue; fi

    cat $file >> $data
  done
done
