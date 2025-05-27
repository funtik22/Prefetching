mkdir -p ./build
mkdir -p ./traces

g++ -g -static -O0 src/memcpy_example.cpp -o ./build/memcpy_example
valgrind --tool=lackey --trace-mem=yes ./build/memcpy_example 2>&1 \
  | awk '/MEMCPY_START/{flag=1; next} /MEMCPY_END/{flag=0} flag' \
  > ./traces/memcpy_trace.log

g++ -g -static -O0 src/linkedList_example.cpp -o ./build/linkedList_example
valgrind --tool=lackey --trace-mem=yes ./build/linkedList_example 2>&1 \
  | awk '/LINKED_START/{flag=1; next} /LINKED_END/{flag=0} flag' \
  > ./traces/linkedList_trace.log
