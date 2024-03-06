witchybnd="F:/Programmi/Dark Souls Modding/WitchyBND/WitchyBND.exe"
input_folder="dds"

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<tpf>
  <filename>font.tpf</filename>
  <compression>None</compression>
  <encoding>0x02</encoding>
  <flag2>0x03</flag2>
  <platform>PC</platform>
  <textures>" >> "_witchy-bnd4.xml"

for dds_file in "$input_folder"/*.dds; do
	name=$(basename -- "$dds_file")
	
	xml_content="	<texture>
		<name>$name</name>
		<format>1</format>
		<flags1>0x00</flags1>
	</texture>"

	echo "$xml_content" >> "_witchy-bnd4.xml"
done

echo "  </textures>
</tpf>" >> "_witchy-bnd4.xml"