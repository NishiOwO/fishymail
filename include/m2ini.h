/* $Id$ */

#ifndef __M2INI_H__
#define __M2INI_H__

/**
 * To use this library - do this in one C or C++ file:
 *   #define M2INI_IMPLEMENTATION
 *   #include "m2ini.h"
 */

#ifndef MIDEF
#ifdef MI_STATIC
#define MIDEF static
#else
#define MIDEF extern
#endif
#endif

#ifndef MI_HAS_C99
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 199901L
#define MI_HAS_C99
#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifdef MI_HAS_C99
#else
#endif

#define mi_ini_t struct mi_ini

mi_ini_t;

struct mi_ini {
	mi_ini_t** child;
	int	   type;
	char*	   comment;
	char*	   key;
	char*	   value;
};

enum MI_TYPE {
	MI_ROOT = 0,
	MI_SECTION,
	MI_KV,
	MI_COMMENT
};

/**
 * @~english
 * @brief Parse INI
 * @param data Data
 * @param size Size
 * @return Parsed result
 */
MIDEF mi_ini_t* mi_parse(const char* data, size_t size);

/**
 * @~english
 * @brief Parse INI
 * @param path Path
 * @return Parsed result
 */
MIDEF mi_ini_t* mi_parse_file(const char* path);

/**
 * @~english
 * @brief Free INI
 * @param ini INI
 */
MIDEF void mi_free(mi_ini_t* ini);

/**
 * @~english
 * @brief Create new INI
 * @return INI
 */
MIDEF mi_ini_t* mi_new(void);

/**
 * @~english
 * @brief Create new key-value pair
 * @param ini INI
 * @param key Key
 * @param value Value
 * @return Key-value pair
 */
MIDEF mi_ini_t* mi_new_kv(mi_ini_t* ini, const char* key, const char* value);

/**
 * @~english
 * @brief Create new section
 * @param ini INI
 * @param name Name
 * @return Section
 */
MIDEF mi_ini_t* mi_new_section(mi_ini_t* ini, const char* name);

/**
 * @~english
 * @brief Create new comment
 * @param ini INI
 * @param comment Comment
 * @return Comment
 */
MIDEF mi_ini_t* mi_new_comment(mi_ini_t* ini, const char* comment);

/**
 * @~english
 * @brief Set key
 * @param ini INI
 * @param key Key
 */
MIDEF void mi_set_key(mi_ini_t* ini, const char* key);
#define mi_set_name mi_set_key

/**
 * @~english
 * @brief Set value
 * @param ini INI
 * @param value Value
 */
MIDEF void mi_set_value(mi_ini_t* ini, const char* value);

/**
 * @~english
 * @brief Get key-value pair
 * @param ini INI
 * @param key Key
 * @return Key-value pair
 */
MIDEF mi_ini_t* mi_get_kv(mi_ini_t* ini, const char* key);

/**
 * @~english
 * @brief Get section
 * @param ini INI
 * @param name Name
 * @return Section
 */
MIDEF mi_ini_t* mi_get_section(mi_ini_t* ini, const char* name);

/**
 * @~english
 * @brief Stringify INI
 * @param ini INI
 * @return String
 */
MIDEF char* mi_string(mi_ini_t* ini);

#ifdef M2INI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* add src to ini */
static void mi_add(mi_ini_t* ini, mi_ini_t* src) {
	mi_ini_t** old = ini->child;
	int	   i;
	for(i = 0; old[i] != NULL; i++);
	ini->child = malloc(sizeof(*ini->child) * (i + 2));
	for(i = 0; old[i] != NULL; i++) ini->child[i] = old[i];
	ini->child[i]	  = src;
	ini->child[i + 1] = NULL;
	free(old);
}

static char* mi_strdup_trim(const char* str) {
	char* r = malloc(strlen(str) + 1);
	int   i;
	r[0] = 0;
	for(i = 0; str[i] != 0; i++) {
		if(str[i] != ' ') {
			strcpy(r, str + i);
			break;
		}
	}
	for(i = strlen(r) - 1; i >= 0; i--) {
		if(r[i] != ' ') {
			break;
		}
		r[i] = 0;
	}
	return r;
}

static void mi_strcat(char** s, const char* a) {
	char* old = *s;
	char* r	  = malloc(strlen(old) + strlen(a) + 1);
	r[0]	  = 0;
	strcat(r, old);
	strcat(r, a);
	free(old);
	*s = r;
}

void mi_set_key(mi_ini_t* ini, const char* key) {
	if(ini->key != NULL) free(ini->key);
	ini->key = malloc(strlen(key) + 1);
	strcpy(ini->key, key);
}

void mi_set_value(mi_ini_t* ini, const char* value) {
	if(ini->value != NULL) free(ini->value);
	ini->value = malloc(strlen(value) + 1);
	strcpy(ini->value, value);
}

void mi_set_comment(mi_ini_t* ini, const char* comment) {
	if(ini->comment != NULL) free(ini->comment);
	ini->comment = malloc(strlen(comment) + 1);
	strcpy(ini->comment, comment);
}

mi_ini_t* mi_new(void) {
	mi_ini_t* ini = malloc(sizeof(*ini));
	memset(ini, 0, sizeof(*ini));
	ini->type = MI_ROOT;

	ini->child    = malloc(sizeof(*ini->child));
	ini->child[0] = NULL;

	return ini;
}

mi_ini_t* mi_new_kv(mi_ini_t* ini, const char* key, const char* value) {
	mi_ini_t* r = mi_new();
	r->type	    = MI_KV;

	mi_set_key(r, key);
	mi_set_value(r, value);

	mi_add(ini, r);

	return r;
}

mi_ini_t* mi_new_section(mi_ini_t* ini, const char* name) {
	mi_ini_t* r = mi_new();
	r->type	    = MI_SECTION;

	mi_set_name(r, name);

	mi_add(ini, r);

	return r;
}

mi_ini_t* mi_new_comment(mi_ini_t* ini, const char* comment) {
	mi_ini_t* r = mi_new();
	r->type	    = MI_COMMENT;

	mi_set_comment(r, comment);

	mi_add(ini, r);

	return r;
}

mi_ini_t* mi_get_kv(mi_ini_t* ini, const char* key) {
	int i;
	for(i = 0; ini->child[i] != NULL; i++) {
		mi_ini_t* c = ini->child[i];
		if(c->type == MI_KV && strcmp(c->key, key) == 0) return c;
	}
	return NULL;
}

mi_ini_t* mi_get_section(mi_ini_t* ini, const char* name) {
	int i;
	for(i = 0; ini->child[i] != NULL; i++) {
		mi_ini_t* c = ini->child[i];
		if(c->type == MI_SECTION && strcmp(c->key, name) == 0) return c;
	}
	return NULL;
}

char* mi_string(mi_ini_t* ini) {
	int   i;
	int   p;
	char* r;

	if(ini->type == MI_KV) {
		char* s = malloc(strlen(ini->key) + 1 + strlen(ini->value) + 1);
		s[0]	= 0;
		strcat(s, ini->key);
		strcat(s, "=");
		strcat(s, ini->value);
		return s;
	}

	r    = malloc(1);
	r[0] = 0;
	if(ini->type == MI_SECTION) {
		mi_strcat(&r, "[");
		mi_strcat(&r, ini->key);
		mi_strcat(&r, "]\n");
	}
	for(p = 0; p < 2; p++) {
		for(i = 0; ini->child[i] != NULL; i++) {
			mi_ini_t* c = ini->child[i];
			if(p == 0 && c->type == MI_KV) {
				char* t = mi_string(c);
				mi_strcat(&r, t);
				mi_strcat(&r, "\n");
				free(t);
			} else if(p == 1 && c->type == MI_SECTION) {
				char* t = mi_string(c);
				mi_strcat(&r, t);
				free(t);
			}
		}
		if(p == 0) mi_strcat(&r, "\n");
	}

	return r;
}

mi_ini_t* mi_parse(const char* data, size_t size) {
	size_t	  i;
	mi_ini_t* ini	  = mi_new();
	mi_ini_t* current = ini;
	char*	  buf	  = malloc(size + 1);
	int	  incr	  = 0;
	memcpy(buf, data, size);
	buf[size] = 0;

	for(i = 0; i <= size; i++) {
		if(i == size || buf[i] == '\n') {
			int   j;
			char* line = buf + incr;
			int   eq   = -1;
			buf[i]	   = 0;

			for(j = 0; line[j] != 0; j++) {
				if(line[j] == '\r') {
					line[j] = 0;
					break;
				} else if(line[j] == '=') {
					eq = j;
				}
			}
			if(strlen(line) > 0) {
				if(line[0] == ';') {
					char* com = mi_strdup_trim(line + 1);

					mi_new_comment(current, com);

					free(com);
				} else if(eq >= 0) {
					char* key;
					char* value;
					line[eq] = 0;
					key	 = mi_strdup_trim(line);
					value	 = mi_strdup_trim(line + eq + 1);

					mi_new_kv(current, key, value);

					free(value);
					free(key);
				} else if(strlen(line) >= 2 && line[0] == '[' && line[strlen(line) - 1] == ']') {
					line[strlen(line) - 1] = 0;
					current		       = mi_new_section(ini, line + 1);
				}
			}

			incr = i + 1;
			if(i == size) break;
		}
	}
	free(buf);

	return ini;
}

mi_ini_t* mi_parse_file(const char* path) {
	FILE* f = fopen(path, "rb");
	if(f != NULL) {
		size_t	  sz;
		char*	  buffer;
		mi_ini_t* ini;

		fseek(f, 0, SEEK_END);
		sz = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(sz);
		fread(buffer, sz, 1, f);

		fclose(f);
		ini = mi_parse(buffer, sz);
		free(buffer);

		return ini;
	}
	return NULL;
}

void mi_free(mi_ini_t* ini) {
	int i;
	for(i = 0; ini->child[i] != NULL; i++) {
		mi_free(ini->child[i]);
	}
	free(ini->child);
	if(ini->key != NULL) free(ini->key);
	if(ini->value != NULL) free(ini->value);
	free(ini);
}

#endif

#ifdef __cplusplus
}
#endif

#endif
