struct SelectPC_ {
	int ans;
	int Get() { return ans; }
	void Proc(int M_icode, int M_Cnd, int M_valA, int W_icode, int F_predPC) {
		if (M_icode == IJXX && !M_Cnd)
			ans = M_valA;
		else if (W_icode == IRET)
			ans = W_valM;
		else 
			ans = F_prePC;
	}
};