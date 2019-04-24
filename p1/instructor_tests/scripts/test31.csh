#! /bin/csh -f

echo "echo hello" > in.txt

echo "echo hello" > results/31.out
echo hello >> results/31.out

rm -f results/31.err
touch results/31.err

echo 0 > results/31.status


./mysh in.txt
