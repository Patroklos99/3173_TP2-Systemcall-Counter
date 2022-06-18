#!/usr/bin/env bats

load test_helper

@test "gratuit" {
	true
}

@test "Execve_clone_fork=0" {
	run ./echelon /bin/da
	checki 1 <<FIN
0
FIN
}

