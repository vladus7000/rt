#include "system/FileManager.hpp"
#include <fstream>

namespace rt
{
namespace fs
{

std::string FileManager::m_root = "";

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

bool FileManager::init()
{
	return true;
}

void FileManager::deinit()
{
}

FileDescriptor::Ref FileManager::LoadFileSync(const std::string & fileName)
{
	FileDescriptor::Ref fileDescriptor(std::make_unique<FileDescriptor>());
	fileDescriptor->fileName = m_root + fileName;

	std::ifstream istream(fileDescriptor->fileName, std::ios::binary);

	std::streamoff fileSise = 0;
	istream.seekg(0, istream.end);
	fileSise = istream.tellg();
	istream.seekg(0, istream.beg);

	if (fileSise > 0)
	{
		
		fileDescriptor->fileSize = static_cast<size_t>(fileSise);
		fileDescriptor->data = new unsigned char[fileDescriptor->fileSize];
		fileDescriptor->valid = true;

		istream.read((char*)fileDescriptor->data, fileSise);
	}
	else
	{
		fileDescriptor->valid = false;
	}
	istream.close();

	return fileDescriptor;
}

}
}