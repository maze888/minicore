/**
 * @file    mco_properties.c
 * @brief   설정파일 제어관련 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_properties.h"
#include "mco_inter.h"

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

// TODO: section argument impl
int mco_set_property(const char *path, const char *key, const char *value)
{
	char *line = NULL;
	char *fbuf = NULL;
	FILE *fp = NULL, *write_fp = NULL;

	if ( !path || !key || !value ) {
		MCO_SET_ERROR("invalid argument - path: %s, key: %s, value: %s", path ? path : "NULL", key ? key : "NULL", value ? value : "NULL");
		goto out;
	}

	fp = fopen(path, "rt");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", path, strerror(errno), errno);
		goto out;
	}

	long fbuf_size = mco_get_file_len(path) + 8192; // one line max 8192

	fbuf = (char *)calloc(1, fbuf_size);
	if ( !fbuf ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	size_t line_size = 0;
	while ( getline(&line, &line_size, fp) > 0 ) {
		char *line_buf = strdup(line);
		if ( !line_buf ) {
			MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
			goto out;
		}

		char *token, *save_token;
		token = strtok_r(line_buf, "=", &save_token);
		if ( token ) {
			mco_trim(token);
			mco_trim(save_token);
			if ( strcasecmp(token, key) == 0 ) {
				if ( !mco_strncat(fbuf, key, fbuf_size) ) goto out;
				if ( !mco_strncat(fbuf, " = ", fbuf_size) ) goto out;
				if ( !mco_strncat(fbuf, value, fbuf_size) ) goto out;
				if ( !mco_strncat(fbuf, "\n", fbuf_size) ) goto out;
			}
			else {
				if ( !mco_strncat(fbuf, line, fbuf_size) ) goto out;
			}
		}
		else {
			if ( !mco_strncat(fbuf, line, fbuf_size) ) goto out;
		}

		free(line_buf);
	}

	write_fp = fopen(path, "wt");
	if ( !write_fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", path, strerror(errno), errno);
		goto out;
	}

	fprintf(write_fp, "%s", fbuf);

	safe_fclose(fp);
	safe_fclose(write_fp);
	safe_free(line);
	safe_free(fbuf);

	return 0;

out:
	safe_fclose(fp);
	safe_fclose(write_fp);
	safe_free(line);
	safe_free(fbuf);

	return -1;
}
