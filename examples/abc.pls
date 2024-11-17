/* prints the alphabet */
🙏 set pos = 0; 

🙏 while (pos < 26) {
  print('A' + pos, space); // 65 = A
  reset pos to pos + 1;
}

set Z to 26;
printn();

if (pos == Z) {
  set total = str(Z);
  🙏 printn("A-Z is ", total, " characters long.");
  please goodbye(0); // We should be at 'Z'!
}
else {
  please goodbye(1);
}
