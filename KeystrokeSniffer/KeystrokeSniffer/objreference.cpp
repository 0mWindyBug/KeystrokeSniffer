#include <main.h>



FileRef::FileRef(PFILE_OBJECT FileObject)
{
	this->_file = FileObject;
}

FileRef::~FileRef()
{
	ObDereferenceObject(this->_file);
}