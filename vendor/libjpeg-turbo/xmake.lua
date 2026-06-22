-- libjpeg-turbo xmake 配置

target("libjpeg-turbo")
    set_kind("static")
    
    -- 添加源文件（排除工具和测试文件）
    add_files("src/jaricom.c", "src/jcapimin.c", "src/jcapistd.c", "src/jcarith.c")
    add_files("src/jccoefct.c", "src/jccolext.c", "src/jccolor.c", "src/jcdctmgr.c")
    add_files("src/jcdiffct.c", "src/jchuff.c", "src/jcicc.c", "src/jcinit.c")
    add_files("src/jclhuff.c", "src/jclossls.c", "src/jcmainct.c", "src/jcmarker.c")
    add_files("src/jcmaster.c", "src/jcomapi.c", "src/jcparam.c", "src/jcphuff.c")
    add_files("src/jcprepct.c", "src/jcsample.c", "src/jctrans.c", "src/jdapimin.c")
    add_files("src/jdapistd.c", "src/jdarith.c", "src/jdatadst.c", "src/jdatasrc.c")
    add_files("src/jdcoefct.c", "src/jdcol565.c", "src/jdcolext.c", "src/jdcolor.c")
    add_files("src/jddctmgr.c", "src/jddiffct.c", "src/jdhuff.c", "src/jdicc.c")
    add_files("src/jdinput.c", "src/jdlhuff.c", "src/jdlossls.c", "src/jdmainct.c")
    add_files("src/jdmarker.c", "src/jdmaster.c", "src/jdmerge.c", "src/jdmrg565.c")
    add_files("src/jdmrgext.c", "src/jdphuff.c", "src/jdpostct.c", "src/jdsample.c")
    add_files("src/jdtrans.c", "src/jerror.c", "src/jfdctflt.c", "src/jfdctfst.c")
    add_files("src/jfdctint.c", "src/jidctflt.c", "src/jidctfst.c", "src/jidctint.c")
    add_files("src/jidctred.c", "src/jmemmgr.c", "src/jmemnobs.c", "src/jquant1.c")
    add_files("src/jquant2.c", "src/jstdhuff.c", "src/jutils.c", "src/jmorecfg.c")
    add_files("src/transupp.c", "src/cdjpeg.c", "src/jpegint.h")
    
    -- MD5 支持
    add_files("src/md5/md5.c", "src/md5/md5hl.c")
    
    -- 包含目录
    add_includedirs("src")
    
    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")