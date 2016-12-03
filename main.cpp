#include <bits/stdc++.h>
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

struct PIPE_ {
	int stat;
	W_ W;
	M_ M;
	E_ E;
	D_ D;
	F_ F;
	
	void Convey(PIPE_ Pas) {
		// To W
		W.stat = Pas.M.stat;
		W.icode = Pas.M.icode;
		W.valE = Pas.M.valE;
		W.valM = Pas.Memory.Get();
		W.dstE = Pas.M.dstE;
		W.dstM = Pas.M.dstM;

		// To M
		M.stat = Pas.E.stat;
		M.icode = Pas.E.icode;
		M.ifun = Pas.CC.Get();
		M.valE = Pas.ALU.Get();
		M.valA = Pas.E.valA;
		M.dstE = 
	}

	void Proc(PIPE_ Pas) {
		Convey(Pas);
	}
} PIPE;

int main() {
	return 0;
}