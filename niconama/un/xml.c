#include <string.h>
#include "xml.h"

void next(char c, struct xml *data) {
	if(c=='>' && (data->fl&IN_TAG) != 0) {
		if((data->fl&IN_DECL) == 0) {
			if(data->pv=='/'){
				if((data->fl&IN_ATTR_VAL_EMP)!=0)
    				*(strchr(data->at_v,0)-1)=0;
    			if((data->fl&IN_ATTR_NAME)!=0)
    				*(strchr(data->at_n,0)-1)=0;
    			if((data->fl&IN_TAG_NAME)!=0)
    				*(strchr(data->el_n,0)-1)=0;
    		}
			if((data->fl&IN_ATTR)!=0)
				if(data->attr)
					data->attr(data);
			if(data->pv=='/')
				if(data->tag)
					data->tag(data);
			if(data->pv=='/'||(data->fl&IN_ENDTAG)!=0) {
    			char *ptr = strchr(data->el_n,0);
    			while((--ptr)>=data->el_n) {
    				if(*ptr=='.') {
    					*ptr=0;
    					break;
    				}
    				*ptr=0;
    			}
			}
		}
		memset(data->at_n,0,64);
		memset(data->at_v,0,128);
		data->fl=0;
	} else if((data->fl&IN_DECL) != 0) {
	} else if(c=='<' && (data->fl&IN_TAG) == 0) {
		if(data->tag)
			data->tag(data);
		data->fl=IN_TAG_NAME;
		*strchr(data->el_n,0)='.';
		data->pv=0;
		memset(data->el_v,0,512);
	} else if(c=='=' && (data->fl&IN_ATTR_NAME)!=0) {
		data->fl=(data->fl|IN_ATTR_VAL_EMP)&(0xFFFF^IN_ATTR_NAME);
		data->pv=0;
	} else if(c=='"' && (data->fl&IN_ATTR_VAL_QUOT)!=0) {
		data->fl=(data->fl|IN_ATTR_VAL_EMP)&(0xFFFF^IN_ATTR_VAL_QUOT);
		data->pv='"';
	} else if(c=='"' && (data->fl&IN_ATTR_VAL_EMP)!=0 && data->pv==0) {
		data->fl=(data->fl|IN_ATTR_VAL_QUOT)&(0xFFFF^IN_ATTR_VAL_EMP);
		data->pv='"';
	} else if(c=='\'' && (data->fl&IN_ATTR_VAL_APOS)!=0) {
		data->fl=(data->fl|IN_ATTR_VAL_EMP)&(0xFFFF^IN_ATTR_VAL_APOS);
		data->pv='\'';
	} else if(c=='\'' && (data->fl&IN_ATTR_VAL_EMP)!=0 && data->pv==0) {
		data->fl=(data->fl|IN_ATTR_VAL_APOS)&(0xFFFF^IN_ATTR_VAL_EMP);
		data->pv='\'';
	} else if(c==' ' && (data->fl&(OUT_QUOTE))!=0) {
		if(data->pv!=0) {
			if ((data->fl&IN_TAG_NAME)==0) {
				if(data->attr)
					data->attr(data);
				memset(data->at_n,0,64);
				memset(data->at_v,0,128);
			}
			data->fl=(data->fl|IN_ATTR_NAME)&(0xFFFF^(IN_TAG_NAME|IN_ATTR_VAL_EMP));
			data->pv=0;
		}
	} else if(c=='/' && (data->fl&IN_TAG_NAME)!=0 && data->pv==0) {
		data->fl|=IN_ENDTAG;
		*(strchr(data->el_n,0)-1)=0;
	} else if(c=='?' && (data->fl&IN_TAG_NAME)!=0 && data->pv==0) {
		data->fl|=IN_DECL;
		*(strchr(data->el_n,0)-1)=0;
	} else if((data->fl&IN_ATTR_VAL)!=0) {
		*strchr(data->at_v,0)=data->pv=c;
	} else if((data->fl&IN_ATTR_NAME)!=0) {
		*strchr(data->at_n,0)=data->pv=c;
	} else if((data->fl&(IN_TAG_NAME|IN_ENDTAG))==IN_TAG_NAME) {
		*strchr(data->el_n,0)=data->pv=c;
	} else if((data->fl&IN_TAG)==0) {
		*strchr(data->el_v,0)=data->pv=c;
	}
}