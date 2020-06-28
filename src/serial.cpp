//
//  main.cpp
//  serial
//
//  Created by ji luyang on 2020/6/25.
//  Copyright © 2020 ji luyang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <ctime>
#include "omp.h"
using namespace std;
#define maxn 5728210
double upper[maxn],lower[maxn],diagonal[maxn],rhs[maxn];
int parent[maxn];
int n;
int main(int argc, char * argv[]) {
    fstream infile;
    ofstream outfile;
    double startTime,backWardEndTime,forwardEndTime;
    char *infileName=argv[1],*outfileName=argv[2];
    infile.open(infileName);
    infile>>n;
    int i=0;
    for(;i<n;i++){
        int index;
        infile>>index>>upper[i]>>lower[i]>>rhs[i]>>diagonal[i]>>parent[i];
    }
    infile.close();
    startTime=omp_get_wtime();
    double factor;
    for(i=n-1;i>=1;i--){
        factor=upper[i]/diagonal[i];
        diagonal[parent[i]]-=factor*lower[i];
        rhs[parent[i]]-=factor*rhs[i];
    }
    backWardEndTime=omp_get_wtime();
    cout<<"Backward Time: "<<backWardEndTime-startTime<<endl;
    rhs[0]/=diagonal[0];
    for(i=1;i<n;i++){
        rhs[i]-=lower[i]*rhs[parent[i]];
        rhs[i]/=diagonal[i];
    }
    forwardEndTime=omp_get_wtime();
    cout<<"Forward time: "<<forwardEndTime-backWardEndTime<<endl;
    cout<<"Total time: "<<forwardEndTime-startTime<<endl;
    outfile.open(outfileName);
    for(i=0;i<n;i++){
        outfile<<i<<" "<<upper[i]<<" "<<lower[i]<<" "<<rhs[i]<<" "<<diagonal[i]<<endl;
    }
    outfile.close();
    return 0;
}
