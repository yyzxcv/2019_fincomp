import numpy as np
import math

S = input("Spot price: ")
X = input("Strike price: ")
H = input("Barrier price: ")
T = input("Years: ")
r = input("Risk-free interest rate: ")
s = input("Volatility: ")
n = input("Number of periods: ")
k = input("Number of simulation paths: ")
dt = float(T) / n
R = np.exp(r * dt)

# Stock price table
P = np.zeros((k, n+1))
P[:, 0] = S
for i in range(1, n+1):
    coef1 = s * np.sqrt(dt) * np.random.standard_normal(k)
    coef2 = (r - s*s/2) * dt
    P[:, i] = P[:, i-1] * np.exp(coef1 + coef2)

# Calculating Average Stock price
SumPrice = np.zeros(k)
AvgPrice = np.zeros((k, n+1))
AvgPrice[:, 0] = S

for i in range(n+1):
    SumPrice += P[:, i]
    AvgPrice[:, i] = SumPrice/(i+1)

# Exercise value table
# Put Option
E = np.where(AvgPrice < X, X - AvgPrice, 0)

# Cash flow table, time = n
CF = E[:, n]

# Hit the Battier, Up and in
B = np.where(AvgPrice[:, n] >= H, E[:, n], 0)

# Backward Induction
for i in range(n-1, 0, -1):
    # Discount
    CF /= R
    B /= R

    # x = Stock price
    path = E[:, i] > 0
    x = AvgPrice[path, i]
    if len(x) == 0:
        continue
    # Discounted future cash flow
    y = CF[path]

    # Least-Square Sum
    coefficient = np.polyfit(x, y, 2)

    # Continuation Value
    Continue = np.polyval(coefficient, x)

    # Exercise value
    Exercise = E[path, i]

    # Consider early exercise
    CF[path] = np.where(Exercise > Continue, Exercise, CF[path])
    B = np.where(AvgPrice[:, i] >= H, CF, B)

if S < H:
    CF = B

# time = 0
CF /= R
Price = np.sum(CF) / k
stderr = np.std(CF) / np.sqrt(k)

if Price < X - S:
    Price = X - S

print("--------------------------")
print("| Price: %.4f           |" % Price)
print("| Standard error: %.4f  |" % stderr)
print("--------------------------")

# S = 100, X = 100, H=105, T = 1, r = 0.05, s = 0.3, n = 252, k=100000 
# put price is 0.6146 and the standard error is 0.0111. 

# S = 100, X = 100, H=101, T = 1, r = 0.1, s = 0.5, n = 252, k=100000 
# put price is 4.6478 and the standard error is 0.0308.