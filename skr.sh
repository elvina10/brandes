f()
 {
 for((i = 0; i < 50; i++))
  do
  time ./brandes $1 < wiki-vote-sort.txt > o
  done;
 }

for((j = 1; j <= 8; j++))
  do
  echo $j : >&2  
  f $j;
  done;
