#include "string_test.h"

int main(int argc, char **argv)
{
	int index = 1;

	fprintf(stderr, "============================================================\n");
	fprintf(stderr, " Mini-Core ANSI C Library Unit Test\n");
	fprintf(stderr, "============================================================\n");

	fprintf(stderr, "    [%02d] mco_strncat_test()     . . . %s\n", index++, mco_strncat_test() ? "OK" : "NOT OK");
	fprintf(stderr, "    [%02d] mco_left_trim_test()   . . . %s\n", index++, mco_left_trim_test() ? "OK" : "NOT OK");
	fprintf(stderr, "    [%02d] mco_right_trim_test()  . . . %s\n", index++, mco_right_trim_test() ? "OK" : "NOT OK");
	fprintf(stderr, "    [%02d] mco_trim_test()        . . . %s\n", index++, mco_trim_test() ? "OK" : "NOT OK");
	fprintf(stderr, "    [%02d] mco_replace_str_test() . . . %s\n", index++, mco_replace_str_test() ? "OK" : "NOT OK");

	if ( strlen(mco_get_last_error()) ) {
		fprintf(stderr, "\nerror: %s\n", mco_get_last_error());
		return 1;
	}

	return 0;
}
