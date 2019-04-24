#! /bin/csh -f

# set tmpFile = `mktemp`
#echo tmpFile

# set tmpFile = fileout
set resultDir = results
set test = 27

echo "cal || uniq" > in.txt
echo "cal || uniq" > ${resultDir}/${test}.out

rm -f ${resultDir}/${test}.err
echo "An error has occurred" > ${resultDir}/${test}.err
echo 0 > ${resultDir}/${test}.status

./mysh in.txt
