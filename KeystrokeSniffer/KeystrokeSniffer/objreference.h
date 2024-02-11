#pragma once
#include <main.h>

struct FileRef {
	FileRef(PFILE_OBJECT FileObject);
	~FileRef();

private:
	PFILE_OBJECT _file;
};