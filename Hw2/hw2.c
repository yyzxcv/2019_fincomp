#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define max(a,b) (a > b)? a:b
#define MAX 10000

typedef struct first{
    double Amin;
    double Amax;
    double diff;
    double price[MAX];
}LNode;

typedef struct second{
    LNode node[MAX];
}NODE;

int main()
{
    double S;   //spot price
    double X;   //strike price
    double H;   //barrier price
    double T;   //years
    double r;   //risk-free rate
    double s;   //annualized voltality
    int n;      //number of periods
    double k;   //number of buckets
    printf("Please enter:\nS, X, H, T, r, s, n, k\n");
    scanf("%lf%lf%lf%lf%lf%lf%d%lf", &S, &X, &H, &T, &r, &s, &n, &k);

    //variables
    int i, j, l;
    double dt = T / n;
    double u = exp(s * sqrt(dt));   //S_up = S * u
    double d = 1 / u;               //S_down = S * d
    double R = exp(r * dt);         //p280
    double p = (R - d)/(u - d);     //p244

    //structure
    NODE *node_1 = malloc(MAX * sizeof(NODE));
    NODE *node_2 = malloc(MAX * sizeof(NODE));

    //Computed the value for all nodes at the last step (time = n, (n+1) nodes, BOPM)
    double sum_h;
    double sum_l;
    double Amji;

    //BOPM, the last ime step
    for(i = 0; i <= n; i++)
    {
        sum_h = 0;
        sum_l = 0;

        for (j = 0; j <= (n - i); j++)
            sum_h += S * (pow(u , j));
        for (; j <= n; j++)
            sum_h += S * (pow(u , (n - i)) * pow(d , (j - n + i)));
        for (j = 0; j <= i ; j++ )
            sum_l += S * (pow (d , j));
        for (; j <= n; j++)
            sum_l += S * (pow(d , i) * pow(u , (j - i)));

        node_2[n].node[i].Amin = node_1[n].node[i].Amin = sum_l/(n + 1);
        node_2[n].node[i].Amax = node_1[n].node[i].Amax = sum_h/(n + 1);
        node_2[n].node[i].diff = node_1[n].node[i].diff = ( sum_h - sum_l ) / (k * (n + 1));
        
        //Calculating price, p435
        for(l = k ; l >= 0 ; l--)
        {
            Amji = (k - l)/k * node_1[n].node[i].Amin + (l/k) * node_1[n].node[i].Amax;
            if(Amji < H) //p384, An up-and-in is a put knock-in option that comes into existence only when the barrier is reached and H > S.
                node_1[n].node[i].price[l] = 0;
            else
                node_1[n].node[i].price[l] = max(X - Amji, 0);  //put
            node_2[n].node[i].price[l] = max(X - Amji, 0);
            //printf("%lf %lf\n", node_1[n].node[i].price[l], node_2[n].node[i].price[l]);
        }
    }

    int time;
    for (time = (n-1); time >= 0; time-- )
    {
        for( i = 0 ; i <= time ; i++ )
        {
            sum_h = 0;
            sum_l = 0;

            for (j = 0; j <= (time - i); j++)
                sum_h += S * (pow(u , j));
            for (; j <= time; j++)
                sum_h += S * (pow(u , (time - i)) * pow(d , (j - time + i)));
            for (j = 0; j <= i ; j++ )
                sum_l += S * (pow (d , j));
            for (; j <= time; j++)
                sum_l += S * (pow(d , i) * pow(u , (j - i)));

            node_2[time].node[i].Amin = node_1[time].node[i].Amin = sum_l/(time + 1);
            node_2[time].node[i].Amax = node_1[time].node[i].Amax = sum_h/(time + 1);
            node_2[time].node[i].diff = node_1[time].node[i].diff = ( sum_h - sum_l ) / (k * (time + 1));
        
            double avg, new_avg, avg_down;
            double up_price, down_price;
            int bucket_l, n_bucket_l;
            double upp, nupp; 
            double ddown, nddown;
            double P_up, n_P_up;
            double P_down, n_P_down;
            double P_total, n_P_total;

            //calculating the price
            for ( l = k ; l >= 0 ; l-- )
            {
                avg = node_1[time].node[i].Amin + l * node_1[time].node[i].diff;

                //up
                up_price = S * pow( u, (time - i + 1)) * pow( d , (i));
                new_avg = ( avg * ( time + 1 ) + up_price ) / (time + 2);

                if( node_1[time+1].node[i].diff != 0 )
                {
                    bucket_l = ceil( (new_avg - node_1[time+1].node[i].Amin) / node_1[time+1].node[i].diff );
                    n_bucket_l = ceil( (new_avg - node_2[time+1].node[i].Amin) / node_2[time+1].node[i].diff );
                    
                    if( bucket_l <= 0)      bucket_l = 1;
                    if( bucket_l > k)       bucket_l = k;
                    if( n_bucket_l <= 0)    n_bucket_l = 1;
                    if( n_bucket_l > k)     n_bucket_l = k;

                    upp = ( bucket_l * node_1[time+1].node[i].diff + node_1[time+1].node[i].Amin - new_avg ) / node_1[time+1].node[i].diff;
                    nupp = ( n_bucket_l * node_2[time+1].node[i].diff + node_2[time+1].node[i].Amin - new_avg ) / node_2[time+1].node[i].diff;
                }
                else
                {
                    bucket_l = 1;
                    upp = 1;
                    n_bucket_l = 1;
                    nupp = 1;
                }

                //p442
                P_up = upp * node_1[time+1].node[i].price[bucket_l - 1] + ( 1 - upp ) * node_1[time+1].node[i].price[bucket_l];
                n_P_up = nupp * node_2[time+1].node[i].price[bucket_l - 1] + ( 1 - nupp ) * node_2[time+1].node[i].price[bucket_l];

                //down
                down_price = S * pow( u, (time - i)) * pow( d , (i + 1));
                avg_down = (avg * ( time + 1 ) + down_price ) / (time + 2);

                if( node_1[time+1].node[i + 1].diff != 0 )
                {
                    bucket_l = ceil( (avg_down - node_1[time+1].node[i + 1].Amin) / (node_1[time+1].node[i + 1].diff) );
                    n_bucket_l = ceil( (avg_down - node_2[time+1].node[i + 1].Amin) / (node_2[time+1].node[i + 1].diff) );

                    if( bucket_l <= 0)      bucket_l = 1;
                    if( bucket_l > k)       bucket_l = k;
                    if( n_bucket_l <= 0)    n_bucket_l = 1;
                    if( n_bucket_l > k)     n_bucket_l = k;

                    ddown = ( bucket_l * node_1[time+1].node[i + 1].diff + node_1[time+1].node[i + 1].Amin - avg_down ) / node_1[time+1].node[i+1].diff;
                    nddown = ( n_bucket_l * node_2[time+1].node[i + 1].diff + node_2[time+1].node[i + 1].Amin - avg_down ) / node_2[time+1].node[i+1].diff;
                }
                else
                {
                    bucket_l = 1;
                    ddown = 1;
                    n_bucket_l = 1;
                    nddown = 1;
                }

                P_down = ddown * node_1[time+1].node[i + 1].price[bucket_l - 1] + ( 1 - ddown ) * node_1[time+1].node[i + 1].price[bucket_l];
                n_P_down = nddown * node_2[time+1].node[i + 1].price[bucket_l - 1] + ( 1 - nddown ) * node_2[time+1].node[i + 1].price[bucket_l];
                
                P_total = (P_up * p + P_down * (1 - p)) / R;
                n_P_total = (n_P_up * p + n_P_down * (1 - p)) / R;
                
                node_1[time].node[i].price[l] = P_total; 
                node_2[time].node[i].price[l] = n_P_total;

                if(avg < H)     //** different **//
                    node_1[time].node[i].price[l] = 0;

            }
        }
    }
        
    double output_price = - node_1[0].node[0].price[0] +  node_2[0].node[0].price[0];
    double delta = ( -node_1[1].node[0].price[0] + node_1[1].node[1].price[0] + node_2[1].node[0].price[0] - node_2[1].node[1].price[0] ) / ( S * u - S * d ) ;
    printf("--------------------------\n");
    printf("| put price is %lf |\n", output_price);
    printf( "| delta is %lf     |\n" , delta);
    printf("--------------------------\n");

    free(node_1);
    free(node_2);

    return 0;
}
//100 105 95 1 0.05 0.3 160 160
// put price is 7.746 and delta is −0.5423