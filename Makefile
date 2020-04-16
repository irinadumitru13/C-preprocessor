CC=cl
CFLAGS=/MD /nologo /W3 /D_CRT_SECURE_NO_DEPRECATE /EHsc /Za
EXENAME=so-cpp.exe

build: $(EXENAME)

$(EXENAME): so-cpp.obj hashmap.obj linked_list.obj
	$(CC) $(CFLAGS) /Fe$@ $**

so-cpp.obj: so-cpp.c
	$(CC) $(CFLAGS) /c so-cpp.c

hashmap.obj: hashmap.c
	$(CC) $(CFLAGS) /c hashmap.c

linked_list.obj: linked_list.c
	$(CC) $(CFLAGS) /c linked_list.c

clean: obj_clean exe_clean

obj_clean:
	del *.obj

exe_clean:
	del $(EXENAME)
