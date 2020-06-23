import numpy as np
import math

x = int(input("x (year): "))			# year of option
y = int(input("y (year): ")) 			# year of bond
r = float(input("r (%): ")) / 100		# initial short rate
b = float(input("b (%): ")) / 100		# betta
m = float(input("m (%): ")) / 100		# mu
s = float(input("s (%): ")) / 100		# sigma (volatility)
n = int(input("n: "))					# number of partition# 
X = float(input("X (%/Par): ")) / 100	# Strike price

# p1096
dt = y/n
dx = np.sqrt(dt)

# x(r) = 2*sqrt(r)/volatility
x_r = 2 * np.sqrt(r)/s
gamma = np.sqrt(b*b+2*s*s)

# Option Duration
D = n*x/y

# Initial Value
Price  = np.ones(n+1)
Option = np.zeros((n+1, n+1))
t = y

# Binomial CIR
for j in range(n-1, -1, -1):
	t = t - dt
	for i in range(j+1):
		# construct a tree for x
		# then, r = f(x) = x^2 * s^2 / 4
		node_x = x_r + (j-2*i) * dx
		node_r = (node_x*node_x) * (s*s) / 4

		# probability for each node
		if node_r == 0:
			p = 1
		else:
			# r+ = f(x+dx); r- = f(x-dx)
			# p(r) = [b*(m-r)*dt + (r+) - (r-)] / [(r+) - (r-)]
			node_r_u = ((node_x+dx)*(node_x+dx)) * (s*s) / 4
			node_r_d = ((node_x-dx)*(node_x-dx)) * (s*s) / 4
			p = (b * (m-node_r) * dt + (node_r-node_r_d)) / (node_r_u-node_r_d)
			if p < 0:
				p = 0
			elif p > 1:
				p = 1

		discount = 1 / math.exp(node_r * dt)

		# Price Tree
		if j <= D:
			tmp = math.exp(gamma * (y-t)) - 1
			A = pow((2*gamma*math.exp((b+gamma)*(y-t)/2) / ((b+gamma)*tmp+2*gamma)), 2*b*m/s/s)
			B = 2*tmp / ((b+gamma)*tmp+2*gamma)	

			Price[i] = A * math.exp(-B*node_r)
			cont = (p*Option[j+1][i]+(1.0-p)*Option[j+1][i+1])*discount
			Option[j][i] = max(X-Price[i], cont, 0)

price = 100 * Option[0][0]
print("------------------")
print("%.4f" %(price), end = "")
print("(% "+"of Par)")

# reference: https://mropengate.blogspot.com/2016/06/american-put-options-on-zero-coupon.html