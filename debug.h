#ifndef DEBUG_H
#define DEBUG_H

#ifndef debug
#define debug  false
#endif
//#define debug false
#if debug
#define D(A)  Serial.print((A))
#define Dln(A) Serial.println(A)
#define D2(A,B) D(A);Serial.println((B))
#define D3(A,B,C) D(A);D(B);Dln(C)
#define D4(A,B,C,X) D(A);D(B);D(C);Dln(X)
#else
#define D(A)
#define Dln(A)
#define D2(A,B)
#define D3(A,B,C)
#define D4(A,B,C,X)
#endif

#endif
