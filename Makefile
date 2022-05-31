all: echelon

check: echelon
	bats check.bats
clean:
	rm echelon
