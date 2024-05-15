#ifndef SCL_H
#define SCL_H

//(llr is negative) -> 1
//(llr is positive) -> 0

class SCL{
    public :
        SCL(int,int,std::vector<double>&,std::vector<int>&,int,std::vector<int>&);
        void output(std::vector<int>&);
        void print_ALL_LLR();
        void print_u_hat();
        void print_v_value();
        void print_path_metric();
        void print_node_state();
        void return_decoded_u(std::vector<int> &);
        void return_llr(std::vector<double> &);
        void return_PM(std::vector<double> &);
    private :
        double sign_function(double);
        double min(double,double);
        void f_function(std::vector<double>& ,std::vector<double>& ,int,int,int,int);
        void g_function(std::vector<double>& ,std::vector<double>& ,std::vector<int>& ,int,int,int,int);
        int hard_decision(double);
        double path_metric(double,double,int);
        void PM_ordering();     //re-order the path metric (from low to high)
        int frozen_process(int,int);
        int v_value_from_u(int,int,int);
    private :
        int N;
        int K;
        int stage;
        int L;                  //list size;
        int calc_list;          //目前有計算幾個list
        int index_node;
        int temp;
        int frozen_counter;
        std::vector<std::vector<std::vector<double>>> ary;  //calculated LLR while SCL decoding (3D-array)
        std::vector<std::vector<std::vector<int>>> u_hat;   //3D-array
        std::vector<std::vector<int>> v_value;  //解碼時的convolution register
        std::vector<int> node_state;            //所有list的node state都相同
        std::vector<int> frozenSet;             //store frozen set
        std::vector<double> PM;
        std::vector<int> impulse;
};

SCL::SCL(int codeword_length,int message_length,std::vector<double>& in,std::vector<int>& frozen,int list,std::vector<int>& con_impulse){   //setting size of every argument
    N = codeword_length;
    K = message_length;
    stage = log2(N);
    L = list;
    ary.resize(2*L);
    u_hat.resize(2*L);
    v_value.resize(2*L);

    //設定 impulse response of convolutional encoder 
    impulse.resize(con_impulse.size());
    impulse = con_impulse;

    for(int i=0;i<ary.size();i++){  //設定ary和u_hat的size 
        ary[i].resize(stage+1);
        u_hat[i].resize(stage+1);
        for(int j=0;j<ary[i].size();j++){
            ary[i][j].resize(N);
            u_hat[i][j].resize(N);
        }
    }
    for(int i=0;i<v_value.size();i++){  //設定 v 值 的size (List x N)
        v_value[i].resize(N);
    }
    frozenSet = frozen;
    ary[0][0] = in;
    node_state.resize(2*N-1);
    calc_list = 1;      //輸入一個資料，使用的list大小為 "1"
}
void SCL::output(std::vector<int>& decoded_codeword){
    decoded_codeword.resize(N);
    // std::cout << "call processing !" << std::endl;
    bool done = 0;
    int depth = 0;
    int node  = 0;
    frozen_counter = 0;
    while(done == 0){
        // std::cout << "---------------------------------do while-----------------------------------" << std::endl;
        if(depth == stage){     //get to leaf
            // std::cout << "depth = " << depth << " , node = " << node << std::endl;
            if(frozenSet.at(frozen_counter)==node && frozen_counter<=N-K-1){
                // std::cout << "it's a frozen bit !" << std::endl;
                for(int i=0;i<calc_list;i++){
                    PM.resize(calc_list);
                    // u_hat[i][depth][node] = 0;
                    u_hat[i][depth][node] = frozen_process(i,node);
                    // std::cout << "ary[" << i << "][" << depth << "][" << node << "] = " << ary[i][depth][node] << std::endl;
                    // std::cout << "u_hat[" << i << "][" << depth << "][" << node << "] = " << u_hat[i][depth][node] << std::endl;
                    PM.at(i) = path_metric(PM.at(i),ary[i][depth][node],u_hat[i][depth][node]);
                    // print_path_metric(calc_list);
                }
                if(frozen_counter<N-K-1)
                    frozen_counter = frozen_counter + 1;
            }
            else{   //information bit   $ duplicate
                // std::cout << "it's a information bit !" << std::endl;
                PM.resize(2*calc_list);     //因複製path，故大小乘2
                for(int i=0;i<calc_list;i++){
                    ary[i+calc_list] = ary[i];      //duplicate the ary and u_hat

                    u_hat[i+calc_list] = u_hat[i];  //複製step1.全複製
                    u_hat[i][depth][node] = 0;      //複製step2.分別給 0 或 1
                    u_hat[i+calc_list][depth][node] = 1;    //複製step2.分別給 0 或 1

                    v_value[i+calc_list] = v_value[i];
                    // std::cout << std::endl;
                    // std::cout << "印出duplicate的v" << std::endl;
                    // for(int k=0;k<N;k++)
                    //     std::cout << "v[" << i << "][" << k << "]=" << v_value[i][k] << " ";
                    // std::cout << std::endl;
                    // for(int k=0;k<N;k++)
                    //     std::cout << "v[" << i+calc_list << "][" << k << "]=" << v_value[i+calc_list][k] << " ";
                    // std::cout << std::endl;
                    // std::cout << std::endl;
                    v_value[i][node] = v_value_from_u(i,node,0);
                    v_value[i+calc_list][node] = v_value_from_u(i+calc_list,node,1);
                    // std::cout << std::endl;
                    // std::cout << "計算後的v" << std::endl;
                    // for(int k=0;k<N;k++)
                    //     std::cout << "v[" << i << "][" << k << "]=" << v_value[i][k] << " ";
                    // std::cout << std::endl;
                    // for(int k=0;k<N;k++)
                    //     std::cout << "v[" << i+calc_list << "][" << k << "]=" << v_value[i+calc_list][k] << " ";
                    // std::cout << std::endl;
                    // std::cout << std::endl;

                    // std::cout << "算PM[" << i+calc_list << "]" << std::endl;
                    PM.at(i+calc_list) = path_metric(PM.at(i),ary[i][depth][node],u_hat[i+calc_list][depth][node]);
                    // std::cout << "算PM[" << i << "]" << std::endl;
                    PM.at(i) = path_metric(PM.at(i),ary[i][depth][node],u_hat[i][depth][node]);
                    // std::cout << "PM[" << i << "] = " << PM.at(i) << std::endl;
                    // std::cout << "PM[" << i+calc_list << "] = " << PM.at(i+calc_list) << std::endl;
                }
                calc_list = calc_list * 2;
                // print_ALL_LLR();
                // print_u_hat();
                // print_v_value();
            }
            // std::cout << "原本的PM ==>" << std::endl;
            //     for(int i=0;i<PM.size();i++){
            //         std::cout << "PM[" << i << "] = " << PM.at(i) << std::endl;
            //     }
            // std::cout << "################ Before cutting !! ###############" << std::endl;
            // print_ALL_LLR();
            // print_u_hat();
            PM_ordering();
            if(calc_list > L){
                    calc_list = calc_list / 2;
                    // std::cout << "################  After cutting and ordering !! ###############" << std::endl; 
                    // print_ALL_LLR();
                    // print_u_hat();
                }
            // else{
                // std::cout << "################  After ordering !! ###############" << std::endl; 
                // print_ALL_LLR();
                // print_u_hat();
            // }
            if(node == N-1)
                done = 1;
            else{
                node = floor(node/2);
                depth = depth - 1;
            }
        }
        else{       //node_state == 0
            index_node = (int)(pow(2,depth)) - 1 + node;
            temp = (int)(pow(2,stage-depth));
            // std::cout << "depth = " << depth << " , node = " << node << " , temp = " << temp << std::endl;
            // std::cout << "node state = " << node_state.at(index_node) << std::endl;
            // std::cout << "目前list size = " << calc_list << std::endl;
            if(node_state.at(index_node)==0){
                // std::cout << "go left child" << std::endl;
                for(int i=0;i<calc_list;i++){
                    std::vector<double> reg(ary[i][depth].begin()+temp*node,ary[i][depth].begin()+temp*(node+1));
                    std::vector<double> a(reg.begin(),reg.begin()+temp/2);
                    std::vector<double> b(reg.begin()+temp/2,reg.end());
                    // for(int j=0;j<reg.size();j++)
                    //     std::cout << "reg[" << i << "]=" << reg.at(j) << std::endl;
                    // std::cout << "call f function !" << std::endl;
                    f_function(a,b,node * 2,depth + 1,temp / 2,i);
                }
                node = node * 2;
                depth = depth + 1;
                temp = temp / 2;
                node_state.at(index_node) = 1;
                // print_ALL_LLR();
                // print_u_hat();
                // print_v_value();
                // print_node_state();
            }
            else if(node_state.at(index_node)==1){
                // std::cout << "go right child !" << std::endl;
                for(int i=0;i<calc_list;i++){
                    std::vector<double> reg(ary[i][depth].begin()+temp*node,ary[i][depth].begin()+temp*(node+1));    //暫存belief
                    std::vector<double> a(reg.begin(),reg.begin()+temp/2);
                    std::vector<double> b(reg.begin()+temp/2,reg.end());
                    int left_node  = 2 * node;      //要把left child得出的u給right child用
                    int left_depth = depth + 1;
                    int left_temp  = temp / 2; 
                    // for(int j=0;j<reg.size();j++)
                    //     std::cout << "reg[" << i << "]=" << reg.at(j) << std::endl;
                    std::vector<int> u_hat_reg(u_hat[i][left_depth].begin()+left_temp*left_node,u_hat[i][left_depth].begin()+left_temp*(left_node+1));
                    // for(int j=0;j<u_hat_reg.size();j++)
                    //     std::cout << "u_hat_reg[" << i << "]=" << u_hat_reg.at(j) << std::endl;
                    // std::cout << "call g function !" << std::endl;
                    g_function(a,b,u_hat_reg,node * 2 + 1,depth + 1,temp / 2,i);
                }
                node = node * 2 + 1;
                depth = depth + 1;
                temp = temp / 2;
                node_state.at(index_node) = 2;
                // print_ALL_LLR();
                // print_u_hat();
                // print_v_value();
                // print_node_state();
            }
            else{   //node_state = 2
                // std::cout << "go parents !" << std::endl;
                for(int i=0;i<calc_list;i++){
                    int left_node = node * 2;
                    int right_node = node * 2 + 1;
                    int left_depth = depth + 1;
                    int right_depth = depth + 1;
                    int left_temp = temp / 2;
                    int right_temp = temp / 2;
                    std::vector<int> u_hat_left(u_hat[i][left_depth].begin()+left_temp*left_node,u_hat[i][left_depth].begin()+left_temp*(left_node+1));
                    std::vector<int> u_hat_right(u_hat[i][right_depth].begin()+right_temp*right_node,u_hat[i][right_depth].begin()+right_temp*(right_node+1));
                    // for(int j=0;j<u_hat_left.size();j++)
                    //     std::cout << "u_hat_left[" << j << "] = " << u_hat_left.at(j) << std::endl;
                    // for(int j=0;j<u_hat_right.size();j++)
                    //     std::cout << "u_hat_right[" << j << "] = " << u_hat_right.at(j) << std::endl;
                    for(int j=0;j<left_temp;j++){
                        u_hat[i][depth][temp*node+j] = (u_hat_left.at(j) + u_hat_right.at(j)) % 2;
                        u_hat[i][depth][temp*node+j+right_temp] = u_hat_right.at(j);
                    }
                }
                node = floor(node/2);
                depth = depth - 1;
                // print_ALL_LLR();
                // print_u_hat();
                // print_v_value();
                // print_node_state();
            }
        }
    }
    // std::cout << "decoding end !!" << std::endl;
    // print_ALL_LLR();
    // print_u_hat();
    // print_v_value();
    // decoded_codeword = u_hat[0][stage];
    decoded_codeword = v_value[0];
}
void SCL::print_ALL_LLR(){
    std::cout << std::endl;
    std::cout << "print All LLR !!" << std::endl;
    for(int k=0;k<calc_list;k++){
        std::cout << "----list[" << k << "]----" << std::endl;
        for(int i=0;i<ary[k].size();i++){
            for(int j=0;j<N;j++)
                std::cout << "ary[" << k << "][" << i << "][" << j << "]=" << std::setw(5) << std::setprecision(2) << ary[k][i][j] << " ";
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
void SCL::print_u_hat(){
    std::cout << std::endl;
    std::cout << "print u hat !!" << std::endl;
    for(int k=0;k<calc_list;k++){
        std::cout << "----list[" << k << "]----" << std::endl;
        for(int i=0;i<u_hat[k].size();i++){
            for(int j=0;j<N;j++){
                std::cout << "u_hat[" << k << "][" << i << "][" << j << "] = " << u_hat[k][i][j] << " " ;
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
void SCL::print_path_metric(){
    std::cout << "print path metric !" << std::endl;
    for(int i=0;i<calc_list;i++){
        std::cout << "PM[" << i << "] = " << PM.at(i) << std::endl;
    }
}
void SCL::print_node_state(){
    // std::cout << "print node state !!" << std::endl;
    int sum = 0;
    int initial = 0;
    int x;
    for(int i=0;i<=stage;i++){
        x = (int)(pow(2,i));
        sum = sum + x;
        for(int j=initial;j<sum;j++)
        {
            std::cout << node_state.at(j);
        }
        std::cout << std::endl;
        initial = initial + x;

    }
}
void SCL::print_v_value(){
    std::cout << "print v value !!" << std::endl;
    for(int i=0;i<calc_list;i++){
        std::cout << "----list[" << i << "]----" << std::endl;
        for(int j=0;j<N;j++)
            std::cout << "v[" << i << "][" << j << "]=" << v_value[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
void SCL::return_decoded_u(std::vector<int> &x){
    x = u_hat[0][stage];
}
void SCL::return_llr(std::vector<double> &x){
    x = ary[0][stage];
}
void SCL::return_PM(std::vector<double> &x){
    x = PM;
}
//private function
double SCL::sign_function(double x){
    if(x>=0)
        return 1;
    else
        return -1;
}
double SCL::min(double x, double y){
    if(x>y)
        return y;
    else
        return x;
}
void SCL::f_function(std::vector<double>& a, std::vector<double>& b,int node, int depth,int temp,int list_index){
    for(int i=0;i<temp;i++)
        ary[list_index][depth][temp*node+i] = sign_function(a.at(i)) * sign_function(b.at(i)) * min(fabs(a.at(i)),fabs(b.at(i)));
}
void SCL::g_function(std::vector<double>& a, std::vector<double>& b, std::vector<int>& u ,int node, int depth,int temp,int list_index){
    for(int i=0;i<temp;i++)
        ary[list_index][depth][temp*node+i] = pow(-1,u.at(i)) * a.at(i) + b.at(i);
    // return pow(-1,u)*a + b;
}
int SCL::hard_decision(double num){
    if(num<0)
        return 1;
    else
        return 0; 
}
double SCL::path_metric(double last_PM,double lambda,int u){
    // std::cout << "原來的metric = " << last_PM << " , lambda = " << lambda << " , u = " << u << std::endl;
    if(u==(1-sign_function(lambda))/2){
        // std::cout << "進上面" << std::endl;
        return last_PM;
    }
    else{
        // std::cout << "進下面" << std::endl;
        return last_PM + std::abs(lambda);
    }
}
void SCL::PM_ordering(){
    // std::cout << "call PM ordering !!" << std::endl;
    // std::cout << "calc_list = " << calc_list << std::endl;
    std::vector<double> PM_2(PM);
    // std::cout << "PM size = " << PM.size() << std::endl;
    std::vector<int> indices(PM.size());
    std::vector<double> PM_reg(PM);
    std::vector<std::vector<std::vector<double>>> ary_reg(ary);
    std::vector<std::vector<std::vector<int>>> u_hat_reg(u_hat);
    std::vector<std::vector<int>> v_value_reg(v_value);
    for (int i = 0; i < indices.size(); i++)
        indices[i] = i;
    std::sort(indices.begin(), indices.end(), [&PM_2](int a, int b) {
        return PM_2[a] < PM_2[b];
    });
    // std::cout << "排序後的索引：";
    // for (int i = 0; i < indices.size(); ++i) {
    //     std::cout << indices[i] << " ";
    // }
    // std::cout << std::endl;
    if(calc_list<L){
        for(int i=0;i<calc_list;i++){
            PM.at(i) = PM_reg.at(indices[i]);
            ary.at(i) = ary_reg.at(indices[i]);
            u_hat.at(i) = u_hat_reg.at(indices[i]);
            v_value.at(i) = v_value_reg.at(indices[i]);
        }
    }
    else{
        for(int i=0;i<L;i++){
            PM.at(i) = PM_reg.at(indices[i]);
            ary.at(i) = ary_reg.at(indices[i]);
            u_hat.at(i) = u_hat_reg.at(indices[i]);
            v_value.at(i) = v_value_reg.at(indices[i]);
        }
    }
}
int SCL::frozen_process(int list_index, int node){
    v_value[list_index][node] = 0;  //因為是frozen bit，故直接給 v 值
    // std::cout << std::endl;

    // print_v_value();
    // std::cout << std::endl;

    std::vector<int> con_reg(impulse.size());     //convolutional 的 register 
    for(int j=0;j<con_reg.size();j++){
        if(node-j>=0)
            con_reg.at(j) = v_value[list_index][node-j];
        else
            con_reg.at(j) = 0;
    }
    // std::cout << "convolutional register for List=" << list_index << std::endl;
    // for(int j=0;j<con_reg.size();j++){
    //     std::cout << "con_reg[" << j << "] = " << con_reg.at(j) << " ";
    // }
    // std::cout << std::endl; 
    // std::cout << std::endl;
    int sum = 0;
    for(int i=0;i<impulse.size();i++){
        if(impulse.at(i)==1){
            sum = sum + con_reg.at(i);
        }
    }
    return sum%2;
}

int SCL::v_value_from_u(int L, int node,int u){
    int sum=0;
    for(int i=1;i<impulse.size();i++){
        if(impulse.at(i)==1){
            if(node-i<0)
                sum = sum;
            else{
                // std::cout << "v_value[" << L << "][" << node-i << "] = " << v_value[L][node-i] << std::endl; 
                sum = sum + v_value[L][node-i];
            }
        }
    }
    // std::cout << "不管u的結果 = " << sum << std::endl;
    sum = sum + u;
    // std::cout << "sum = " << sum << std::endl;
    return sum % 2;
}



#endif