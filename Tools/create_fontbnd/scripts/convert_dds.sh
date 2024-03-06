source "config.txt"
input_folder="input"
out_folder="tmp/dds"

if [ -z "$(find "$input_folder" -maxdepth 1 -type f -name "*.bmp")" ]; then
	echo "There are no bmp files in $input_folder"
	exit 1
fi

mkdir -p "$out_folder"

# Loop through each BMP file in the folder
for bmp_file in "$input_folder"/*.bmp; do
    # Extract filename without extension
    filename=$(basename -- "$bmp_file")
    filename_no_ext="${filename%.*}"
    
    # Make black color transparent
    "$magick" "$bmp_file" -transparent black "$input_folder/$filename_no_ext"_transparent.bmp

    # Convert to DDS
    "$nvcompress" -bc3 "$input_folder/$filename_no_ext"_transparent.bmp "$out_folder/$filename_no_ext".dds

    # Optionally remove the intermediate BMP file
    rm "$input_folder/$filename_no_ext"_transparent.bmp
done