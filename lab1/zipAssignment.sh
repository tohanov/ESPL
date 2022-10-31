mkdir task1C task2A task2B

cp "./task1/1c/makefile" "task1C/"
cp "./task1/1c/encoder.c" "task1C/"


cp "./task2/2a/makefile" "task2A/"
cp "./task2/2a/encoder.c" "task2A/"


cp "./task2/2b/makefile" "task2B/"
cp "./task2/2b/encoder.c" "task2B/"

zip -r "lab1.zip" "task1C/" "task2A/" "task2B/"

rm -rf "task1C/" "task2A/" "task2B/"
