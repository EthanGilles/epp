set arr to {'A', 'B', 'C', 'D'};

reset arr[1] to 'Z';
printn(arr[1]);


🙏 set x = 0; 

🙏 while (x < 26) {
  print('A' + x, space); // 65 = A
  reset x = x + 1;
}

set Z to 26;
printn();

if (x == Z) {
  🙏 printn('A', 'B', 'C', 's', '!');
  please goodbye(0); // We should be at 'Z'!
}
else {
  please goodbye(1);
}
