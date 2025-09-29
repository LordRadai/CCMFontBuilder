#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>
#include <filesystem>
#include "RCore.h"
#include "fntxml/fntxml.h"
#include "WitchyXML/WitchyXML.h"

RLog* g_log;

FontDataCCM2::FontDataCCM2* convertFntToCCM(const fnt::Font& fnt)
{
    g_log->debugMessage(MsgLevel_Info, "Creating ccm file from fnt xml...\n");

    if (fnt.getCommon().getScaleH() != fnt.getCommon().getScaleW())
        return nullptr;

    FontDataCCM2::FontDataCCM2* pCCM = FontDataCCM2::FontDataCCM2::create(fnt.getCommon().getLineHeight(), fnt.getCommon().getScaleH());

    if (pCCM == nullptr || !pCCM->getInitStatus())
    {
        g_log->debugMessage(MsgLevel_Error, "Failed to create CCM font data\n");
        return nullptr;
    }

    for (const auto& ch : fnt.getChars())
    {
		int charId = ch.getId();

        FontDataCCM2::TexRegion* pTexRegion = FontDataCCM2::TexRegion::create(ch.getX(), ch.getY(), ch.getX() + ch.getWidth(), ch.getY() + ch.getHeight());
        if (pTexRegion == nullptr)
        {
            g_log->debugMessage(MsgLevel_Error, "Failed to create TexRegion for char %d\n", charId);
            pCCM->destroy();
            return nullptr;
        }

        FontDataCCM2::Glyph* pGlyph = FontDataCCM2::Glyph::create(charId, pCCM->getNumGlyphs(), ch.getPage(), ch.getXOffset(), ch.getWidth(), ch.getXAdvance(), pTexRegion);
        if (pGlyph == nullptr)
        {
            g_log->debugMessage(MsgLevel_Error, "Failed to create Glyph for char %d\n", charId);
            pCCM->destroy();
            return nullptr;
        }

        g_log->debugMessage(MsgLevel_Info, "Adding glyph for char %d\n", charId);

        pCCM->addGlyph(pGlyph);
    }

	pCCM->setNumTextures(fnt.getCommon().getPages());

	g_log->debugMessage(MsgLevel_Info, "CCM font data created with %d glyphs and %d textures\n", pCCM->getNumGlyphs(), pCCM->getNumTextures());

	return pCCM;
}

int main(int argc, char** argv)
{
	g_log = new RLog(MsgLevel_Debug, "ConvertFntToCCM.log", "ConvertFntToCCM");

    if (argc < 2)
    {
        g_log->debugMessage(MsgLevel_Error, "Usage: %s \"fntDir\"\n", argv[0]);
        delete g_log;
        return 0;
    }

	const std::filesystem::path fntDir = argv[1] + std::string("\\");

    g_log->debugMessage(MsgLevel_Info, "%s %s\n", argv[0], argv[1]);

	const std::filesystem::path outdir = "output\\" + fntDir.parent_path().filename().string();

    std::filesystem::path fntFile = "";

	// Find the first .fnt file in the directory
    for (const auto& entry : std::filesystem::directory_iterator(fntDir.parent_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".fnt")
        {
            fntFile = entry.path().string();
            break;
        }
	}

    g_log->debugMessage(MsgLevel_Info, "Converting FNT file %s to CCM format\n", fntFile.string().c_str());

	fnt::Font fnt;
    if (!fnt.loadFromFile(fntFile.string()))
    {
        g_log->debugMessage(MsgLevel_Error, "Failed to load FNT file %s\n", fntFile.c_str());
		delete g_log;
		return 0;
    }

	std::filesystem::create_directories(outdir);

	g_log->debugMessage(MsgLevel_Info, "Creating output directory %s and copying textures...\n", outdir.string().c_str());

	// Copy all files from the source directory to the output directory
    for (const auto& entry : std::filesystem::directory_iterator(fntDir.parent_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".dds")
        {
			std::filesystem::path filename = entry.path().filename();
			std::filesystem::path filenameNoExt = filename.stem();
			std::filesystem::path copyDir = outdir / filenameNoExt;
			std::filesystem::path copyPath = copyDir / filename;

            WitchyManifest::tpfManifest tpf(std::string(filenameNoExt.string() + ".tpf").c_str());
			tpf.addTexture(filename.string().c_str());

			std::filesystem::create_directories(copyPath.parent_path());

			tpf.save(std::string(copyDir.string() + "\\_witchy-tpf.xml").c_str());

            g_log->debugMessage(MsgLevel_Info, "Added file %s to tpf manifest\n", filename.string().c_str());

            std::filesystem::copy_file(entry.path(), copyPath, std::filesystem::copy_options::overwrite_existing);

            g_log->debugMessage(MsgLevel_Info, "Copied texture %s to %s\n", entry.path().string().c_str(), copyPath.string().c_str());
        }
    }

    // Create and convert the fnt to ccm, then save it
	FontDataCCM2::FontDataCCM2* pCCM = convertFntToCCM(fnt);

    if (pCCM == nullptr)
    {
        pCCM->destroy();

        delete g_log;
        return 1;
    }

    try
    {
        pCCM->save(RString::toWide(outdir.string() + "\\" + fntFile.replace_extension().filename().string() + ".ccm"));
    }
    catch (const std::exception& e)
    {
		g_log->debugMessage(MsgLevel_Error, "Failed to save CCM file %s: %s\n", outdir.string().c_str(), e.what());
    }

    pCCM->destroy();

	WitchyManifest::bnd4Manifest bnd4(std::string(fntFile.replace_extension().filename().string() + ".fontbnd.dcx").c_str());
    int fileID = 0;
    for (const auto& entry : std::filesystem::directory_iterator(outdir))
    {
        if (entry.is_regular_file())
        {
            if (entry.path().extension() != ".xml" && entry.path().extension() != ".bak")
            {
                std::string filename = entry.path().filename().string();
                bnd4.addFile("Flag1", fileID, filename.c_str());
                fileID++;
            }
        }
        else if (entry.is_directory())
        {
            std::string filename = entry.path().filename().string() + ".tpf";
            bnd4.addFile("Flag1", fileID, filename.c_str());
            fileID++;
        }
        else
            continue;

        g_log->debugMessage(MsgLevel_Info, "Added file %s to bnd4 manifest\n", entry.path().filename().string().c_str());
    }

	bnd4.save(std::string(outdir.string() + "\\_witchy-bnd4.xml").c_str());

	g_log->debugMessage(MsgLevel_Info, "Successfully created %s\n", outdir.string().c_str());

    delete g_log;
    return 1;
}