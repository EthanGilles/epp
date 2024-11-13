
/* fibonacci sequence */
PLEASE 
🙏 set nums = {1, 0, 0};
🙏 set limit = 10;


🙏 set expression to (10 * 2 / 5) - 7 * 32 + (6 / 3 + 8) % 10 < 5;

set count = 0;
while (count < limit) {
  reset nums[2] = nums[0] + nums[1];
  reset nums[0] = nums[1];
  reset nums[1] = nums[2];
  reset count = count + 1;
}

// 10th fibonacci number is 55
if (nums[2] == 55) {
  please printn("pass");
}
else {
  please printn("fail");
}

goodbye(0);
