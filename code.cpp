#include<iostream>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<math.h>


using namespace std;


string data;
int m; //number of bytes in a row of the data block
float p; //probability of each bit being toggled
int generator_polynomial;

void task1()
{
	//task 1
	string output_str;
	int len = strlen(data.c_str());

	output_str = data;
	while(output_str.length() % m != 0)
	{
		output_str+= "~";
	}

	printf("data string after padding : %s\n\n",output_str.c_str());
	data = output_str;
}

//did not use bitset as bitset<const int> takes constant values
string task2()
{

	string bin ="";
	string output_str;
    for (int i = 0; i < data.length(); i++)
    {
    	int val = (int)data[i];
    	while (val > 0) 
        { 
            (val % 2 == 1)? bin.push_back('1') : bin.push_back('0'); 
            val /= 2; 
        } 

        while(bin.length() != 8) bin.push_back('0'); //for adding the invisible 0
        
        reverse(bin.begin(), bin.end()); 
        
        output_str+=bin;
        bin = "";
  	}

	     	for (int i = 0; i < output_str.length(); ++i)
	     	{
	     		if((i % (m*8)) == 0) printf("\n");
	     			printf("%c",output_str[i]);
	     	}
	     	printf("\n");
	return output_str;

}




int main()
{
	
	data = "Computer Networks";
	m = 4;
	p = 0.05;
	
	printf("Enter data string : %s\n", data.c_str());
	//cin >> data;
	printf("enter number of data bytes in a row (m) : %d\n", m);
	//cin >> m;
	printf("enter probability (p) : %.2lf\n",p);
	//cin >> p;


	task1();
	printf("data block <ascii code of m characters per row>\n");
	task2();

	return 0;
}