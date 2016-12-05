struct Instructions_ {
	int A[6];
	int icode, ifunc, rA, rB, valC;
	int imem_error;	
	void Proc(int *COMMAND) {
		for (int i = 0; i < 6; i++) A[i] = COMMAND[i];
		icode = A[0] >> 4;
		ifunc = A[0] & 0xF;
		if (icode == 0 || icode == 1) return;
		else if (icode == 7 || icode == 8) {
			valC = A[1] + A[2] << 8 + A[3] << 16 + A[4] << 24;
			return;
		} else {
			rA = A[1] >> 4;
			rB = A[1] & 0xF;
			valC = A[2] + A[3] << 8 + A[4] << 16 + A[5] << 24;
		}
		return;
	}
};