#include <cstdio>
#include <cstring>
#include <algorithm>
#include <set>
using namespace std;

#include "Const.h"
#include "LogicalUnits.h"
#include "HardwareUnits.h"

struct W_ { int stat, icode, valE, valM, dstE, dstM; };

struct M_ { int stat, icode, Cnd, valE, valA, dstE, dstM; };

struct E_ { int stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB; };

struct D_ { int stat, icode, ifun, rA, rB, valC, valP; };

struct F_ { int predPC; };

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

	void LHProc(PIPE_ Pas) {		
		// Fetch
		SelectPC.Proc(Pas.M.icode, Pas.M.Cnd, Pas.M.valA, Pas.W.icode, Pas.W.valM, F.predPC);		
		Instructions.Proc(COMMAND + SelectPC.Get());
		Split.Proc(Instructions.icode, Instructions.ifun);
		f_icode.Proc(Instructions.imem_error, Split.icode);
		f_ifun.Proc(Instructions.imem_error, Split.ifun);
		Instrvalid.Proc(f_icode.Get());
		f_stat.Proc(f_icode.Get(), Instrvalid.Get(), Instructions.imem_error);
		NeedvalC.Proc(f_icode.Get());
		Needregids.Proc(f_icode.Get());
		Align.Proc(Instructions.rA, Instructions.rB, Instructions.valC, Needregids.Get());	
		PredictPC.Proc(f_icode.Get(), Align.valC, PCinc.valP);
		
		// Decode
		d_dstE.Proc(Pas.D.icode, Pas.D.rB);
		d_dstM.Proc(Pas.D.icode, Pas.D.rA);
		d_srcA.Proc(Pas.D.icode, Pas.D.rA);
		d_srcB.Proc(Pas.D.icode, Pas.D.rB);
		FwdA.Proc(Pas.D.icode, Pas.D.valP, d_srcA.Get(), Pas.RegisterFiles.d_rvalA,
				Pas.e_dstE.Get(), Pas.ALU.valE, Pas.M.dstE, Pas.M.valE, Pas.M.dstM,
				Pas.Datamemory.Get(), Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
		FwdB.Proc(d_srcB.Get(), Pas.RegisterFiles.d_rvalB,
				Pas.e_dstE.Get(), Pas.ALU.valE, Pas.M.dstE, Pas.M.valE, Pas.M.dstM,
				Pas.Datamemory.Get(), Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
		
		// Execute		
		ALUfun.Proc(Pas.E.icode, Pas.E.ifun);
		ALUA.Proc(Pas.E.icode, Pas.E.valC, Pas.E.valA);
		ALUB.Proc(Pas.E.icode, Pas.E.valB);
		SetCC.Proc(Pas.E.icode, Pas.W.stat, Pas.m_stat.Get());
		ALU.Proc(ALUA.Get(), ALUB.Get(), ALUfun.Get()); 
		CC.Proc(ALU.ZF, ALU.SF, ALU.OF, SetCC.Get());
		cond.Proc(Pas.E.ifun, CC.le, CC.l, CC.e, CC.ne, CC.ge, CC. g);
		e_dstE.Proc(Pas.E.icode, cond.Get(), Pas.E.dstE);
		
		// Memory
		Addr.Proc(Pas.M.icode, Pas.M.valE, Pas.M.valA);
		Memread.Proc(Pas.M.icode);
		Memwrite.Proc(Pas.M.icode);
		Datamemory.Proc(Memread.Get(), Memwrite.Get(), Addr.Get(), Pas.M.valA);
		m_stat.Proc(Pas.M.stat, Datamemory.dmem_error);

		// Writeback
		RegisterFiles.Write(Pas.W.dstM, Pas.W.valM, Pas.W.dstE, Pas.W.valE);
	}

	void Convey(PIPE_ Pas) {

		// To F
		F.predPC = Pas.PredictPC.Get();
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
	}

	void Proc(PIPE_ Pas) {
		LHProc(Pas);
		Convey(Pas);
	}
} PIPE;

int main() {
	return 0;
}
