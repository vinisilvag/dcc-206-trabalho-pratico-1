#!/bin/bash
make generator || exit 12312312312
for ((i = 1; ; i += 1)) do
  ./generator $i > input  
  ./executavel_test  < input > output1
  ./tp01 < input > output2
  diff output1 output2 || exit 1
  echo $i
done
