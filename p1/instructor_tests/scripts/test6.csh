#! /bin/csh -f

echo "quit" > in.txt

echo "quit" > results/6.out
touch results/6.err
echo 0 > results/6.status

#(./myprog in.txt > results/6.out) >& results/6.err
#echo $status > results/6.status

./mysh in.txt
 
