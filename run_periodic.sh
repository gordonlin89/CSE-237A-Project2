#!/system/bin/sh

echo "bzip2"
./periodic.out -exec "./bzip2/bzip2_base.arm-gcc42-05 ./bzip2/dryer.jpg 2 > ./bzip2/dryer.jpg.out 2>> ./bzip2/dryer.jpg.err" > bzip2_util.csv

echo gobmk
./periodic.out -exec "./gobmk/gobmk_base.arm-gcc42-05 --quiet --mode gtp < ./gobmk/dniwog.tst > ./gobmk/dniwog.out 2>> ./gobmk/dniwog.err" > gobmk_util.csv

echo hmmer
./periodic.out -exec "./hmmer/hmmer_base.arm-gcc42-05 --fixed 0 --mean 325 --num 45000 --sd 200 --seed 0 ./hmmer/bombesin.hmm > ./hmmer/bombesin.out 2>> ./hmmer/bombesin.err" > hmmer_util.csv

echo mcf
./periodic.out -exec "./mcf/mcf_base.arm-gcc42-05 ./mcf/inp_test.in > ./mcf/inp.out 2>> ./mcf/inp.err" > mcf_util.csv

echo sjeng
./periodic.out -exec "./sjeng/sjeng_base.arm-gcc42-05 ./sjeng/test.txt > ./sjeng/test.out 2>> ./sjeng/test.err" > sjeng_util.csv


