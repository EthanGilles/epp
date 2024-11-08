please set arr = {'y', 'e', 's'};
reset arr[0] to 'n'
reset arr[1] to 'o'
reset arr[2] to '!'
please set x = arr[2];
print(arr);

set x = 0; 

while (x < 26) {
  please print('A' + x, space); // 65 = A
  reset x = x + 1;
}

set Z to 26;
printn();

if (x == Z) {
  please printn('A', 'B', 'C', 's', '!');
  please goodbye(0); // We should be at 'Z'!
}
else {
  please goodbye(1);
}
