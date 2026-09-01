	LOC #100
Main	IS @

	LOC Main
	SET $200,3
	SET $2,$200
	SET $201,7
	SET $3,$201
	CMP $4,$2,$3
	BP $4,L1
	SET $2,$3
L1	TRAP 0,Halt,0
