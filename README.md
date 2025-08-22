# CCMFontBuilder
A set of tools to create FromSoftware's DLFontDataCCM2 format.
It has three executables:

## CCMFontBuilder.exe
Usage is `CCMFontBuilder.exe "fontname" size [optional]bBold (default=false) [optional]bItalic (default=false) [optional]kerning (default=0)` (e.g. `CCMFontBuilder.exe "MS Gothic" 12 false false 0`).
It loads a list of unicode character codes from `chrlist.txt` and outputs bitmap textures, CCM2 layout file and a plain text layout file for debugging. 
 
It comes with a default chrlist set, but you can create your own by dragging and dropping an existing CCM2 file on top of `CreateChrListCCM2.exe`.

If you want to pack the output to a fontbnd.dcx format, copy the output to the `create_fontbnd/input` folder, edit `config.txt` and put there the absolute path to WitchyBND, nvcompress and magick, then run `create_fontbnd.sh`. This requires WitchyBND, NVIDIA Texture Tools and ImageMagick to be installed on your system.

## GetChrListFromCCM.exe
Usage is `GetChrListFromCCM.exe "ccmFilePath"` (e.g. `GetChrListFromCCM.exe font.ccm)
It will read the input CCM and write a chrlist.txt file containing the charcodes of characters present in the input ccm. This .txt can be then used by CCMFontBuilder to only create a subset of characters.

## ConvertFntToCCM.exe
Usage is `ConvertFntToCCM.exe "fntFolder"` (e.g `ConvertFntToCCM.exe "MSUIGothic12").
Note that the input folder **must** contain a valid .fnt (in .xml format) and .dds textures, otherwise the tool will fail.

This will produce an `output` folder with a subfolder with the same name as the .fnt filename in the input folder.

If you want to pack the output to a fontbnd.dcx format, copy the output to the `pack_fontbnd/input` folder, edit `config.txt` and put there the absolute path to WitchyBND, then run `pack_fontbnd.sh`. This requires WitchyBND to be installed on your system.

# NOTICE
To generate bitmap fonts for game use, I recommend you use [/url=https://angelcode.com/products/bmfont/]bmfont[/url] alongside `ConvertFntToCCM`.
