#include "WitchyXML.h"

namespace WitchyManifest
{
	manifestBase::manifestBase(const char* filename, const char* rootName)
		: m_pDoc(nullptr)
	{
		m_pDoc = new tinyxml2::XMLDocument();

		m_pDoc->InsertFirstChild(m_pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\""));

		tinyxml2::XMLElement* root = m_pDoc->NewElement(rootName);

		root->InsertNewChildElement("filename")->SetText(filename);

		m_pDoc->InsertEndChild(root);
	}

	bool manifestBase::save(const char* filepath)
	{
		if (m_pDoc == nullptr)
			return false;

		tinyxml2::XMLError eResult = m_pDoc->SaveFile(filepath);

		if (eResult != tinyxml2::XML_SUCCESS)
			return false;

		return true;
	}

	bnd4Manifest::bnd4Manifest(const char* filename) : manifestBase(filename, "bnd4")
	{
		tinyxml2::XMLElement* root = m_pDoc->FirstChildElement("bnd4");

		root->InsertNewChildElement("compression")->SetText("DCX_DFLT");
		root->InsertNewChildElement("dfltUnk04")->SetText(65536);
		root->InsertNewChildElement("dfltUnk10")->SetText(36);
		root->InsertNewChildElement("dfltUnk14")->SetText(44);
		root->InsertNewChildElement("dfltUnk30")->SetText(9);
		root->InsertNewChildElement("dfltUnk38")->SetText(0);
		root->InsertNewChildElement("version")->SetText("14M18O9");
		root->InsertNewChildElement("format")->SetText("IDs, Names2, Compression");
		root->InsertNewChildElement("bigendian")->SetText("False");
		root->InsertNewChildElement("bitbigendian")->SetText("False");
		root->InsertNewChildElement("unicode")->SetText("False");
		root->InsertNewChildElement("extended")->SetText("0x00");
		root->InsertNewChildElement("unk04")->SetText("False");
		root->InsertNewChildElement("unk05")->SetText("False");
		root->InsertNewChildElement("files");
	}

	bool bnd4Manifest::addFile(const char* flag, int id, const char* filename)
	{
		tinyxml2::XMLElement* filesElem = m_pDoc->FirstChildElement("bnd4")->FirstChildElement("files");

		if (!filesElem)
			return false;

		tinyxml2::XMLElement* fileElem = filesElem->InsertNewChildElement("file");
		fileElem->InsertNewChildElement("flag")->SetText(flag);
		fileElem->InsertNewChildElement("id")->SetText(id);
		fileElem->InsertNewChildElement("path")->SetText(filename);

		return true;
	}

	tpfManifest::tpfManifest(const char* filename) : manifestBase(filename, "tpf")
	{
		tinyxml2::XMLElement* root = m_pDoc->FirstChildElement("tpf");
		root->InsertNewChildElement("compression")->SetText("None");
		root->InsertNewChildElement("encoding")->SetText("0x02");
		root->InsertNewChildElement("flag2")->SetText("0x03");
		root->InsertNewChildElement("platform")->SetText("PC");
		root->InsertNewChildElement("textures");
	}

	bool tpfManifest::addTexture(const char* name, int format)
	{
		tinyxml2::XMLElement* texturesElem = m_pDoc->FirstChildElement("tpf")->FirstChildElement("textures");

		if (!texturesElem)
			return false;

		tinyxml2::XMLElement* textureElem = texturesElem->InsertNewChildElement("texture");
		textureElem->InsertNewChildElement("name")->SetText(name);
		textureElem->InsertNewChildElement("format")->SetText(format);
		textureElem->InsertNewChildElement("flags1")->SetText("0x00");

		return true;
	}
}
