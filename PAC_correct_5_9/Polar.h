#ifndef POLAR_H
#define POLAR_H
using namespace std;

class Polar{
    public :
        Polar(int codelength,int messagelength){
            N = codelength;
            K = messagelength;
            stage = log2(N);
        };
        void output(vector<int>&,vector<int>&);
    private :
        int N;
        int K;
        int stage; //stage of polar tree
        int space;
        int cal_num;
        int segment;
        vector<int> codeword;
};
void Polar::output(vector<int> &in,vector<int> &out){
    codeword.resize(N);
    codeword = in;
    for(int i=0;i<stage;i++){
        space   = pow(2,i);
        cal_num = space*2;
        segment = N/cal_num;
        // std::cout << "   space =" << space << std::endl;
        // std::cout << "   cal_num = " << cal_num << std::endl;
        // std::cout << "   segment = " << segment << std::endl;
        for(int j=0;j<segment;j++){
            // std::cout << "第" << j << "塊計算"  << std::endl;
            for(int k=0;k<cal_num/2;k++){
                codeword[k+j*cal_num] = (codeword[k+j*cal_num]+codeword[k+j*cal_num+space])%2;
                // std::cout << "codeword[" << k+j*cal_num << "] = " << "codeword[" << k+j*cal_num << "] + codeword[" << k+j*cal_num+space << "]" << std::endl;
            }
            for(int k=cal_num/2;k<cal_num;k++){
                codeword[k+j*cal_num] = codeword[k+j*cal_num];
                // std::cout << "codeword[" << k+j*cal_num << "] = " << "codeword[" << k+j*cal_num << "]" << std::endl;
            }
        }
    }
    out = codeword;
}

#endif