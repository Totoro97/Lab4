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
	void Proc(int *COMMAND, int Isbubble) {
		printf("first:%d\n",COMMAND[0]);
		if (Isbubble) {
			icode = IBUBBLE;
			ifun = rA = rB = valC = 0;
			return;
		}
		for (int i = 0; i < 6; i++) A[i] = COMMAND[i];
		if (A[0] == -1) { imem_error = 1; return; }
		icode = A[0] >> 4;
		ifun = A[0] & 0xF;
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
	void Proc(int f_pc, int *COMMAND, int Isbubble) {
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
		else d_rvalA = 0;
		if (srcB != RNONE) d_rvalB = val[srcB];
		else d_rvalB = 0;
	}
	
	void Write(int dstM, int M, int dstE, int E) {
		if (dstM != RNONE) val[dstM] = M;
		if (dstE != RNONE) val[dstE] = E;
	}
};

//------------------------------------EXECUTE--------------------------------------------------------

struct ALU_ {
	int valE;
	int ZF, SF, OF;
	void Proc(int valA, int valB, int alu_fun, int SetCC, int ZF_, int SF_, int OF_) {
		ZF = SF = OF = 0;
		if (alu_fun == 0) {
			valE = valA + valB;
			if (SetCC) OF = (valA < 0 && valB < 0 && valE > 0) || (valA > 0 && valB > 0 && valE < 0);
		} else if (alu_fun == 1) {
			valB = -valB;
			valE = valA + valB;
			if (SetCC) OF = (valA < 0 && valB < 0 && valE > 0) || (valA > 0 && valB > 0 && valE < 0) || (valB == (1 << 31));
		} else if (alu_fun == 2) {
			valE = valA & valB;
		} else {
			valE = valA ^ valB;
		}
		if (SetCC) SF = (valE < 0);
		if (SetCC) ZF = (valE == 0);
		if (!SetCC) ZF = ZF_, SF = SF_, OF = OF_;
	}
};

struct CC_ {
	int le, l, e, ne, ge, g;
	void Proc(int ZF, int SF, int OF, int SetCC) {
		//if (!SetCC) { le = l = e = ne = ge = g = 0; return; }
		le = (SF ^ OF) | ZF;
		l = (SF ^ OF);
		e = ZF;
		ne = !ZF;
		ge = !(SF ^ OF);
		g = !(SF ^ OF) & (!ZF);
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
	int Max = 10000;
	int dmem_error;
	int Val[3010];
	int val;
	int Get() { return val; }
	void Proc(int Memread, int Memwrite, int Addr, int M_valA) {
		if (Addr < 0 || Addr > Max) { dmem_error = 1; return; }
		Addr /= 4;
		if (Memread)
			Val[Addr] = M_valA;
		else 
			val = Val[Addr];
	}
};