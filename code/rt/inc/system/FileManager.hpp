#pragma once

#include <string>
#include <vector>
#include <memory>

#include "System.hpp"

namespace rt
{
	namespace fs
	{
		struct FileDescriptor
		{
			using Ref = std::unique_ptr<FileDescriptor>;
			FileDescriptor()
				: valid{false}
				, fileSize{ 0 }
				, data{nullptr}
			{
			}
			~FileDescriptor()
			{
				if (data)
				{
					delete[] data;
					data = nullptr;
					fileSize = 0;
					valid = false;
					fileName.clear();
				}
			}
			std::string fileName;
			bool valid;
			size_t fileSize;
			unsigned char* data;
		};

		class FileManager
		{
		public:
			bool init();
			void deinit();

			static FileManager& getInstance();

			const std::string& getRoot() { return m_root; }
			void setRoot(const std::string& root) { m_root = root; }

			FileDescriptor::Ref loadFileSync(const std::string& fileName, bool isRelativeToRoot = true);
			std::vector<std::string> getListOfFiles(const std::string& root, bool isRelativeToRoot = true);

		private:
			FileManager();
			~FileManager();
			FileManager& operator=(const FileManager& rhs);
			FileManager(const FileManager& rhs);
			std::string m_root;
		};
	} // fs
} // rt