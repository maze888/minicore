#include "string_test.h"

int mco_strncat_test()
{
	char buf[5] = "ab";
	char buf_add[] = "cd";

	if ( mco_strncat(buf, buf_add, sizeof(buf)) != 4 ) return 0;
	if ( strcmp(buf, "abcd") != 0 ) return 0;
	
	char buf2[4] = "ab";
	char buf_add2[] = "c";

	if ( mco_strncat(buf2, buf_add2, sizeof(buf)) != 3 ) return 0;
	if ( strcmp(buf2, "abc") != 0 ) return 0;

	return 1;
}

int mco_left_trim_test()
{
	char buf[16] = "     ab"; // five left space
	
	if ( mco_left_trim(buf) != 5 ) return 0;
	if ( strlen(buf) != 2 ) return 0;

	return 1;
}

int mco_right_trim_test()
{
	char buf[16] = "ab     "; // five right space
	
	if ( mco_right_trim(buf) != 5 ) return 0;
	if ( strlen(buf) != 2 ) return 0;
	
	return 1;
}

int mco_trim_test()
{
	char buf[16] = "     ab     "; // five space(left, right)
	
	if ( mco_trim(buf) != 10 ) return 0;
	if ( strlen(buf) != 2 ) return 0;
	
	return 1;
}

int mco_replace_str_test()
{
	char buf[64] = "ABC, ABC, ACCABC, BCBCBCBCBC";
	char buf2[] = "AZZZ, AZZZ, ACCAZZZ, ZZZZZZZZZZZZZZZ";

	size_t buf_len = mco_replace_str(buf, sizeof(buf), "BC", "ZZZ");
	if ( !buf_len ) return 0;

	if ( buf_len != strlen(buf2) ) return 0;

	return 1;
}
