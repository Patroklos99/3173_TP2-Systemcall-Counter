#!/usr/bin/env bats

load test_helper

@test "date" {
	run ./echelon /bin/date +%Y
	checki 0 <<FIN
2022
0
1
FIN
}

@test "pipe" {
	run ./echelon /bin/bash -c "echo 'aucun test ne passe :(' | wc -c"
	checki 0 <<FIN
23
2
2
FIN
}

@test "exit" {
	run ./echelon /bin/bash -c "exit 31"
	checki 31 <<FIN
0
1
FIN
}

@test "i_forky" {
	run bash -c "test -f tests/forky || gcc tests/forky.c -o tests/forky"
	run ./echelon tests/forky
	checki 0 <<FIN
0
1
FIN
}

@test "sigsegv" {
	run bash -c "test -f tests/sigsegv || gcc tests/sigsegv.c -o tests/sigsegv"
	run ./echelon tests/sigsegv
	checki 139 <<FIN
0
1
FIN
}

@test "threads" {
	run bash -c "test -f tests/threads || gcc tests/threads.c -o tests/threads -pthread"
	run ./echelon tests/threads 13
	checki 13 <<FIN
13
1
FIN
}
