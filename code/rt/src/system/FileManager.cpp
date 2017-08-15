#include "stdafx.h"
#include "system/FileManager.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace rt
{
namespace fs
{

FileManager::FileManager()
	: m_root("")
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

FileManager& FileManager::getInstance()
{
	static FileManager manager;
	return manager;
}

FileDescriptor::Ref FileManager::loadFileSync(const std::string & fileName, bool isRelativeToRoot)
{
	FileDescriptor::Ref fileDescriptor(std::make_unique<FileDescriptor>());

	if (isRelativeToRoot)
	{
		fileDescriptor->fileName = m_root + fileName;
	}
	else
	{
		fileDescriptor->fileName = fileName;
	}

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

void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::vector<std::string> FileManager::getListOfFiles(const std::string& directoryRoot, bool isRelativeToRoot)
{
	std::vector<std::string> ret;
	std::string root;
	if (isRelativeToRoot)
	{
		root = m_root + directoryRoot;
	}
	else
	{
		root = directoryRoot;
	}
	
	for (auto& p : std::experimental::filesystem::directory_iterator(root))
	{
		std::wstring w(p.path().c_str());
		std::string s(w.begin(), w.end());
		
		ReplaceStringInPlace(s, "\\", "/");

		ret.push_back(std::move(s));
		std::cout << p << std::endl;
	}

	return std::move(ret);
}

}
}