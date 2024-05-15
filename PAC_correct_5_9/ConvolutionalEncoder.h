#ifndef CONVOLTIONALENCODER_H
#define CONVOLTIONALENCODER_H

using namespace std;

class ConvolutionalEncoder{
    public :
        ConvolutionalEncoder(int,int);  
        void output(vector<int>&,vector<int>&,vector<int>&);
    private :
        vector<int> impulse;
        int N;
        int K;
        int constraint_length;
        int sum;
};
ConvolutionalEncoder::ConvolutionalEncoder(int codelength,int messagelength){
    N = codelength;
    K = messagelength;
}
void ConvolutionalEncoder::output(vector<int>& message,vector<int>& convolution,vector<int>& codeword){
    impulse.resize(convolution.size());
    impulse = convolution;

    constraint_length = impulse.size();

    // for(int i=0;i<impulse.size();i++){
    //     cout << "impulse[" << i << "]=" << impulse.at(i) << endl;
    // }

    // for(int i=0;i<reg.size();i++)
    //     cout << "reg[" << i << "]=" << reg.at(i) << endl;

    for(int i=0;i<N;i++){
        sum = 0;
        // cout << "codeword[" << i << "] = " ;
        for(int j=0;j<constraint_length;j++){
            if(i-j>=0){
                sum = sum + impulse.at(j) * message.at(i-j);
            }
            else 
                break;
        }
        codeword.at(i) = sum % 2;
        // cout << codeword.at(i) << endl;
    }
}

#endif