struct f_icode_ {
	int val;
	int Get() { return val; }
	void Proc(int imem_error, int imem_icode) {
		if (imem_error) return INOP;
		else return imem_icode;
	}
};