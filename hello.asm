decl @getint(): i32
decl @getch(): i32
decl @getarray(*i32): i32
decl @putint(i32)
decl @putch(i32)
decl @putarray(i32, *i32)
decl @starttime()
decl @stoptime()

fun @main(): i32 {
%entry:
  @a_1 = alloc i32
  store 1, @a_1
  %1 = load @a_1
  %2 = eq %1, 2
  %3 = load @a_1
  %4 = eq %3, 3
  %5 = or %2, %4
  %6 = ne %5, 0
	br %6, %then6, %else6

%then6:
  ret 0

%else6:
  %7 = load @a_1
  %8 = add %7, 1
  ret %8

%end6:
}

