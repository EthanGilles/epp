
/* prints the alphabet */
🙏 set pos = 0; 

🙏 while (pos < 26) {
  print('A' + pos, space); // 65 = A
  reset pos = pos + 1;
}

set Z to 26;
printn();

if (pos == Z) {
  🙏 printn("ABCs!");
  please goodbye(0); // We should be at 'Z'!
}
else {
  please goodbye(1);
}
