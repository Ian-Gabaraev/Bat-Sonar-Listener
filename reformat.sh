 #/bin/bash
make format && clang-format -i $(find . -name "*.c" -o -name "*.h")