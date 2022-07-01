#!/usr/bin/env bats

load test_helper

@test "we_forky" {
	run bash -c "test -f tests/forky || gcc tests/forky.c -o tests/forky"
	run ./echelon tests/forky tests/forky tests/forky tests/forky tests/forky tests/forky tests/forky
	checki 0 <<FIN
6
7
FIN
}

@test "i_exy" {
	run bash -c "test -f tests/exy || gcc tests/exy.c -o tests/exy"
	run ./echelon tests/exy
	checki 0 <<FIN
0
1
FIN
}

@test "we_exy" {
	run bash -c "test -f tests/exy || gcc tests/exy.c -o tests/exy"
	run ./echelon tests/exy tests/exy tests/exy tests/exy tests/exy tests/exy tests/exy
	checki 0 <<FIN
0
7
FIN
}

@test "sigfpe" {
	run bash -c "test -f tests/sigfpe || gcc tests/sigfpe.c -o tests/sigfpe"
	run ./echelon tests/sigfpe
	checki 136 <<FIN
0
1
FIN
}

@test "threads_nada" {
	run bash -c "test -f tests/threads || gcc tests/threads.c -o tests/threads -pthread"
	run ./echelon tests/threads
	checki 1 <<FIN
0
1
FIN
}
