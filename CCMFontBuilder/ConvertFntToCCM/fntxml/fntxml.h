#pragma once
#include <RCore.h>
#include <vector>

namespace fnt
{
	class Char
	{
		int m_id;
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		int m_xoffset;
		int m_yoffset;
		int m_xadvance;
		int m_page;
		int m_chnl;
	public:
		Char() : m_id(0), m_x(0), m_y(0), m_width(0), m_height(0), m_xoffset(0), m_yoffset(0), m_xadvance(0), m_page(0), m_chnl(0) {}
		Char(int id, int x, int y, int width, int height, int xoffset, int yoffset, int xadvance, int page, int chnl)
			: m_id(id), m_x(x), m_y(y), m_width(width), m_height(height), m_xoffset(xoffset), m_yoffset(yoffset), m_xadvance(xadvance), m_page(page), m_chnl(chnl) {}
		~Char() {}

		int getId() const { return m_id; }
		void setId(int id) { m_id = id; }

		int getX() const { return m_x; }
		void setX(int x) { m_x = x; }

		int getY() const { return m_y; }
		void setY(int y) { m_y = y; }

		int getWidth() const { return m_width; }
		void setWidth(int width) { m_width = width; }

		int getHeight() const { return m_height; }
		void setHeight(int height) { m_height = height; }

		int getXOffset() const { return m_xoffset; }
		void setXOffset(int xoffset) { m_xoffset = xoffset; }

		int getYOffset() const { return m_yoffset; }
		void setYOffset(int yoffset) { m_yoffset = yoffset; }

		int getXAdvance() const { return m_xadvance; }
		void setXAdvance(int xadvance) { m_xadvance = xadvance; }

		int getPage() const { return m_page; }
		void setPage(int page) { m_page = page; }

		int getChnl() const { return m_chnl; }
		void setChnl(int chnl) { m_chnl = chnl; }
	};

	class Page
	{
		int m_id;
		std::string m_file;
	public:
		Page() : m_id(0), m_file("") {}
		Page(int id, const std::string& file) : m_id(id), m_file(file) {}
		~Page() {}

		int getId() const { return m_id; }
		void setId(int id) { m_id = id; }
		std::string getFile() const { return m_file; }
		void setFile(const std::string& file) { m_file = file; }
	};

	class Info
	{
		std::string m_face;
		int m_size;
		bool m_bBold;
		bool m_bItalic;
		std::string m_charset;
		bool m_bUnicode;
		int m_stretchH;
		bool m_bSmooth;
		int m_aa;
		int m_padding[4];
		int m_spacing[2];
		int m_outline;

	public:
		Info() : m_face(""), m_size(0), m_bBold(false), m_bItalic(false), m_charset(""), m_bUnicode(false), m_stretchH(100), m_bSmooth(false), m_aa(1), m_padding{ 0,0,0,0 }, m_spacing{ 0,0 }, m_outline(0) {}
		~Info() {}

		std::string getFace() const { return m_face; }
		void setFace(const std::string& face) { m_face = face; }

		int getSize() const { return m_size; }
		void setSize(int size) { m_size = size; }

		bool isBold() const { return m_bBold; }
		void setBold(bool bold) { m_bBold = bold; }

		bool isItalic() const { return m_bItalic; }
		void setItalic(bool italic) { m_bItalic = italic; }

		std::string getCharset() const { return m_charset; }
		void setCharset(const std::string& charset) { m_charset = charset; }

		bool isUnicode() const { return m_bUnicode; }
		void setUnicode(bool unicode) { m_bUnicode = unicode; }

		int getStretchH() const { return m_stretchH; }
		void setStretchH(int stretchH) { m_stretchH = stretchH; }

		bool isSmooth() const { return m_bSmooth; }
		void setSmooth(bool smooth) { m_bSmooth = smooth; }

		int getAA() const { return m_aa; }
		void setAA(int aa) { m_aa = aa; }

		void getPadding(int padding[4]) const { for (int i = 0; i < 4; ++i) padding[i] = m_padding[i]; }
		void setPadding(int padding[4]) { for (int i = 0; i < 4; ++i) m_padding[i] = padding[i]; }
		void getSpacing(int spacing[2]) const { for (int i = 0; i < 2; ++i) spacing[i] = m_spacing[i]; }
		void setSpacing(int spacing[2]) { for (int i = 0; i < 2; ++i) m_spacing[i] = spacing[i]; }

		int getOutline() const { return m_outline; }
		void setOutline(int outline) { m_outline = outline; }

		bool loadFromXMLElement(tinyxml2::XMLElement* elem);
	};

	class Common
	{
		int m_lineHeight;
		int m_base;
		int m_scaleW;
		int m_scaleH;
		int m_pages;
		bool m_bPacked;
		int m_alphaChnl;
		int m_redChnl;
		int m_greenChnl;
		int m_blueChnl;
	public:
		Common() : m_lineHeight(0), m_base(0), m_scaleW(0), m_scaleH(0), m_pages(0), m_bPacked(false), m_alphaChnl(0), m_redChnl(0), m_greenChnl(0), m_blueChnl(0) {}
		~Common() {}

		int getLineHeight() const { return m_lineHeight; }
		void setLineHeight(int lineHeight) { m_lineHeight = lineHeight; }

		int getBase() const { return m_base; }
		void setBase(int base) { m_base = base; }

		int getScaleW() const { return m_scaleW; }
		void setScaleW(int scaleW) { m_scaleW = scaleW; }

		int getScaleH() const { return m_scaleH; }
		void setScaleH(int scaleH) { m_scaleH = scaleH; }

		int getPages() const { return m_pages; }
		void setPages(int pages) { m_pages = pages; }

		bool isPacked() const { return m_bPacked; }
		void setPacked(bool packed) { m_bPacked = packed; }

		int getAlphaChnl() const { return m_alphaChnl; }
		void setAlphaChnl(int alphaChnl) { m_alphaChnl = alphaChnl; }

		int getRedChnl() const { return m_redChnl; }
		void setRedChnl(int redChnl) { m_redChnl = redChnl; }

		int getGreenChnl() const { return m_greenChnl; }
		void setGreenChnl(int greenChnl) { m_greenChnl = greenChnl; }

		int getBlueChnl() const { return m_blueChnl; }
		void setBlueChnl(int blueChnl) { m_blueChnl = blueChnl; }

		bool loadFromXMLElement(tinyxml2::XMLElement* elem);
	};

	class Font
	{
		Info m_info;
		Common m_common;
		std::vector<Page> m_pages;
		std::vector<Char> m_chars;
	public:
		Font() {};
		~Font() {}

		bool loadFromFile(const std::string& filename);
		bool saveToFile(const std::string& filename);

		const Info& getInfo() const { return m_info; }
		const Common& getCommon() const { return m_common; }
		const std::vector<Page>& getPages() const { return m_pages; }
		const std::vector<Char>& getChars() const { return m_chars; }
	};
}
