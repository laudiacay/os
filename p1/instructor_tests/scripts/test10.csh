#! /bin/csh -f

echo "This verifies one of the assumptions of this project, that your shell checks for a line of length 256 characters. Let's see whether your programs holds this basic assumption. What I do after this is to check the length of this line. Opoos! it would be over 256 characters. Go Go Go." > in.txt
echo "cal" >> in.txt


echo "This verifies one of the assumptions of this project, that your shell checks for a line of length 256 characters. Let's see whether your programs holds this basic assumption. What I do after this is to check the length of this line. Opoos! it would be over 256 characters. Go Go Go." > results/10.out

echo "cal" >> results/10.out
cal >> results/10.out

echo "An error has occurred" > results/10.err
echo 0 > results/10.status

./mysh in.txt
