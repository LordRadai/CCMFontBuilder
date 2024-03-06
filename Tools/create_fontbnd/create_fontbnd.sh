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