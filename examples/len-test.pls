please set list = {1, 2, 3};
set x = 2;
set str_length = str(len("HELLO") + len(x) + len(list));

please set another = [100, 'a'];
please set print_len = str(len(another));

please printn("length of all list items: ", str_length);
printn(print_len);
print(another);

goodbye(0);
