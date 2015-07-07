#include <stdlib.h>

int main () {
	int s = setenv("BINDER_ADDRESS", "ubuntu1204-004.student.cs.uwaterloo.ca", 1);
	return s;


}
