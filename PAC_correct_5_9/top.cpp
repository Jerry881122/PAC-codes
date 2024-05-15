#include<iostream>
#include<stdlib.h>
#include<ctime>   /* 時間相關函數 */
#include<vector>
#include<cmath>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include "RateProfile.h"
#include "ConvolutionalEncoder.h"
#include "Polar.h"
#include "AWGN.h"
#include "SCL.h"

using namespace std;

#define N 128
#define K N/2
#define L 8
#define ERROR_FRAME 1
#define SNR_BEGIN 1.5
#define SNR_MAX 1.5
#define SNR_GAP 0.25

int main(){
    srand(time(NULL));

    //encoder 參數
    vector<int> message(K);
    vector<int> informationset(K);
    vector<int> frozenset(N-K);
    vector<int> rate_out(N);
    vector<int> impulse({1,0,1,1});
    vector<int> con_out(N);
    vector<int> polar_out(N);
    vector<double> FER_result;
    vector<int> u(N);
    vector<double> u_llr(N);
    vector<double> PM(L);
    // vector<int> mismatch_frequency(N);
    double total_error;
    double frame;
    double error;
    double error_frame;
    double SNR;
    double match;
    double mismatch;
    bool flag;

    //channel 參數
    vector<double> llr_out(N);

    //decoder 參數
    vector<int> SCL_out(N);

    //object
    RateProfile rate(N,K);
    rate.generate_informationset(informationset,frozenset);

    ConvolutionalEncoder encoder1(N,K); 
    Polar encoder2(N,K);

    cout << "PAC codes(" << N << "," << K << ") and list size = " << L << endl;
     
    for(SNR=SNR_BEGIN;SNR<=SNR_MAX;SNR=SNR+SNR_GAP){
        total_error = 0;
        frame = 0;
        error_frame = 0;
        match = 0;
        mismatch = 0;
        flag = 0;
        // int u_error = 0;
        while(error_frame<ERROR_FRAME){
            frame = frame + 1;
            //randomly generate message
            for(int i=0;i<K;i++)
                message.at(i) = rand()%2;

            //connecting to rate profile
            // rate.print_informationset();
            rate.output(message,rate_out);
            // for(int i=0;i<N;i++)
            //     cout << "rate_out[" << i << "] = " << rate_out.at(i) << endl;
            // cout << endl;


            //connecting to convolutional encoder
            encoder1.output(rate_out,impulse,con_out);
            // for(int i=0;i<N;i++)
            //     cout << "con_out[" << i << "]=" << con_out.at(i) << endl;
            // cout << endl;


            //connecting to polar transformation
            encoder2.output(con_out,polar_out);
            // for(int i=0;i<N;i++)
            //     cout << "polar_out[" << i << "] = " << polar_out.at(i) << endl;
            // cout << endl;

            //connecting to AWGN
            AWGN channel(N,K,SNR);
            channel.output(polar_out,llr_out);
            // for(int i=0;i<N;i++)
            //     cout << "llr_out[" << i << "] = " << llr_out.at(i) << endl;
            // cout << endl;
            
            //connecting to SCL decoder
            SCL decoder(N,K,llr_out,frozenset,L,impulse);
            // decoder.print_ALL_LLR();
            // decoder.print_u_hat();
            // cout << "---------------------start process decoding--------------------" << endl;
            decoder.output(SCL_out);
            // for(int i=0;i<N;i++)
            //     cout << "SCL_out[" << i << "] = " << SCL_out.at(i) << endl;
            // cout << endl;
            // decoder.print_path_metric();
            decoder.return_PM(PM);
            for(int i=0;i<L;i++){
                cout << "PM[" << i << "] = " << PM.at(i) << endl;
            }
            cout << endl;
            decoder.return_llr(u_llr);

            error = 0;
            for(int i=0;i<N;i++){
                if(rate_out.at(i)!=SCL_out.at(i))
                    error = error + 1;
            }

            for(int i=0;i<N;i++){
                if((u.at(i)==0 && u_llr.at(i)>0)||(u.at(i)==1 && u_llr.at(i)<0))
                    flag = flag;
                else if((u.at(i)==0 && u_llr.at(i)<0)||(u.at(i)==1 && u_llr.at(i)>0)){
                    flag = 1;
                    // mismatch_frequency.at(i)++;
                } 
            }
            if(flag=1)
                mismatch++;
            else
                match++;
            

            // cout << "error = " << error << endl;
            if(error!=0)
                error_frame = error_frame + 1;
            total_error = total_error + error;
        }
        cout << "SNR = " << SNR << endl;
        cout << "   total frame = " << frame << endl;
        cout << "   total error bit=" << total_error << " error_frame=" << error_frame << endl;
        cout << "   BER=" << total_error/(frame*K) << " FER=" << error_frame/frame << endl;
        cout << "   match = " << match << "  mismatch = " << mismatch << endl;
        // cout << "   u_error = " << u_error << endl;
        FER_result.push_back(error_frame/frame);
        // for(int i=0;i<N;i++){
        //     cout << "mismatch_frequency[" << i << "]=" << mismatch_frequency.at(i) << endl;
        // }
    }
    
   return 0;
}

