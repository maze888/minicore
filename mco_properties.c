#include "mco_properties.h"
#include "mco_hidden.h"

/**
 * @brief   INI format file's property getter
 *
 * @param   path     INI file path
 * @param   key      key to find
 * @param   section  section to find
 *
 * @return  Found value
 */
char * mco_get_property(const char *path, const char *key, const char *section)
{
	char *value = NULL;
	char *line = NULL;
	FILE *fp = NULL;

	if ( !path || !key || !section ) {
		MCO_SET_ERROR("invalid argument - path: %s, key: %s, section: %s", path ? path : "NULL", key ? key : "NULL", section ? section : "NULL");
		return NULL;
	}

	fp = fopen(path, "rt");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", path, strerror(errno), errno);
		return NULL;
	}

	char section2[256] = {0};
	snprintf(section2, sizeof(section2), "[%s]", section);

	int found = 0;
	size_t line_size = 0;
	while ( getline(&line, &line_size, fp) > 0 ) {
		char *p = strchr(line, ';');
		if ( p ) p[0] = '\0';
		
		p = strchr(line, '#');
		if ( p ) p[0] = '\0';

		if ( !found ) {
			if ( !strcasestr(line, section2) ) continue; // search section
			found = 1;
		}

		char *token, *save_token;
		token = strtok_r(line, "=", &save_token);
		if ( token ) {
			mco_trim(token);
			if ( strcasecmp(token, key) == 0 ) {
				mco_trim(save_token);
				value = strdup(save_token);
				break;
			}
		}
	}

	safe_fclose(fp);
	safe_free(line);

	return value;
}
