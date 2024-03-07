if [ ! -d "$magick" ]; then
	echo "Specified nvcompress folder does not exist ($magick)"
	exit 1
fi

if [ ! -d "$nvcompress" ]; then
	echo "Specified nvcompress folder does not exist ($nvcompress)"
	exit 1
fi

if [ -d "$witchybnd" ]; then
	echo "Specified WitchyBND folder does not exist ($witchybnd)"
	exit 1
fi

scripts/convert_dds.sh
if [ $? -eq 1 ]; then
    exit 1
fi

scripts/create_tpf.sh
if [ $? -eq 1 ]; then
    exit 1
fi

scripts/create_bnd4.sh
if [ $? -eq 1 ]; then
    exit 1
fi

rm -r "tmp/"