#ifndef RATEPROFILE
#define RATEPROFILE
using namespace std;

class RateProfile{
    public :
        RateProfile(int,int);
        void generate_PWsequence(vector<int>&);
        void print_PWsequence();
        void generate_informationset(vector<int>&,vector<int>&);
        void print_informationset();
        void output(vector<int>&,vector<int>&);
    private :
        void D2B(int);
    private :
        int N;
        int K;
        int binarysize;     //Binary vector 的最大長度
        int weight;         //Binary weight
        vector<int> binary;         //用來存 binary form (其實可以不用)
        vector<int> sort_array;     //用來存 binary weight 的排序
        vector<int> sort_index;     //儲存 sort_array 的 index
        vector<int> information_set;    //用來儲存放 infromation bit 的 index (排序小到大)
        vector<int> frozen_set;         //用來儲存放 frozen bit 的 index (排序小到大)
};

RateProfile::RateProfile(int codelength,int messagelength){
    N = codelength;
    K = messagelength;

    binarysize = log2(N);
    binary.resize(binarysize);
    sort_array.resize(N);
    sort_index.resize(N);
    //執行decimal to binary，將binary weight存入sort_array
    for(int i=0;i<N;i++){
        D2B(i);
        for(int j=binarysize-1;j>=0;j--){
        }
        sort_array.at(i) = weight;
        sort_index.at(i) = i;
    }
    //排序
    for(int i=0;i<N-1;i++){
        for(int j=0;j<N-1-i;j++){
            // cout << "i = " << i << ",  j = " << j << endl;
            if(sort_array.at(j)>sort_array.at(j+1)){
                swap(sort_array.at(j),sort_array.at(j+1));
                swap(sort_index.at(j),sort_index.at(j+1));
            }
        }
    }


    //Deal with infromation set and frozen set
    information_set.resize(K);

    for(int i=0;i<K;i++){   //weight大的放infroamtion bit，故取後面的部分
        information_set.at(i) = sort_index.at(N-K+i);
    }

    sort(information_set.begin(),information_set.end());    //increasing set

    int counter = 0;        //差空格，非infroamtion set 的就是frozen set
    for(int i=0;i<N;i++){
        if(information_set.at(counter)==i)
            counter++;
        else
            frozen_set.push_back(i);
    }
}
void RateProfile::D2B(int num){
    weight = 0;
    for(int i=binarysize-1;i>=0;i--){
        int k = num >> i;

        if(k & 1){
            binary.at(i) = 1;
            weight = weight + 1;
        }
        else
            binary.at(i) = 0;
    }
}
void RateProfile::generate_PWsequence(vector<int>& sequence){
    sequence = sort_index;
}
void RateProfile::print_PWsequence(){
    cout << "print PWsequence = " ;
    for(int i=0;i<N;i++){
        cout << sort_index.at(i) << " ";
    }
    cout << endl;
    cout << endl;
}
void RateProfile::generate_informationset(vector<int>& a,vector<int>& b){
    a = information_set;
    b = frozen_set;
}
void RateProfile::print_informationset(){
    cout << "information set = ";
    for(int i=0;i<K;i++)
        cout << information_set.at(i) << " ";
    cout << endl;
    cout << endl;
    cout << "frozen set = ";
    for(int i=0;i<N-K;i++)
        cout << frozen_set.at(i) << " ";
    cout << endl;
    cout << endl;
}
void RateProfile::output(vector<int>& message, vector<int>& codeword){
    int counter = 0;
    for(int i=0;i<N;i++){
        if(information_set.at(counter)==i && counter < K){
            codeword.at(i) = message.at(counter);
            counter ++;
        }
        else
            codeword.at(i) = 0;
    }
}

#endif

