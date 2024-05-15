#ifndef AWGNNOISE_H
#define AWGNNOISE_H


#define RANDMAX 32767



class AWGN {
    public:
        AWGN(int codeword_length,int message_length,double noise_SNR){
            // std::cout << "call AWGNNoise constructor !!" <<std::endl;
            N = codeword_length;
            K = message_length;
            SNR = noise_SNR;
            CodeRate = (double)K/(double)N;
            var = 1.0 / (2.0 * CodeRate * pow(10.0,SNR / 10.0));
            // std::cout << "var=" << var << std::endl;
            sigma = sqrt(var);
        };
        void output(std::vector<int>& , std::vector<double>&);
        void print_parameter();
        void print_LLR();
    private:
        int N;
        int K;
        double SNR;
        double CodeRate;
        double var;
        double sigma;
        std::vector<double> modulation;
        float rand49();
        double Normal();
        std::vector<double> llr;    //LLR use for print
};

//public
void AWGN::output(std::vector<int>& codeword, std::vector<double>& LLR){
    // std::cout << "call AddNoise member function !!" <<std::endl;
    LLR.resize(N);
    llr.resize(N);
    modulation.resize(N);
    for(int i=0;i<codeword.size();i++){
        double noise = sigma*Normal();
        modulation.at(i) = 1 - 2*codeword.at(i) + noise;       //BPSK + noise
        // modulation.at(i) = 1 - 2*codeword.at(i);
        // std::cout << "LLR[" << i << "] = " << LLR.at(i) << std::endl;
        LLR.at(i) = (2*modulation.at(i))/var;
        // std::cout << "modu[" << i << "]=" << modulation.at(i) << "  noise=" << noise << "  LLR[" << i << "]=" << LLR.at(i) << std::endl;
    }
    llr = LLR;
    // std::cout << std::endl;
}
void AWGN::print_parameter(){
    // std::cout << "call print_parameter member function !!" <<std::endl;
    std::cout << "N = " << N << std::endl;
    std::cout << "K = " << K << std::endl;
    std::cout << "SNR = " << SNR << std::endl;
    std::cout << "CodeRate = " << CodeRate << std::endl;
    std::cout << "var = " << var << std::endl;
    std::cout << "sigma = " << sigma << std::endl;
    std::cout << std::endl;
}
void AWGN::print_LLR(){
    // std::cout << "call print_LLR function" << std::endl;
    for(int i=0;i<N;i++)
        std::cout << "LLR[" << i << "] = " << llr.at(i) << std::endl; 
    // std::cout << std::endl;
}

//private
float AWGN::rand49()  // AWGN noise settingfloat rand49(void)  // uniformly distributed in [0,1) /*RANDMAX=7FFF (32767)*/
{
	static int Num=0;
    double number;
    int i;
    i=rand()%(RANDMAX+1);
    number=(double)i/((unsigned)(RANDMAX+1));
    Num++;
    if (Num >=RANDMAX){
		time_t t;
		t=time(NULL);
		srand((unsigned)(t%RAND_MAX));
        Num=0;
    }
    return (float)number;
}
double AWGN::Normal(void) // generate standard normal dist. N(0,1) by Polar form of Box-Muller method
{
	static int iset=0;
    static double qset;
    double vx,vy,r,temp;
    if (iset==0){
	    do
        {
		    vx=2.0*rand49()-1.0; // uniformly distributed in [-1,1]
            vy=2.0*rand49()-1.0; // uniformly distributed in [-1,1]
            r=vx*vx+vy*vy;
        } while (r>=1.0 || r==0);
        temp=sqrt(-2.0*log(r)/r);
        qset=vy*temp;
        iset=1;
        return (vx*temp);
    }
    else{
   	    iset=0;
        return qset;
    }
}



#endif