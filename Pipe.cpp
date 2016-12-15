#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <set>
using namespace std;

#include "Const.h"
#include "LogicalUnits.h"
#include "HardwareUnits.h"

struct W_ { 
	int stat, icode, valE, valM, dstE, dstM;

	void Print() {
		printf("\"W\" : { ");
		printf("\"stat\" : %d, ",stat);
		printf("\"icode\" : %d, ",icode);
		printf("\"valE\" : %d, ",valE);
		printf("\"valM\" : %d, ",valM);
		printf("\"dstE\" : %d, ",dstE);
		printf("\"dstM\" : %d ",dstM);
		printf("}\n");
	}
};

struct M_ { 
	int stat, icode, Cnd, valE, valA, dstE, dstM;
	void Print() {
		printf("\"M\" : { ");
		printf("\"stat\" : %d, ",stat);
		printf("\"icode\" : %d, ",icode);
		printf("\"Cnd\" : %d, ",Cnd);
		printf("\"valE\" : %d, ",valE);
		printf("\"valA\" : %d, ",valA);
		printf("\"dstE\" : %d, ",dstE);
		printf("\"dstM\" : %d ",dstM);
		printf("}\n");
	}
};

struct E_ { 
	int stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB; 
	void Print() {
		printf("\"E\" : { ");
		printf("\"stat\" : %d, ",stat);
		printf("\"icode\" : %d, ",icode);
		printf("\"ifun\" : %d, ",ifun);
		printf("\"valC\" : %d, ",valC);
		printf("\"valA\" : %d, ",valA);
		printf("\"valB\" : %d, ",valB);
		printf("\"dstE\" : %d, ",dstE);
		printf("\"dstM\" : %d, ",dstM);
		printf("\"srcA\" : %d, ",srcA);
		printf("\"srcB\" : %d ",srcB);
		printf("}\n");
	}
};

struct D_ { 
	int stat, icode, ifun, rA, rB, valC, valP; 
	void Print() {
		printf("\"D\" : { ");
		printf("\"stat\" : %d, ",stat);
		printf("\"icode\" : %d, ",icode);
		printf("\"ifun\" : %d, ",ifun);
		printf("\"rA\" : %d, ",rA);
		printf("\"rB\" : %d, ",rB);
		printf("\"valC\" : %d, ",valC);
		printf("\"valP\" : %d ",valP);
		printf("}\n");
	}	
};

struct F_ { 
	int predPC; 
	void Print() {
		printf("\"F\" : { ");
		printf("\"predPC\" : %d ",predPC);
		printf("}\n");
	}	
};

int COMMAND[101000];

struct PIPE_ {
	int stat;
	// PipelineRegisters
	W_ W;
	M_ M;
	E_ E;
	D_ D;
	F_ F;
	
	// LogicalUnits
	SelectPC_  SelectPC;
	f_icode_ f_icode;
	Needregids_ Needregids;
	NeedvalC_ NeedvalC;
	Instrvalid_ Instrvalid;
	f_ifun_ f_ifun;
	PredictPC_ PredictPC;
	f_stat_ f_stat;
	d_dstE_ d_dstE;
	d_dstM_ d_dstM;
	d_srcA_ d_srcA;
	d_srcB_ d_srcB;
	FwdA_ FwdA;
	FwdB_ FwdB;
	ALUA_ ALUA;
	ALUB_ ALUB;
	ALUfun_ ALUfun;
	e_dstE_ e_dstE;
	SetCC_ SetCC;
	Addr_ Addr;
	Memread_ Memread;
	Memwrite_ Memwrite;
	m_stat_ m_stat;

	//HardwareUnits
	Align_ Align;
	Instructions_ Instructions;
	PCinc_ PCinc;
	Split_ Split;
	RegisterFiles_ RegisterFiles;
	ALU_ ALU;
	CC_ CC;
	cond_ cond;
	Datamemory_ Datamemory;

/*struct W_ { int stat, icode, valE, valM, dstE, dstM; };

struct M_ { int stat, icode, Cnd, valE, valA, dstE, dstM; };

struct E_ { int stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB; };

struct D_ { int stat, icode, ifun, rA, rB, valC, valP; };

struct F_ { int predPC; };
*/

	void Print() {
		printf("0x%08x\n",SelectPC.Get());
		W.Print();
		M.Print();
		E.Print();
		D.Print();
		F.Print();
		RegisterFiles.Print();
	}

	void Init() {
		stat = D.stat = E.stat = M.stat = W.stat = SAOK;
	}
	
	void LHProc(PIPE_ Pas) {
		// Writeback
		RegisterFiles = Pas.RegisterFiles;
		RegisterFiles.Write(Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
	
		// Memory
		Addr.Proc(Pas.M.icode, Pas.M.valE, Pas.M.valA);
		Memread.Proc(Pas.M.icode);
		Memwrite.Proc(Pas.M.icode);
		Datamemory.Proc(Memread.Get(), Memwrite.Get(), Addr.Get(), Pas.M.valA);
		m_stat.Proc(Pas.M.stat, Datamemory.dmem_error);
		
		// Execute
		ALUfun.Proc(Pas.E.icode, Pas.E.ifun);
		ALUA.Proc(Pas.E.icode, Pas.E.valC, Pas.E.valA);
		ALUB.Proc(Pas.E.icode, Pas.E.valB);
		SetCC.Proc(Pas.E.icode, Pas.W.stat, Pas.m_stat.Get());
		ALU.Proc(ALUA.Get(), ALUB.Get(), ALUfun.Get(), SetCC.Get(), Pas.ALU.ZF, Pas.ALU.SF, Pas.ALU.OF); 
		CC.Proc(ALU.ZF, ALU.SF, ALU.OF, SetCC.Get());
		cond.Proc(Pas.E.ifun, CC.le, CC.l, CC.e, CC.ne, CC.ge, CC. g);
		e_dstE.Proc(Pas.E.icode, cond.Get(), Pas.E.dstE);
		
		// Decode
		d_dstE.Proc(Pas.D.icode, Pas.D.rB);
		d_dstM.Proc(Pas.D.icode, Pas.D.rA);
		d_srcA.Proc(Pas.D.icode, Pas.D.rA);
		d_srcB.Proc(Pas.D.icode, Pas.D.rB);
		printf("d_srcA:%d\n",d_srcA.Get());
		printf("d_srcB:%d\n",d_srcB.Get());
		RegisterFiles.Proc(d_srcA.Get(), d_srcB.Get());
		FwdA.Proc(Pas.D.icode, Pas.D.valP, d_srcA.Get(), RegisterFiles.d_rvalA,
				e_dstE.Get(), ALU.valE, Pas.M.dstE, Pas.M.valE, Pas.M.dstM,
				Datamemory.Get(), Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
		FwdB.Proc(d_srcB.Get(), RegisterFiles.d_rvalB,
				e_dstE.Get(), ALU.valE, Pas.M.dstE, Pas.M.valE, Pas.M.dstM,
				Datamemory.Get(), Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
	
		// Fetch
		SelectPC.Proc(Pas.D.icode, Pas.E.icode, Pas.SelectPC.Get(), Pas.M.icode, Pas.M.Cnd, Pas.M.valE, Pas.W.icode, Pas.W.valM, Pas.PCinc.valP);		
		Instructions.Proc(COMMAND + SelectPC.Get(), SelectPC.Isbubble);
		Split.Proc(Instructions.icode, Instructions.ifun);
		f_icode.Proc(Instructions.imem_error, Split.icode);
		f_ifun.Proc(Instructions.imem_error, Split.ifun);
		Instrvalid.Proc(f_icode.Get());
		f_stat.Proc(f_icode.Get(), Instrvalid.Get(), Instructions.imem_error);
		NeedvalC.Proc(f_icode.Get());
		Needregids.Proc(f_icode.Get());
		PCinc.Proc(SelectPC.Get(), COMMAND, SelectPC.Isbubble);
		Align.Proc(Instructions.rA, Instructions.rB, Instructions.valC, Needregids.Get());	
		PredictPC.Proc(f_icode.Get(), Align.valC, PCinc.valP);
		
	}

	void Convey(PIPE_ Pas) {
		// To F
		F.predPC = PredictPC.Get();
		// To D
		D.stat = f_stat.Get();
		D.icode = f_icode.Get();
		D.ifun = f_ifun.Get();
		D.rA = Align.rA;
		D.rB = Align.rB;
		D.valC = Align.valC;
		D.valP = PCinc.valP;

		// To E
		E.stat = Pas.D.stat;
		E.icode = Pas.D.icode;
		E.ifun = Pas.D.ifun;
		E.valC = Pas.D.valC;
		E.valA = FwdA.Get();
		E.valB = FwdB.Get();
		E.dstE = d_dstE.Get();
		E.dstM = d_dstM.Get();
		E.srcA = d_srcA.Get();
		E.srcB = d_srcB.Get();

		// To M
		M.stat = Pas.E.stat;
		M.icode = Pas.E.icode;
		M.Cnd = cond.Get();
		M.valE = ALU.valE;
		M.valA = Pas.E.valA;
		M.dstE = e_dstE.Get();
		M.dstM = Pas.E.dstM;

		// To W
		W.stat = m_stat.Get();
		W.icode = Pas.M.icode;
		W.valE = Pas.M.valE;
		W.valM = Datamemory.Get();
		W.dstE = Pas.M.dstE;
		W.dstM = Pas.M.dstM;

		stat = W.stat;
	}

	void Proc(PIPE_ Pas) {
		LHProc(Pas);
		Convey(Pas);
	}
};

PIPE_ PIPE[1110];

int main() {
	freopen("input.yo","r",stdin);
	freopen("output.json","w",stdout);
	int n = 0;
	while (cin >> hex >> COMMAND[n])
		n++;
	COMMAND[n] = -1;
	int m = 0;
	PIPE[0].Init();	
	//printf("%d\n%d\n",n,COMMAND[44]);
	//return 0;
	do {
		m++;
		PIPE[m].Proc(PIPE[m - 1]);	
		PIPE[m].Print();
		printf("\n");
	} while (PIPE[m].stat == SAOK);
	return 0;
}
