#include "SceneMesh.h"
#include <xutil.h>
#include "zip_resource.h"
#include "zip_headers.h"


XZIP_FileHeader::XZIP_FileHeader(void)
{
	fileName = NULL;
	dataOffset = 0;
	dataSize = 0;

	extDataSize = 0;
	extData = NULL;
}

void XZIP_FileHeader::set(const char* fname,unsigned offs,unsigned size,void* ext_ptr,int ext_sz)
{
	SetName(fname);
	dataOffset = offs;
	dataSize = size;

	if(ext_sz){
		extData = new char[extDataSize = ext_sz];
		memcpy(extData,(char*)ext_ptr,extDataSize);
	}
}

void XZIP_FileHeader::load(XStream& fh)
{
	int sz;
	char fname[256];
	fh > sz;

	fh.read(fname,sz);
	fname[sz] = 0;

	SetName(fname);

	fh > dataOffset > dataSize > extDataSize;

	if(extDataSize){
		extData = new char[extDataSize];
		fh.read(extData,extDataSize);
	}
}

XZIP_FileHeader::~XZIP_FileHeader(void)
{
	if(fileName) free(fileName);

	if(extData)
		delete extData;
}

void XZIP_FileHeader::SetName(const char* p)
{
	int i,sz = strlen(p);
	fileName = strdup(p); 
	for(i = 0; i < sz; i ++)
		if(fileName[i] == '/') fileName[i] = '\\';
}

void XZIP_FileHeader::save(XStream& fh)
{
	int sz = strlen(fileName);
	fh < sz;
	fh.write(fileName,sz);
	fh < dataOffset < dataSize < extDataSize;

	if(extDataSize)
		fh.write(extData,extDataSize);
}

XZIP_Resource::XZIP_Resource(const char* fname,int fl)
{
	flags = fl;

	fileName = fname;
	idxName = fname;
	int sz = strlen(fname);
	idxName[sz - 1] = 'x';
	idxName[sz - 2] = 'd';
	idxName[sz - 3] = 'i';

	file.open(fname,XS_IN);
	if(flags & XZIP_ENABLE_ZIP_HEADERS)
		LoadHeaders();
	else 
		LoadIndex();
}

void XZIP_Resource::LoadIndex(void)
{
	int sz;
	XStream fh(idxName.c_str(),XS_IN);
	fh > sz;
	for(int i = 0; i < sz; i ++){
		fileList.push_back(XZIP_FileHeader());
		fileList.back().load(fh);
	}
	fh.close();
}

void XZIP_Resource::LoadHeaders(void)
{
	int fl = 0;
	longint sig;

	char fname[512];

	local_file_header lhdr;
	central_directory_file_header chdr;
	end_central_dir_record ecr;

	while(!fl){
		file > sig;

		switch(sig){
			case local_file_header_signature:
				file > lhdr.version_needed_to_extract; 
				file > lhdr.general_purpose_bit_flag; 
				file > lhdr.compression_method; 
				file > lhdr.last_mod_file_time; 
				file > lhdr.last_mod_file_date; 
				file > lhdr.crc32; 
				file > lhdr.compressed_size; 
				file > lhdr.uncompressed_size; 
				file > lhdr.filename_length; 
				file > lhdr.extra_field_length; 

				file.read(fname,lhdr.filename_length);
				fname[lhdr.filename_length] = 0;

				file.seek(lhdr.extra_field_length,XS_CUR);
				fileList.push_back(XZIP_FileHeader());
				fileList.back().set(fname,file.tell(),lhdr.uncompressed_size,&lhdr,sizeof(local_file_header));

				file.seek(lhdr.compressed_size,XS_CUR);
				break;
			case central_file_header_signature:
				file > chdr.version_made_by; 
				file > chdr.version_needed_to_extract; 
				file > chdr.general_purpose_bit_flag; 
				file > chdr.compression_method; 
				file > chdr.last_mod_file_time; 
				file > chdr.last_mod_file_date; 
				file > chdr.crc32; 
				file > chdr.compressed_size; 
				file > chdr.uncompressed_size; 
				file > chdr.filename_length; 
				file > chdr.extra_field_length; 
				file > chdr.file_comment_length; 
				file > chdr.disk_number_start; 
				file > chdr.internal_file_attributes; 
				file > chdr.external_file_attributes; 
				file > chdr.relative_offset_local_header; 

				file.seek(chdr.filename_length,XS_CUR);
				file.seek(chdr.extra_field_length,XS_CUR);
				file.seek(chdr.file_comment_length,XS_CUR);
				break;
			case end_central_dir_signature:
				file > ecr.number_this_disk; 
				file > ecr.number_disk_with_start_central_directory; 
				file > ecr.total_entries_central_dir_on_this_disk; 
				file > ecr.total_entries_central_dir; 
				file > ecr.size_central_directory; 
				file > ecr.offset_start_central_directory; 
				file > ecr.zipfile_comment_length;

				file.seek(ecr.zipfile_comment_length,XS_CUR);
				fl = 1;
				break;
		}
	}
	file.seek(0,XS_BEG);
}

void XZIP_Resource::SaveIndex(void)
{
	XStream fh(idxName.c_str(),XS_OUT);
	//TODO not sure if we should use 64 bits here
	fh < (unsigned long) fileList.size();

	FileList::iterator fi;
	FOR_EACH(fileList, fi)
		fi->save(fh);

	fh.close();
}

XZIP_FileHeader* XZIP_Resource::find(const char* fname)
{
	FileList::iterator fi;
	FOR_EACH(fileList, fi)
		if(!stricmp(fi->name(),fname))
			return &*fi;

	return 0;
}

int XZIP_Resource::open(const char* fname,XStream& fh,int mode)
{
	XZIP_FileHeader* p = find(fname);
	if(p){
		 fh.open(&file,p -> offset(),p -> size());
		 return 1;
	}
	return 0;
}

void XZIP_Resource::dump(const char* fname)
{
	XStream fh(fname,XS_OUT);
	FileList::iterator fi;
	FOR_EACH(fileList, fi)
		fh < fi->name() < " " <= fi->size() < "/" <= fi->offset() < "\r\n";
	fh.close();
}
