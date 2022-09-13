H_DIR=$XIMC_DIR/c-profiles/STANDA
PY_DIR=$XIMC_DIR/python-profiles/STANDA
TMP_DIR=$ARCHIVE_DIR/tmp
mkdir $TMP_DIR

for path in ${H_DIR}/*.h; do
	filename="${path##*/}"
	profname="${filename%.*}"
	tar -rvf ${TMP_DIR}/${profname}.tar -C $H_DIR $filename
done	

for path in ${PY_DIR}/*.py; do
	filename="${path##*/}"
	profname="${filename%.*}"
	tar -rvf ${TMP_DIR}/${profname}.tar -C $PY_DIR $filename
done	

for path in ${CFG_DIR}/*.cfg; do
	filename="${path##*/}"
	profname="${filename%.*}"
	if [ -e $path ]
	then
		tar -rvf ${TMP_DIR}/${profname}.tar -C $CFG_DIR $filename
	else
		echo $path
	fi
done

for path in ${TMP_DIR}/*.tar; do
	gzip $path
	filename="${path##*/}"	
	tar -rvf profile-STANDA.tar -C $TMP_DIR ${filename}.gz
done

gzip profile-STANDA.tar
if [ ! $ARCHIVE_DIR -ef . ]; then
       	mv profile-STANDA.tar.gz $ARCHIVE_DIR
fi
rm -rvf $TMP_DIR
