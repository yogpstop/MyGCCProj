#include <string.h>
#include "xml.h"

void xml_next(char c, struct xml *data) {
	if (c == '<' && !data->fl) { // Start of tag
		data->fl = IN_TAG_NAME;
		if (*data->el_n) // Parent is existing
			*strchr(data->el_n, 0) = '/';
	} else if (c == '>' && data->fl) { //End of tag
		if (!(data->fl & IN_DECL)) { // Not declaration
			if ((data->fl & IN_ATTR_NAME) && !strcmp(data->at_n, "/")) {
				data->fl = (data->fl | IN_ENDTAG) & ~IN_ATTR_NAME;
				memset(data->at_n, 0, 64);
			}
			if (data->fl & IN_ENDTAG) { // End of element
				if (data->tag)
					data->tag(data->user, data->el_n, data->el_v);
				char *ptr = strchr(data->el_n, 0);
				while ((--ptr) >= data->el_n) {
					if (*ptr == '/') {
						*ptr = 0;
						break;
					}
					*ptr = 0;
				}
			}
			if (data->fl & IN_ATTR) {// End of attribute
				if (data->attr)
					data->attr(data->user, data->el_n, data->at_n, data->at_v);
				memset(data->at_n, 0, 64);
				memset(data->at_v, 0, 128);
			}
			memset(data->el_v, 0, 512);
		}
		data->fl = 0;
	} else if (data->fl & IN_DECL) { //Declaration
	} else if (c == ' ' && data->fl && !(data->fl & IN_QUOTE)) { // Next attr
		if ((data->fl & IN_ATTR_VAL_EMP) && *data->at_v) {
			if (data->attr)
				data->attr(data->user, data->el_n, data->at_n, data->at_v);
			memset(data->at_n, 0, 64);
			memset(data->at_v, 0, 128);
			data->fl = (data->fl | IN_ATTR_NAME) & ~IN_ATTR_VAL_EMP;
		}
		if ((data->fl & IN_ATTR_NAME) && *data->at_n) {
			if (data->attr)
				data->attr(data->user, data->el_n, data->at_n, data->at_v);
			memset(data->at_n, 0, 64);
		}
		if ((data->fl & IN_TAG_NAME) &&
				*data->el_n && *(strchr(data->el_n, 0) - 1) != '/') {
			data->fl = (data->fl | IN_ATTR_NAME) & ~IN_TAG_NAME;
		}
		// FIXME IN_ATTR_VAL_EMP and at_v is empty
	} else if (c == '=' && (data->fl & IN_ATTR_NAME)) { // Attribute equals
		data->fl = (data->fl | IN_ATTR_VAL_EMP) & ~IN_ATTR_NAME;
	} else if (c == '"' && (data->fl & IN_ATTR_VAL_EMP)) { // Attribute quote
		data->fl = (data->fl | IN_ATTR_VAL_QUOT) & ~IN_ATTR_VAL_EMP;
	} else if (c == '"' && (data->fl & IN_ATTR_VAL_QUOT)) { // End of quote
		data->fl = (data->fl | IN_ATTR_VAL_EMP) & ~IN_ATTR_VAL_QUOT;
	} else if (c == '\'' && (data->fl & IN_ATTR_VAL_EMP)) { // Attribute apos
		data->fl = (data->fl | IN_ATTR_VAL_APOS) & ~IN_ATTR_VAL_EMP;
	} else if (c == '\'' && (data->fl & IN_ATTR_VAL_APOS)) { // End of apos
		data->fl = (data->fl | IN_ATTR_VAL_EMP) & ~IN_ATTR_VAL_APOS;
	} else if (c == '/' && data->fl && !(data->fl & IN_QUOTE)) { // End tag
		data->fl |= IN_ENDTAG;
		if (*data->el_n && *(strchr(data->el_n, 0) - 1) == '/')
			*(strchr(data->el_n, 0) - 1) = 0;
	} else if (c == '?' && IN_TAG_NAME_START) { // Declaration
		data->fl |= IN_DECL;
	} else if (data->fl & IN_ATTR_NAME) {
		*strchr(data->at_n, 0) = c;
	} else if (data->fl & IN_ATTR_VAL) {
		*strchr(data->at_v, 0) = c;
	} else if ((data->fl & (IN_TAG_NAME | IN_ENDTAG)) == IN_TAG_NAME) {
		*strchr(data->el_n, 0) = c;
	} else if (!data->fl) {
		*strchr(data->el_v, 0) = c;
	}
}
