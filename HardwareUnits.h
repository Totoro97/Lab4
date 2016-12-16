//------------------------------------FETCH--------------------------------------------------------

struct Align_ {
	int rA, rB, valC;
	void Proc(int rA_, int rB_, int valC_, int NeedRegids) {
		if (NeedRegids) rA = rA_, rB = rB_;
		else rA = rB = RNONE;
		valC = valC_;
	}
};

struct Instructions_ {
	int A[6];
	int icode, ifun, rA, rB, valC;
	int imem_error;	
	int Max = 2500;
	void Proc(int *COMMAND, int now) {
		//printf("first:%d\n",COMMAND[0]);
		for (int i = 0; i < 6; i++) A[i] = COMMAND[i];
		if (now > Max) { imem_error = 1; return; }
		icode = A[0] >> 4;
		ifun = A[0] & 0xF;
		rA = rB = RNONE; valC = 0;
		if (set<int> { IHALT, INOP, IRET }.count(icode)) return; 
		else if (set<int> { IJXX, ICALL }.count(icode)) {
			valC = A[1] + (A[2] << 8) + (A[3] << 16) + (A[4] << 24);
			return;
		} else if (set<int> { IRRMOVL, IOPL, IPUSHL, IPOPL }.count(icode)) {
			rA = A[1] >> 4;
			rB = A[1] & 0xF;			
		} else {
			rA = A[1] >> 4;
			rB = A[1] & 0xF;
			valC = A[2] + (A[3] << 8) + (A[4] << 16) + (A[5] << 24);
		}
		return;
	}
};

struct PCinc_ {
	int valP;
	int Get() { return valP; }
	void Proc(int f_pc, int *COMMAND) {
		int s = f_pc;
		int icode = COMMAND[s] >> 4;
		if (COMMAND[s] == -1) valP = 0;
		else if (set<int>{ INOP, IHALT, IRET }.count(icode)) valP = 1;
		else if (set<int>{ IIRMOVL, IRMMOVL, IMRMOVL }.count(icode)) valP = 6;
		else if (set<int>{ IJXX, ICALL }.count(icode)) valP = 5;
		else valP = 2;
		valP += s;
		//printf("[PCinc_valP:%d]",valP);
		return;
	}
};

struct Split_ {
	int icode, ifun;
	void Proc(int icode_, int ifun_ ) {
		icode = icode_;
		ifun = ifun_;
	}
};

//------------------------------------DECODE--------------------------------------------------------

struct RegisterFiles_ {
	int val[8];
	int d_rvalA, d_rvalB;

	void Print() {
		printf("\"RegisterFiles\" : { ");
		printf("\"eax\" : %d, ",val[0]);
		printf("\"ecx\" : %d, ",val[1]);
		printf("\"edx\" : %d, ",val[2]);
		printf("\"ebx\" : %d, ",val[3]);
		printf("\"esp\" : %d, ",val[4]);
		printf("\"ebp\" : %d, ",val[5]);
		printf("\"esi\" : %d, ",val[6]);
		printf("\"edi\" : %d ",val[7]);
		printf("}\n");
	}
	
	void Proc(int srcA, int srcB) {
		if (srcA != RNONE) d_rvalA = val[srcA];
		if (srcB != RNONE) d_rvalB = val[srcB];
	}
	
	void Write(int dstM, int M, int dstE, int E, int W_icode) {
		if (!set<int> { IMRMOVL, IIRMOVL, IRRMOVL, IPOPL, IRET, IOPL, IPUSHL, ICALL }.count(W_icode))
			return;
		// if (dstM != RNONE && dstE != RNONE) ???
		if (dstM != RNONE) val[dstM] = M;
		if (dstE != RNONE) val[dstE] = E;
	}
};

//------------------------------------EXECUTE--------------------------------------------------------

struct ALU_ {
	int valE;
	int ZF, SF, OF;
	void Proc(int valA, int valB, int alu_fun) {
		ZF = SF = OF = 0;
		if (alu_fun == 0) {
			valE = valB + valA;
			OF = (valA < 0 && valB < 0 && valE > 0) || (valA > 0 && valB > 0 && valE < 0);
		} else if (alu_fun == 1) {
			valA = -valA;
			valE = valB + valA;
			OF = (valA < 0 && valB < 0 && valE > 0) || (valA > 0 && valB > 0 && valE < 0) || (valA == (1 << 31));
		} else if (alu_fun == 2) {
			valE = valB & valA;
		} else {
			valE = valB ^ valA;
		}
		SF = (valE < 0);
		ZF = (valE == 0);
	}
};

struct CC_ {
	int le, l, e, ne, ge, g;
	void Proc(int ZF, int SF, int OF, int SetCC) {
		if (!SetCC) { return; }
		le = (SF ^ OF) | ZF;
		l = (SF ^ OF);
		e = ZF;
		ne = !ZF;
		ge = !(SF ^ OF);
		g = (!(SF ^ OF)) & (!ZF);
	}
};

struct cond_ {
	int val;
	int Get() { return val; }
	void Proc(int E_ifun, int le, int l, int e, int ne, int ge, int g) {
		if (E_ifun == 0) val = 1;
		else if (E_ifun == 1) val = le;
		else if (E_ifun == 2) val = l;
		else if (E_ifun == 3) val = e;
		else if (E_ifun == 4) val = ne;
		else if (E_ifun == 5) val = ge;
		else if (E_ifun == 6) val = g;
	}
};

//------------------------------------MEMORY--------------------------------------------------------

struct Datamemory_ {
	int Max = 2500;
	int dmem_error;
	int Val[3010];
	int val;
	int Get() { return val; }
	void Proc(int Memread, int Memwrite, int Addr, int M_valA) {
		if (Addr < 0 || Addr > Max) { dmem_error = 1; return; }		
		if (Memwrite) {
			Val[Addr] = M_valA & 0xFF;
			Val[Addr + 1] = (M_valA >> 8) & 0xFF;
			Val[Addr + 2] = (M_valA >> 16) & 0xFF;
			Val[Addr + 3] = (M_valA >> 24) & 0xFF;
		}
		if (Memread) {
			val = Val[Addr] + (Val[Addr + 1] << 8) + (Val[Addr + 2] << 16) + (Val[Addr + 3] << 24);
		}
	}
};
