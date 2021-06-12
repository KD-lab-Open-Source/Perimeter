@echo off
del Libs\_xtool.h
copy /b xtool.title + xtcore.h + xconsole.h + xbuffer.h + xstream.h + xerrhand.h + xrec.h + xkey.h + xutl.h + xcpuid.h + xzip.h + xt_list.h + zip_resource.h Libs\_xtool.h


del Libs\xutil.h
copy /b xtool.title + xbuffer.h + xstream.h + xerrhand.h + xutl.h + xcpuid.h + xzip.h Libs\xutil.h

copy /b XMath\xmath.h Libs\xmath.h