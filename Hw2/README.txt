# Programming Language
  gcc 4.2.1
  
# Compile and run
  gcc hw2.c -o hw2
  ./hw2            
	
# Note
  Enter S(spot price), X(strike price), H(barrier price), T(years), r(risk-free interest rate)
	s(annualized volatility), n(number of periods), k(number of buckets) in order.
  Then, it will output the put price and delta.

- 一開始 time = n 的部分，照著 Amij （p435）的部分進行
  接著照著 p438 這幾頁的部分算 new average, Cu, Cd, 但結果跑出來是錯的QAQ
  也是因為這次開始的比較晚（適逢六日跟著球隊打盃賽，來不及de完bug 嗚嗚

- 4/24 15:00 deBug：
  在上述的程式中，把 BOPM (time = n) 的部分跟 Asian Option 混在一起了，沒有分開
  將兩棵樹分開，重新計算結果，就正確了。
