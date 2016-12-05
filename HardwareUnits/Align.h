struct Align_ {
	int rA, rB, valC;
	void Proc(int rA_, int rB_, int valC_, int NeedRegids) {
		if (NeedRegids) rA = rA_, rB = rB_;
		else rA = rB = 0xF;
		valC = valC_;
	}
};