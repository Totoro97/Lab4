struct PredictPC_ {
	int ans;
	int Get() { return ans; }
	void Proc(int f_icode, int f_valC, int f_valP) {
		if (f_icode == IJXX || f_code == ICALL) ans = f_valC;
		else ans = f_valP;		
	}
};