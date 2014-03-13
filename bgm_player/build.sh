rm -rf build && \
mkdir build && \
cp -r src configure.ac Makefile.am build && \
cd build && \
gnulib-tool --import --no-cache-modules --no-conditional-dependencies \
	--no-vc-files --no-libtool canonicalize dirname && \
autoreconf -fiv && \
./configure $@ && \
make clean && \
make && \
mv BGM_Player .. && \
cd .. && \
rm -rf build
