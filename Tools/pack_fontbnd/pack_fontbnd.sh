source "config.txt"

if [ ! -d "input" ]; then
	echo "Missing input folder. Create it before running the tool"
	sleep 2
	exit 1
fi

if [ ! -f "$witchybnd" ]; then
	echo "Specified WitchyBND folder does not exist ($witchybnd)"
	sleep 2
	exit 1
fi

for folder in input/*/*; do
    if [ -d "$folder" ]; then
        echo "Processing $folder"

		"$witchybnd" "$folder" "-s"
    fi
done

for folder in input/*; do
    if [ -d "$folder" ]; then
        echo "Processing $folder"

		"$witchybnd" "$folder" "-s"
    fi
done