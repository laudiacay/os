#! /bin/csh -f

# set tmpFile = `mktemp`
#echo tmpFile
# grep 'include' /u/c/s/cs537-3/test/p2/para.c | head -2

set tmpFile = fileout
set resultDir = results
set test = 24

echo "grep 'include' para.c | head -2" > in.txt

rm -f ${resultDir}/${test}.out
echo "grep 'include' para.c | head -2" > ${resultDir}/${test}.out
grep 'include' para.c | head -2 >> ${resultDir}/${test}.out

rm -f ${resultDir}/${test}.err
touch ${resultDir}/${test}.err
echo 0 > ${resultDir}/${test}.status

./mysh in.txt
