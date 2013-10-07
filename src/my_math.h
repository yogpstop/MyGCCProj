#define gcd(a, b, y, t); {t x,z=(a)%(b);y=b;while(z){x=y;y=z;z=x%y;}}
#define lcm(a, b, c, t); {gcd(a,b,c,t);(c)=(a)*(b)/(c);}

