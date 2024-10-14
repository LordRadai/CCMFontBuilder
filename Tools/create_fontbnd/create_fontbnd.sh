source "config.txt"

if [ ! -f "$magick" ]; then
	echo "Specified magick folder does not exist ($magick)"
	sleep 2
	exit 1
fi

if [ ! -f "$nvcompress" ]; then
	echo "Specified nvcompress folder does not exist ($nvcompress)"
	sleep 2
	exit 1
fi

if [ ! -f "$witchybnd" ]; then
	echo "Specified WitchyBND folder does not exist ($witchybnd)"
	sleep 2
	exit 1
fi

scripts/convert_dds.sh
if [ $? -eq 1 ]; then
	sleep 2
    exit 1
fi

scripts/create_tpf.sh
if [ $? -eq 1 ]; then
	sleep 2
    exit 1
fi

scripts/create_bnd4.sh
if [ $? -eq 1 ]; then
	sleep 2
    exit 1
fi

rm -r "tmp/"