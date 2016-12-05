struct f_ifun {
	int val;
	int Get() { return val; }
	void Proc(int imem_error, int imen_ifun) {
		val = imem_error ? FNONE : imem_ifun;
	} 
};