#ifndef __UTIL_H__
#define __UTIL_H__

inline char* TestFirstName(const char *name,char *string)
{
	if(name==0||string==0) return 0;
	int LengthName=strlen(name),LengthString=strlen(string);
	_strlwr((char*)name);
	if(LengthName<LengthString) return 0;
	for(int i=0;i<LengthString;i++)
		if(name[i]!=string[i]) return 0;
	return (char*)name+LengthString;
}
inline const char* GetFileName(const char *str)
{
	if(str==0) return 0;
	int l=strlen(str);
	if((l==0)||(str[l-1]=='\\')) return str;
	for(int i=l-1;i>=0;i--)
		if(str[i]=='\\')
			return &str[i+1];
	return str;
}
inline char* GetKey(const void *buf,int size,char *key)
{
	if(buf==0||size==0||key==0) return 0;
	char *str=(char*)buf;
	for(int i=0;i<size;i++)
		for(int l=0;str[i+l]==key[l];l++)
			if(key[l+1]==0)
				return &str[i];
	return 0;
}
inline char* isGroupName(const char *name)				{ return TestFirstName((char*)name,"group "); }

#endif //__UTIL_H__