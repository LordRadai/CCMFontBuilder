#pragma once
#include <RCore.h>

namespace WitchyManifest
{
	class manifestBase
	{
	protected:
		tinyxml2::XMLDocument* m_pDoc;
	public:
		manifestBase(const char* filename, const char* rootName);
		virtual ~manifestBase() { delete m_pDoc; }

		bool save(const char* filepath);
	};

	class bnd4Manifest : public manifestBase
	{
	public:
		bnd4Manifest(const char* filename);

		bool addFile(const char* flag, int id, const char* filename);
	};

	class tpfManifest : public manifestBase
	{
	public:
		tpfManifest(const char* filename);

		bool addTexture(const char* name, int format = 1);
	};
}
