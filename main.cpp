#include <bits/stdc++.h>
#include <StructDefine.h>
using namespace std;

struct W_ {
	int stat, icode, valE, valM, dstE, dstM;
};

struct M_ {
	int stat, icode, Cnd, valE, valA, dstE, dstM;
};

struct E_ {
	int stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB;
};

struct D_ {
	int stat, icode, ifun, rA, rB, valC, valP;
};

struct F_ {
	int predPC;
};

struct Memory_ {
};

struct CC_ {
};

struct ALU_ {
};

struct LdstE_ {
};

struct FwdA_ {
};

struct FwdB_ {
};

struct Commands_ {
	int A[6];
	int icode, ifunc, rA, rB, valC;

	void Proc(int s) {
		for (int i = 0; i < 6; i++) A[i] = COMMAND[s + i];
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

struct SelectPC_ {
	
};

struct stadD_ {
};

struct PCinc_ {
	int valP;

	void Proc(int iCode, int s) {
		if (iCode <= 1 || icode == 9) valP = 1;
		else if (icode <= 5 && icode >= 3) valP = 6;
		else if (icode == 7 || icode == 8) valP = 5;
		else valP = 2;
	}

	int Get() { return valP; }
};

struct PredictPC_ {
	int val;

	void Proc(int valC, int valP) {
		
	}
};

struct PIPE_ {
	int stat;
	W_ W;
	M_ M;
	E_ E;
	D_ D;
	F_ F;
	
	void LogicalProc(PIPE_ Pas) {		
		SelectPC.Proc();		
		Commands.Proc(SelectPC.Get());
		PCinc.Proc(Commands.iCode, SelctPc.Get());
		PredictPC.Proc(Commands.valC, PCinc.valP);
		Memory.Proc();
		CC.Proc();
		ALU.Proc();
		LdstE.Proc();
		FwdB.Proc();
		stadD.Proc();
	}

	void Convey(PIPE_ Pas) {
		// To W
		W.stat = Pas.M.stat;
		W.icode = Pas.M.icode;
		W.valE = Pas.M.valE;
		W.valM = Memory.Get();
		W.dstE = Pas.M.dstE;
		W.dstM = Pas.M.dstM;

		// To M
		M.stat = Pas.E.stat;
		M.icode = Pas.E.sicode;
		M.ifun = CC.Get();
		M.valE = ALU.Get();
		M.valA = Pas.E.valA;
		M.dstE = LdstE.Get();
		M.dstM = Pas.E.dstM;

		// To E
		E.stat = Pas.D.stat;
		E.icode = Pas.D.icode;
		E.ifun = Pas.E.ifun;
		E.valC = Pas.D.valC;
		E.valA = FwdA.Get();
		E.valB = FwdB.Get();
		// To do : dstE dstM srcA srcB

		// To D
		D.stat = statD.Get();
		D.icode = Commands.icode;
		D.ifun = Commands.ifun;
		D.rA = Commands.rA;
		D.rB = Commands.rB;
		D.valC = Commands.valC;
		D.valP = PCinc.Get();	

		// To E
		E.predPC = PredictPC.Get();
	}

	void Proc(PIPE_ Pas) {
		LogicalProc(Pas);
		Convey(Pas);
	}
} PIPE;

int main() {
	return 0;
}