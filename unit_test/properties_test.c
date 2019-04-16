#include "properties_test.h"

int mco_properties_test()
{
	char *value;

	value = mco_get_property("./test.conf", "test_key", "test_section");
	if ( !value ) return 0;
	free(value);

	value = mco_get_property("./test.conf", "test_key2", "test_section");
	if ( !value ) return 0;
	free(value);

	return 1;
}
