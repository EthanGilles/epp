PLEASE PLEASE PLEASE
set abcs = ['A'..'Z'];

for (set i = 0; i < 20; reset i += 1) {
  set str_i = str(i);
  printn(str_i);
  please set br = 0;
  if (i == 10) {
    please reset br = 1;
    set str_br = str(br);
    printn(str_br);
  }
  elsif (i == 13) {
    please reset br = 2;
    set str_br = str(br);
    printn(str_br);
  }
  else {
    set x = 1;
    reset x = 2;
    reset x = 3;
  }
}


/*
TODO:
 - push next release to main

NEXT FEATURES :
 - Functions?


{
  set i = 0; 
  while(i < 26) 
  {
    // stmts
    reset i += 1;
  }
}
 */
