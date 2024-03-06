# CCMFontBuilder
 This is a command line tool. It loads a list of unicode character codes from `chrlist.txt` and outputs bitmap textures, CCM2 layout file and a plain text layout file for debugging. Usage is `CCMFontBuilder.exe "fontname" size` (e.g. `CCMFontBuilder.exe "MS Gothic" 12`.
 
 It comes with a default chrlist set, but you can create your own by dragging and dropping a CCM2 file on top of `CreateChrListCCM2.exe`.

 If you want to pack the output to a fontbnd.dcx format, copy the output to the `create_fontbnd/input` folder, edit `config.txt` and put there the absolute path to WitchyBND, nvcompress and magick, then run `create_fontbnd.sh`. This requires WitchyBND, NVIDIA Texture Tools and ImageMagick to be installed on your system
