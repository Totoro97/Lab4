//------------------------------------FETCH--------------------------------------------------------

struct SelectPC_ {
	int ans;
	int Isbubble;
	int Get() { return ans; }
	void Proc(int D_icode, int E_icode, int PasSelect, int M_icode, int M_Cnd, int M_valE, int W_icode, int W_valM, int F_predPC) {
		Isbubble = (D_icode == IJXX || E_icode == IJXX);
		if (Isbubble) 
			ans = PasSelect;
		else if (M_icode == IJXX && M_Cnd)
			ans = M_valE;
		else if (W_icode == IRET)
			ans = W_valM;
		else
			ans = F_predPC;
		printf("Select:%d\n",ans);
	}
};

struct f_icode_ {
	int val;
	int Get() { return val; }
	void Proc(int imem_error, int imem_icode) {
		if (imem_error) val = INOP;
		else val = imem_icode;
	}
};

/*
HCL:
bool need_regids = f_icode in { IRRMOVL, IOPL, IPUSHL, IPOPL, IIRMOVL, IRMMOVL, IMRMOVL };
*/
struct Needregids_ {
	int val;
	int Get() { return val; }
	void Proc(int f_icode) {
		val = set<int> { IRRMOVL, IOPL, IPUSHL, IPOPL, IIRMOVL, IRMMOVL, IMRMOVL }.count(f_icode);
	}
};

/*HCL:
bool need_valC =
	f_icode in { IIRMOVL, IRMMOVL, IMRMOVL, IJXX, ICALL };
*/
struct NeedvalC_ {
	int val;
	int Get() { return val; }
	void Proc(int f_icode) {
		val = set<int>{ IIRMOVL, IRMMOVL, IMRMOVL, IJXX, ICALL }.count(f_icode);
	}
};
/*
HCL:
instrvalid = f_icode in 
	{ INOP, IHALT, IRRMOVL, IIRMOVL, IRMMOVL, IMRMOVL,
	  IOPL, IJXX, ICALL, IRET, IPUSHL, IPOPL };
*/

struct Instrvalid_ {
	int val;
	int Get() { return val; }
	void Proc(int f_icode) {
		val = set<int> { 
			INOP, IHALT, IRRMOVL, IIRMOVL, IRMMOVL, IMRMOVL,
	  	    IOPL, IJXX, ICALL, IRET, IPUSHL, IPOPL, IBUBBLE
		}.count(f_icode);		
	}
};

struct f_ifun_ {
	int val;
	int Get() { return val; }
	void Proc(int imem_error, int imem_ifun) {
		val = imem_error ? FNONE : imem_ifun;
	} 
};

struct PredictPC_ {
	int ans;
	int Get() { return ans; }
	void Proc(int f_icode, int f_valC, int f_valP) {
		if (f_icode == IJXX || f_icode == ICALL) ans = f_valC;
		else ans = f_valP;
	}
};

/*
HCL:
int f_stat = [
	imem_error: SADR;
	!instr_valid : SINS;
	f_icode == IHALT : SHLT;
	1 : SAOK;
];
*/

struct f_stat_ {
	int val;
	int Get() { return val; }
	void Proc(int f_icode, int instr_valid, int imem_error) {
		printf("f_icode:%d\n",f_icode);
		if (imem_error) val = SADR;
		else if (!instr_valid) val = SINS;
		else if (f_icode == IHALT) val = SHLT;
		else val = SAOK;
	}
};

//------------------------------------DECODE--------------------------------------------------------

/*
int d_dstE = [
	D_icode in { IRRMOVL, IIRMOVL, IOPL} : D_rB;
	D_icode in { IPUSHL, IPOPL, ICALL, IRET } : RESP;
	1 : RNONE;  # Don't write any register
];
*/

struct d_dstE_ {
	int val;
	int Get() { return val; }
	void Proc(int D_icode, int D_rB) {
		if (set<int>{ IRRMOVL, IIRMOVL, IOPL }.count(D_icode)) val = D_rB;
		else if (set<int> { IPUSHL, IPOPL, ICALL, IRET}.count(D_icode)) val = RESP;
		else val = RNONE;
	}
};

/*
int d_dstM = [
	D_icode in { IMRMOVL, IPOPL } : D_rA;
	D_icode in { } : REBP;
	1 : RNONE;  # Don't write any register
];
*/

struct d_dstM_ {
	int val;
	int Get() { return val; }
	void Proc(int D_icode, int D_rA) {
		if (set<int> { IMRMOVL, IPOPL }.count(D_icode)) val = D_rA;
		else val = RNONE;
	}
};

/*
int d_srcA = [
	D_icode in { IRRMOVL, IRMMOVL, IOPL, IPUSHL  } : D_rA;
	D_icode in { IPOPL, IRET } : RESP;
	D_icode in { } : REBP;
	1 : RNONE; # Don't need register
];
*/

struct d_srcA_ {
	int val;
	int Get() { return val; }
	void Proc(int D_icode, int D_rA) {
		if (set<int>{ IRRMOVL, IRMMOVL, IOPL, IPUSHL }.count(D_icode)) val = D_rA;
		else if (set<int> { IPOPL, IRET }.count(D_icode)) val = RESP;
		else val = RNONE;
	}
};

/*
int d_srcB = [
	D_icode in { IOPL, IRMMOVL, IMRMOVL  } : D_rB;
	D_icode in { IPUSHL, IPOPL, ICALL, IRET } : RESP;
	D_icode in { } : REBP;
	1 : RNONE;  # Don't need register
];
*/

struct d_srcB_ {
	int val;
	int Get() { return val; }
	void Proc(int D_icode, int D_rB) {
		if (set<int>{ IOPL, IRMMOVL, IMRMOVL }.count(D_icode)) val = D_rB;
		else if (set<int> { IPUSHL, IPOPL, ICALL, IRET }.count(D_icode)) val = RESP;
		else val = RNONE;
	}
};

/*int d_valA = [
	D_icode in { ICALL, IJXX } : D_valP; # Use incremented PC
	d_srcA == e_dstE : e_valE;    # Forward valE from execute
	d_srcA == M_dstM : m_valM;    # Forward valM from memory
	d_srcA == M_dstE : M_valE;    # Forward valE from memory
	d_srcA == W_dstM : W_valM;    # Forward valM from write back
	d_srcA == W_dstE : W_valE;    # Forward valE from write back
	1 : d_rvalA;  # Use value read from register file
];
*/

struct FwdA_ {
	int val;
	int Get() { return val; }
	void Proc(int D_icode, int D_valP, int d_srcA, int d_rvalA,
		int e_dstE, int e_valE, int M_dstE, int M_valE, int M_dstM, int m_valM, int W_dstM, int W_valM, int W_dstE, int W_valE) {
		printf("e_valE:%d\n",e_valE);
		if (D_icode == ICALL || D_icode == IJXX) val = D_valP;
		else if (d_srcA == e_dstE) val = e_valE;
		else if (d_srcA == M_dstM) val = m_valM;
		else if (d_srcA == M_dstE) val = M_valE;
		else if (d_srcA == W_dstM) val = W_valM;
		else if (d_srcA == W_dstE) val = W_valE;
		else val = d_rvalA;	
	}
};

/*
int d_valB = [
	d_srcB == e_dstE : e_valE;    # Forward valE from execute
	d_srcB == M_dstM : m_valM;    # Forward valM from memory
	d_srcB == M_dstE : M_valE;    # Forward valE from memory
	d_srcB == W_dstM : W_valM;    # Forward valM from write back
	d_srcB == W_dstE : W_valE;    # Forward valE from write back
	1 : d_rvalB;  # Use value read from register file
];
*/

struct FwdB_ {
	int val;
	int Get() { return val; }
	void Proc(int d_srcB, int d_rvalB,
		int e_dstE, int e_valE, int M_dstE, int M_valE, int M_dstM, int m_valM, int W_dstM, int W_valM, int W_dstE, int W_valE) {
		if (d_srcB == e_dstE) val = e_valE;
		else if (d_srcB == M_dstM) val = m_valM;
		else if (d_srcB == M_dstE) val = M_valE;
		else if (d_srcB == W_dstM) val = W_valM;
		else if (d_srcB == W_dstE) val = W_valE;
		else val = d_rvalB;
	}
};

//------------------------------------EXECUTE--------------------------------------------------------
/*
int aluA = [
	E_icode in { IRRMOVL, IOPL } : E_valA;
	E_icode in { IIRMOVL, IRMMOVL, IMRMOVL } : E_valC;
	E_icode in { ICALL, IPUSHL } : -4;
	E_icode in { IRET, IPOPL } : 4;
	# Other instructions don't need ALU
];
*/

struct ALUA_ {
	int val;
	int Get() { return val; }
	void Proc(int E_icode, int E_valC, int E_valA) {
		if (set<int>{ IRRMOVL, IOPL }.count(E_icode)) val = E_valA;
		else if (set<int>{ IIRMOVL, IRMMOVL, IMRMOVL, IJXX }.count(E_icode)) val = E_valC;
		else if (set<int>{ ICALL, IPUSHL }.count(E_icode)) val = -4;	
		else if (set<int>{ IRET, IPOPL }.count(E_icode)) val = 4;		
	}
};

/*
int aluB = [
	E_icode in { IRMMOVL, IMRMOVL, IOPL, ICALL, 
		     IPUSHL, IRET, IPOPL } : E_valB;
	E_icode in { IRRMOVL, IIRMOVL } : 0;
	# Other instructions don't need ALU
];
*/

struct ALUB_ {
	int val;
	int Get() { return val; }
	void Proc(int E_icode, int E_valB) {
		if (set<int>{ IRMMOVL, IMRMOVL, IOPL, ICALL, 
		     IPUSHL, IRET, IPOPL }.count(E_icode)) val = E_valB;
		else if (set<int> { IRRMOVL, IIRMOVL, IJXX }.count(E_icode)) val = 0;
	}
};

/*
int alufun = [
	E_icode == IOPL : E_ifun;
	1 : ALUADD;
];
*/

struct ALUfun_ {
	int val;
	int Get() { return val; }
	void Proc(int E_icode, int E_ifun) {
		if (E_icode == IOPL) val = E_ifun;
		else val = ALUADD;
	}
};

/*
int e_dstE = [
	E_icode == IRRMOVL && !e_Cnd : RNONE;
	1 : E_dstE;
];
*/

struct e_dstE_ {
	int val;
	int Get() { return val; }
	void Proc(int E_icode, int e_Cnd, int E_dstE) {
		if (E_icode == IRRMOVL && !e_Cnd) val = RNONE;
		else val = E_dstE;
	}
};

/*
bool set_cc = E_icode in {IOPL} &&
	# State changes only during normal operation
	!m_stat in { SADR, SINS, SHLT } && !W_stat in { SADR, SINS, SHLT };
*/

struct SetCC_ {
	int val;
	int Get() { return val; }
	void Proc(int E_icode, int W_stat, int m_stat) {
		val = (E_icode == IOPL && 
			!set<int>{ SADR, SINS, SHLT }.count(m_stat) &&
			!set<int>{ SADR, SINS, SHLT }.count(W_stat));
	}
};

//------------------------------------MEMORY--------------------------------------------------------


/*
int mem_addr = [
	M_icode in { IRMMOVL, IPUSHL, ICALL, IMRMOVL } : M_valE;
	M_icode in { IPOPL, IRET } : M_valA;
	# Other instructions don't need address
];
*/

struct Addr_ {
	int val;
	int Get() { return val; }
	void Proc(int M_icode, int M_valE, int M_valA) {
		if (set<int>{ IRMMOVL, IPUSHL, ICALL, IMRMOVL }.count(M_icode)) val = M_valE;
		else if (set<int>{ IPOPL, IRET }.count(M_icode)) val = M_valA;
	}
};

/*
bool mem_read = M_icode in { IMRMOVL, IPOPL, IRET };
*/

struct Memread_ {
	int val;
	int Get() { return val; }
	void Proc(int M_icode) {
		val = (set<int>{ IMRMOVL, IPOPL, IRET }.count(M_icode));
	}
};

/*
bool mem_write = M_icode in { IRMMOVL, IPUSHL, ICALL };
*/

struct Memwrite_ {
	int val;
	int Get() { return val; }
	void Proc(int M_icode) {
		val = (set<int>{ IRMMOVL, IPUSHL, ICALL }.count(M_icode));
	}
};

/*
int m_stat = [
	dmem_error : SADR;
	1 : M_stat;
];
*/

struct m_stat_ {
	int val;
	int Get() { return val; }
	void Proc(int M_stat, int dmem_error) {
		val = dmem_error ? SADR : M_stat;
	}
};

//------------------------------------WRITEBACK--------------------------------------------------------

/*

*/