#include <filesystem>
#include "fntxml.h"

namespace fnt
{
	bool Info::loadFromXMLElement(tinyxml2::XMLElement* elem)
	{
		if (elem == nullptr || std::string(elem->Name()) != "info")
			return false;

		m_face = elem->Attribute("face");
		m_size = elem->IntAttribute("size");
		m_bBold = elem->BoolAttribute("bold");
		m_bItalic = elem->BoolAttribute("italic");
		m_charset = elem->Attribute("charset");
		m_bUnicode = elem->BoolAttribute("unicode");
		m_stretchH = elem->IntAttribute("stretchH", 100);
		m_bSmooth = elem->BoolAttribute("smooth", false);
		m_aa = elem->IntAttribute("aa", 1);

		const char* padding = elem->Attribute("padding");
		if (padding)
			sscanf_s(padding, "%d,%d,%d,%d", &m_padding[0], &m_padding[1], &m_padding[2], &m_padding[3]);

		const char* spacing = elem->Attribute("spacing");
		if (spacing)
			sscanf_s(spacing, "%d,%d", &m_spacing[0], &m_spacing[1]);

		m_outline = elem->IntAttribute("outline", 0);

		return true;
	}

	bool Common::loadFromXMLElement(tinyxml2::XMLElement* elem)
	{
		if (elem == nullptr || std::string(elem->Name()) != "common")
			return false;

		m_lineHeight = elem->IntAttribute("lineHeight");
		m_base = elem->IntAttribute("base");
		m_scaleW = elem->IntAttribute("scaleW");
		m_scaleH = elem->IntAttribute("scaleH");
		m_pages = elem->IntAttribute("pages");
		m_bPacked = elem->BoolAttribute("packed", false);
		m_alphaChnl = elem->IntAttribute("alphaChnl", 0);
		m_redChnl = elem->IntAttribute("redChnl", 0);
		m_greenChnl = elem->IntAttribute("greenChnl", 0);
		m_blueChnl = elem->IntAttribute("blueChnl", 0);

		return true;
	}

	bool Font::loadFromFile(const std::string& filename)
	{
		std::filesystem::path filepath(filename);

		if (!std::filesystem::exists(filepath))
			return false;

		tinyxml2::XMLDocument doc;

		if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
			return false;
		
		tinyxml2::XMLElement* root = doc.RootElement();
		if (root == nullptr || std::string(root->Name()) != "font")
			return false;
		
		m_info.loadFromXMLElement(root->FirstChildElement("info"));
		m_common.loadFromXMLElement(root->FirstChildElement("common"));

		tinyxml2::XMLElement* pagesElem = root->FirstChildElement("pages");
		if (pagesElem)
		{
			tinyxml2::XMLElement* pageElem = pagesElem->FirstChildElement("page");
			while (pageElem)
			{
				m_pages.push_back(Page(pageElem->IntAttribute("id"), pageElem->Attribute("file")));
				pageElem = pageElem->NextSiblingElement("page");
			}
		}

		tinyxml2::XMLElement* charsElem = root->FirstChildElement("chars");
		if (charsElem)
		{
			m_chars.reserve(charsElem->IntAttribute("count"));

			tinyxml2::XMLElement* charElem = charsElem->FirstChildElement("char");
			while (charElem)
			{
				m_chars.push_back(Char(
					charElem->IntAttribute("id"),
					charElem->IntAttribute("x"),
					charElem->IntAttribute("y"),
					charElem->IntAttribute("width"),
					charElem->IntAttribute("height"),
					charElem->IntAttribute("xoffset"),
					charElem->IntAttribute("yoffset"),
					charElem->IntAttribute("xadvance"),
					charElem->IntAttribute("page"),
					charElem->IntAttribute("chnl")
				));

				charElem = charElem->NextSiblingElement("char");
			}
		}

		return true;
	}

	bool Font::saveToFile(const std::string& filename)
	{
		throw std::runtime_error("Font::saveToFile() - Not implemented");
		return false;
	}
}
