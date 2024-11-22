
set abcs = ['A'..'Z'];
set stmt = true;

/* 
   Instead of using {elif}🤮 like python does, or 
   using {else if}🤮  like C or Java does, we use elsif😎 
   brackets are mandatory though, even if there is just one statement.
   One of the features I'd like to implement is no brackets required for 
   one-liners 
*/

if (abcs[0] == 'B') {
  please printn("B");
}
elsif (stmt == false) {
  please printn("A");
}
else {
  please printn("C");
}


if (abcs[0] == 'B') {
  please printn("B");
} elsif (stmt) {
  please printn("A");
}
