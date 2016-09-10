obj = dgrecv
obj1 = dgram

$(obj) : $(obj).c $(obj1).c
	cc -o $(obj) $(obj1).c $(obj).c 
