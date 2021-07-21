
/*
	we want to determine, for all functions that are pointed to on virtual function tables, what instructions may call them

	so we JIT special thunks that load the current return address, subtract the base of the exe, and then search for it in an array to see if we already have that return address

	so that reallocation is not needed we greatly overallocate. (this assumes the user has enormous amounts of memory, in this case a workstation with 256 gb of ddr4 is used)
*/