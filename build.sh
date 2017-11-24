
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/command_line.c -o obj/Debug/command_line.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/file_system.c -o obj/Debug/file_system.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/main.c -o obj/Debug/main.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/copy_tests.c -o obj/Debug/test/copy_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/create_tests.c -o obj/Debug/test/create_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/delete_tests.c -o obj/Debug/test/delete_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/file_name_tests.c -o obj/Debug/test/file_name_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/init_tests.c -o obj/Debug/test/init_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/load_test.c -o obj/Debug/test/load_test.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/read_tests.c -o obj/Debug/test/read_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/rename_tests.c -o obj/Debug/test/rename_tests.o
gcc -Wno-unused-result -g  -c /home/alex/CProjects/OS_4/test/write_tests.c -o obj/Debug/test/write_tests.o
g++  -o FSEmulATOR obj/Debug/command_line.o obj/Debug/file_system.o obj/Debug/main.o obj/Debug/test/copy_tests.o obj/Debug/test/create_tests.o obj/Debug/test/delete_tests.o obj/Debug/test/file_name_tests.o obj/Debug/test/init_tests.o obj/Debug/test/load_test.o obj/Debug/test/read_tests.o obj/Debug/test/rename_tests.o obj/Debug/test/write_tests.o 