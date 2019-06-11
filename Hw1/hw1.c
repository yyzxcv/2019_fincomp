#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX 1000000
//Spot rate curve: 0.08 - 0.05 * exp(-0.18 * t)

int main()
{
	double S, K, s, T, n;
	printf("Please enter:\nS(spot price), K(strile price), s(volatility), T(years), n(number of periods)\n");
	scanf("%lf%lf%lf%lf%lf", &S, &K, &s, &T, &n);

	//variables
	double dt = T / n;
	double u = exp(s * sqrt(dt));
	double d = 1 / u;

	//Calculate Put price 
	double put_price;
	double init_price;
	double price[MAX] = {0};
	double p1, p2;
	int now = 0;

	double r;
	double p;
	double future_r;
	double close_r;

	for(int time = n; time >= 0; time --)
	{
		//Compute forward rate
		future_r = 0.08 - 0.05 * exp(-0.18 * (double)(time+1) * dt);
		close_r  = 0.08 - 0.05 * exp(-0.18 * (double)(time) * dt);
		r = (pow((1+future_r), (time+1)) / pow((1+close_r), time)) - 1;
		//printf("now: %d  r: %lf\n", now, r);

		for(now = 0; now <= time; now ++)
		{
			init_price = price[0];

			p = (exp(r * dt) - d) / (u - d);
			p1 = K - (pow(u, (time - now)) * pow(d, now) * S);
            p2 = exp(-(r * dt)) * (price[now] * p + price[now + 1] * (1 - p));
            //printf("%lf %lf\n", p1, p2);
           	price[now] = (p1 > p2)? p1: p2;
           	if(now >= MAX)
           		break;
		}
	}
	printf("--------------------------\n");
	printf("| put price is %lf |\n", price[0]);

	//Compute delta
	double delta = (init_price - price[1] ) / ( S * u - S * d );
    printf( "| delta is %lf     |\n" , delta);
    printf("--------------------------\n");
	return 0;
}
// put price is 10.3488 and the delta is −0.4147.