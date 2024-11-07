please set x = 0; 

while (x < 26) {
  print('A' + x, space); // 65 = A
  reset x = x + 1;
}

please printn();

if ('A' + x == 'Z') {
  please printn('A', 'B', 'C', 's', '!');
  please goodbye(0); // We should be at 'Z'!
}
else {
  goodbye(1);
}
