source "config.txt"
input_folder="tmp/dds"
font_folder="tmp/font"

if [ ! -f "$witchybnd" ]; then
	echo "Specified WitchyBND folder does not exist ($witchybnd)"
	exit 1
fi

if [ -z "$(find "$input_folder" -maxdepth 1 -type f -name "*.dds")" ]; then
	echo "There are no dds files in $input_folder"
	exit 1
fi

mkdir -p "$font_folder"

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<tpf>
  <filename>font.tpf</filename>
  <compression>None</compression>
  <encoding>0x02</encoding>
  <flag2>0x03</flag2>
  <platform>PC</platform>
  <textures>" > "$font_folder/_witchy-tpf.xml"

cp "$input_folder"/* "$font_folder"/

for dds_file in "$input_folder"/*.dds; do
	name=$(basename -- "$dds_file")
	
	xml_content="	<texture>
		<name>$name</name>
		<format>1</format>
		<flags1>0x00</flags1>
	</texture>"

	echo "$xml_content" >> "$font_folder/_witchy-tpf.xml"
done

echo "  </textures>
</tpf>" >> "$font_folder/_witchy-tpf.xml"

"$witchybnd" "$font_folder"