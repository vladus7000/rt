#pragma once

#include <string>
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
			FileManager();
			~FileManager();

			bool init();
			void deinit();

			static const std::string& getRoot() { return m_root; }
			static void setRoot(const std::string& root) { m_root = root; }

			static FileDescriptor::Ref LoadFileSync(const std::string& fileName);
		private:
			static std::string m_root;
		};
	} // fs
} // rt