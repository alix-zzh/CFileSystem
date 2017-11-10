gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/command_line.c -o obj/Release/command_line.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/file_system.c -o obj/Release/file_system.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/main.c -o obj/Release/main.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/copy_tests.c -o obj/Release/test/copy_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/create_tests.c -o obj/Release/test/create_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/delete_tests.c -o obj/Release/test/delete_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/init_tests.c -o obj/Release/test/init_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/load_test.c -o obj/Release/test/load_test.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/read_tests.c -o obj/Release/test/read_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/rename_tests.c -o obj/Release/test/rename_tests.o
gcc -Wno-unused-result -O2  -c /home/alex/CProjects/OS_4/test/write_tests.c -o obj/Release/test/write_tests.o
g++  -o FSEmulATOR obj/Release/command_line.o obj/Release/file_system.o obj/Release/main.o obj/Release/test/copy_tests.o obj/Release/test/create_tests.o obj/Release/test/delete_tests.o obj/Release/test/init_tests.o obj/Release/test/load_test.o obj/Release/test/read_tests.o obj/Release/test/rename_tests.o obj/Release/test/write_tests.o  -s 
