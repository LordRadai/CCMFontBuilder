magick="C:/Program Files/ImageMagick-7.1.1-Q16-HDRI/magick.exe"
nvcompress="C:/Program Files/NVIDIA Corporation/NVIDIA Texture Tools/nvcompress.exe"
input_folder="bmp"
out_folder="dds"

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