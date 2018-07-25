
OBJ = o/rlint.a o/menu.a o/control.a o/bundle.a


rlint : $(OBJ)
	$(RM) o/menu.root o/control.root o/bundle.root
	iix link o/rlint o/menu o/control o/bundle keep=$@


o/rlint.a : rlint.c rlint.h
o/menu.a : menu.c rlint.h
o/control.a : control.c rlint.h
o/bundle.a : bundle.c rlint.h

o :
	mkdir $@

o/%.a : %.c | o
	iix compile $< keep=o/$*