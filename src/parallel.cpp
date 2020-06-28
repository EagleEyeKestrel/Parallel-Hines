//
//  main.cpp
//  1
//
//  Created by ji luyang on 2020/3/25.
//  Copyright © 2020 ji luyang. All rights reserved.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include "omp.h"
#define THREAD_NUM 20
#define maxn 5728210
double *upper,*lower,*diagonal,*rhs;
int *parent;
double t1,t2,t3;
using namespace std;
vector<int> childs[maxn];//childs of node i
int n;
int depth=0;//record max depth

struct task{//combine nodes to task
    vector<int> nodes;
    vector<task*> childs;
    task(){
        nodes=vector<int>();
    }
};

vector<task*> level[maxn];  //tasks of level i

void init(char* argv[]){  //initialize arrays
    fstream infile;
    char* infileName=argv[1];
    //char* infileName="/Users/jiluyang/Desktop/Project/data/case12.txt";
    infile.open(infileName);
    infile>>n;
    upper=new double[n];
    lower=new double[n];
    diagonal=new double[n];
    rhs=new double[n];
    parent=new int[n];
    for(int i=0;i<n;i++){
        int index;
        infile>>index>>upper[i]>>lower[i]>>rhs[i]>>diagonal[i]>>parent[i];
        if(parent[i]!=-1){
            childs[parent[i]].push_back(i);
        }
    }
    infile.close();
}

task* taskroot;

task* build(int root){  //build task tree
    task* newtask=new task();
    newtask->nodes.push_back(root);
    while(childs[root].size()==1){
        newtask->nodes.push_back(childs[root][0]);
        root=childs[root][0];
    }
    if(childs[root].size()>1){
        for(int i=0;i<childs[root].size();i++){
            newtask->childs.push_back(build(childs[root][i]));
        }
    }
    return newtask;
}

void dfs(task* tmp,int h){  //bfs task tree to build level[i]
    level[h].push_back(tmp);
    depth=max(h,depth);
    for(int i=0;i<tmp->childs.size();i++){
        dfs(tmp->childs[i],h+1);
    }
}

void backSweep(){  //hines backsweep
    t1=omp_get_wtime();
    for(int i=depth;i>=0;i--){
#pragma omp parallel for num_threads(THREAD_NUM) schedule(dynamic)
            for(int j=0;j<level[i].size();j++){
                task* tmptask=level[i][j];
                for(int k=tmptask->nodes.size()-1;k>=0;k--){
                    int now=tmptask->nodes[k];
                    for(int kk=0;kk<childs[now].size();kk++){
                        int child=childs[now][kk];
                        double factor=upper[child]/diagonal[child];
                        diagonal[now]-=factor*lower[child];
                        rhs[now]-=factor*rhs[child];
                    }
                }
            }
    }
    rhs[0]/=diagonal[0];
    t2=omp_get_wtime();
    cout<<"backward sweep time: "<<t2-t1<<endl;
}

void forwardSweep(){// hines forward sweep
    for(int i=0;i<=depth;i++){
#pragma omp parallel for num_threads(THREAD_NUM) schedule(dynamic)
            for(int j=0;j<level[i].size();j++){
                task* tmptask=level[i][j];
                for(int k=0;k<tmptask->nodes.size();k++){
                    int now=tmptask->nodes[k];
                    if(now){
                        rhs[now]-=lower[now]*rhs[parent[now]];
                        rhs[now]/=diagonal[now];
                    }
                }
            }
    }
    t3=omp_get_wtime();
    cout<<"forward sweep time: "<<t3-t2<<endl;
    cout<<"total sweep time: "<<t3-t1<<endl;
}

void output(char* argv[]){  // output to file
    ofstream outfile;
    char* outfileName=argv[2];
    //char* outfileName="/Users/jiluyang/Desktop/res12.txt";
    outfile.open(outfileName);
    for(int i=0;i<n;i++){
        outfile<<i<<" "<<upper[i]<<" "<<lower[i]<<" "<<rhs[i]<<" "<<diagonal[i]<<endl;
    }
    outfile.close();
}

int main(int argc,char* argv[]){
    init(argv);
    taskroot=build(0);
    dfs(taskroot,0);
    backSweep();
    forwardSweep();
    output(argv);
    return 0;
}
