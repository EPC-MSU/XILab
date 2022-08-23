H_DIR=$XIMC_DIR/c-profiles/STANDA
PY_DIR=$XIMC_DIR/python-profiles/STANDA

for path in ${H_DIR}/*.h; do
	filename="${path##*/}"
	profname="${filename%.*}"
	tar -rvf ${ARCHIVE_DIR}/${profname}.tar -C $H_DIR $filename
done	

for path in ${PY_DIR}/*.py; do
	filename="${path##*/}"
	profname="${filename%.*}"
	tar -rvf ${ARCHIVE_DIR}/${profname}.tar -C $PY_DIR $filename
done	

for path in ${CFG_DIR}/*.cfg; do
	filename="${path##*/}"
	profname="${filename%.*}"
	if [ -e $path ]
	then
		tar -rvf ${ARCHIVE_DIR}/${profname}.tar -C $CFG_DIR $filename
	else
		echo $path
	fi
done

for path in ${ARCHIVE_DIR}/*.tar; do
	gzip $path
	tar -rvf profile-archives.tar ${path}.gz
done

gzip profile-archives.tar
rm -rvf $ARCHIVE_DIR
