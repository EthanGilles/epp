/* fibonacci sequence */
PLEASE 
🙏 set nums = {1, 0, 0};
🙏 set limit = 10;


set count = 0;
while (count < limit) {
  reset nums[2] = nums[0] + nums[1];
  reset nums[0] = nums[1];
  reset nums[1] = nums[2];
  reset count = count + 1;
}


// 10th fibonacci number is 55
set answer to 55
if (nums[2] == answer) {
  set str_limit = str(limit);
  set str_fib = str(nums[2]);
  please printn("The ", str_limit, "th fibonacci number is ", str_fib);
}
else {
  please printn("fail");
}
