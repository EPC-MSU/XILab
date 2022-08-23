H_DIR=$XIMC_DIR/c-profiles/STANDA
PY_DIR=$XIMC_DIR/python-profiles/STANDA

for path in ${H_DIR}/*.h; do
	filename="${path##*/}"
	profname="${filename%.*}"
	zip -j ${ARCHIVE_DIR}/${profname}.zip $path
done	

for path in ${PY_DIR}/*.py; do
	filename="${path##*/}"
	profname="${filename%.*}"
	zip -j ${ARCHIVE_DIR}/${profname}.zip $path
done	

for path in ${CFG_DIR}/*.cfg; do
	filename="${path##*/}"
	profname="${filename%.*}"
	zip -j ${ARCHIVE_DIR}/${profname}.zip $path
done

zip -r profile-archives.zip ${ARCHIVE_DIR}
rm ${ARCHIVE_DIR}/*
mv profile-archives.zip ${ARCHIVE_DIR}/
