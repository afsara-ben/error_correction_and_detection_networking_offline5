#include<iostream>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<math.h>
#include<vector>
#include <random>
#include <sstream>
#include <bitset>

#define DIV_VAL 100000
#define MAX_POLYNOMIAL_SIZE 20

using namespace std;

random_device rand_dev;
default_random_engine generator(rand_dev());
uniform_int_distribution<int> distribution(0, DIV_VAL);

string data;
int m; //number of bytes in a row of the data block
float p; //probability of each bit being toggled
int generator_polynomial;
int num_row;

string actual_checksum; //used in task 5;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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

    num_row = output_str.length() / (m*8);
    cout<< num_row <<endl;
    return output_str;

}

void convertToBinary(unsigned int n)
{
	if (n / 2 != 0) {
		convertToBinary(n / 2);
	}
	printf("%d", n % 2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						Hamming Code(correction for one bit error)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//helper function
int power(int x, unsigned int y) 
{ 
    if (y == 0) 
        return 1; 
    else if (y%2 == 0) 
        return power(x, y/2)*power(x, y/2); 
    else
        return x*power(x, y/2)*power(x, y/2); 
} 

//main method
void task3(string str)
{
	
    int str_len;
  	str_len = str.length();

    int extra_bits = log2(m * 8) + 1; //finding max possible power of 2 possible for given bits

  	cout << "No of data bits to be added " << extra_bits << " : Total Bits :" << str_len + extra_bits << endl;

    int row_len = m*8;
  	int num_row = str.length() / row_len;
	cout << " row_len : " << row_len<<endl;

	cout << endl << "Data bits entered : " << str << "\n";

	int toThePower = 0;

	int str_index = 0;


	while(num_row--)
	{
		toThePower = 0;

		for (int i = 0 ; i < (m * 8 + extra_bits) ; ++i)
		{
			if( (i + 1) == power(2, toThePower))
			{
				toThePower++;
				str.insert(str_index,"u");
			}
			str_index++;
		}
	}

	for (int i = 0; i < str.length(); ++i)
	{
		if((i % (m * 8 + extra_bits)) == 0) printf("\n");
		cout << str[i];
	}

	cout <<"\n";


	bool *color_bits = new bool[str.length() + 10];
	for (int i = 0; i < str.length() + 10; ++i)
	{
		color_bits[i] = false;
	}

    for(int offset=0; offset<str.length(); offset+=row_len + extra_bits){
        //cout <<"offset = " << offset << endl;
        int base = 1; // base is the check bits indexes, it will increase as 1,2,4,8,.... so on
        while(base <= row_len + extra_bits ){
            int XOR  = 0;
            int counter = 0;
            int index = base - 1; // actual index, as the index is 0 based
            while(index < row_len + extra_bits ){
                if(str[offset+index] == 'u') {
                    counter++;
                    index++;
                    continue;
                }
                XOR = XOR ^ (str[offset+index] - '0');
                counter++;
                index++;
                if(counter == base){
                    // we have checked base number of bits, and now going to skip base number of bits
                    index += base;
                }
            }
            //cout<<" index : " <<index <<endl;
            // here the XOR will represent the check bit at base-th position
            //cout << "check bits corresponding to " << base << "\033[1;32m is " << XOR << "\033[0m\n";
            str[offset+base-1] = XOR+'0'; 
            color_bits[offset+base - 1] = true;
            base *= 2;
        }

    }

    cout<<"data block after adding check bits " <<endl;
    for (int i = 0; i < str.length(); ++i)
	{
		if((i % (m * 8 + extra_bits)) == 0) printf("\n");
		if(color_bits[i]) cout << "\033[1;32m" << str[i] << "\033[0m";
		else cout << str[i];
	}

	cout <<"\n";


}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//							Serialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void task4(string str, int extra) //str is binary of original msg; extra is extra bits from task3
{
	char temp_arr[num_row][m*8 + extra];

	int itr = 0;
	for (int j = 0; j < num_row; ++j)
	{
		for (int i = 0; i < m*8 + extra; ++i)
		{
			temp_arr[j][i] = str[itr];
			itr++;
		}
		
	}

	for (int j = 0; j < num_row; ++j)
	{
		for (int i = 0; i < m*8 + extra; ++i)
		{
			cout << temp_arr[j][i];
		}
		cout<<endl;
		
	}
  
  	// accessing element column wise 
  	for (int i = 0; i < m*8 + extra; i++) { 
    	for (int j = 0; j < num_row; j++) 
    	{ 
      		cout << temp_arr[j][i]; 
    	} 
  	} 
  	cout<<endl;
 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 5 : CRC checksum calculation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//helper functions
//converts the int generator polynomial to an int vector
vector <int> integerToArray()
{
	vector <int> resultArray;
	while (true)
	{
		resultArray.insert(resultArray.begin(), generator_polynomial%10);
		generator_polynomial /= 10;
		if(generator_polynomial == 0)
			return resultArray;
	}
}

//method to create and concat checksum at end of string
string checksum(string msg, int msg_Size, int divisor[], int div_Size)
{
	//test with msg = 11010110110000 divisor = 10011

	string original_msg = msg;
	int n = 0,m = 0,flag = 0,k = 0,count = 0;
	string result, rem;
	int rem_Size;

	for(int i = msg_Size; i < (msg_Size + div_Size)-1; i++)
		{ msg += '0';}

	cout<<"Message string : "<<msg<<endl;

	msg_Size = (msg_Size + div_Size) - 1; //new size after appending (divisor_size - 1) zeros at end

	cout<<"\nNew message bit is " << msg << "\n";

	rem_Size = msg_Size;

	result = msg; //copy message string to result string

	cout<<"\nResult string : "<<result<<endl;
	
	for(int i=0; i<div_Size; i++)
	{
		result[i] = (msg[i]^divisor[i]);
		//cout<<msg[i]<< "^" <<divisor[i]<< " : " << result[i] << "\n";
		n = i; // total bit length after xor ing; n = 7, 12, 14 in example
	}

	while(n <= msg_Size)
	{
		while(flag == 0)
		{
			if(result[m] == '0')
				m++; //m iterates over bit positions from left until a 1 is found
			else
				flag = 1; //first non-zero bit from left side pele flag is set, so that while loop breaks
        }//while()
        
        flag = 0;

        //cout<< "First non-zero bit in " << m << "th postion; count is " << count << endl;
        for(int i = m; i < (div_Size + m); i++)
        {
        	if(result[i] == '0' || result[i] == '1')
        		count++; //keep on moving to the next existent bit
        }
        
        if(count == div_Size)
        {
        	for(int i=m; i<(div_Size+m); i++)
        	{
        		//cout<<result[i]<< "^" <<divisor[k]<< " : ";
        		result[i] = result[i]^divisor[k];
        		//cout<< result[i] <<"\n";
        		k++;
        	}
        	k = 0;
        }
        else
        	break;

        count = 0;
    
       // cout << "result : " << result << "\n"; //this print shows the gradual change
    
    }//while()


    rem_Size = msg_Size-1;

    for(int i = 1; i < div_Size; i++)
    {
    	msg[rem_Size] = result[rem_Size]; //copying from end 
    	rem_Size--;
    }

    cout<<"\nCRC code that is generated is " << msg << "\n";

    //putting value in remainder
    rem_Size = msg_Size-1;
    for(int i = (div_Size - 1); i > 0; i--)
    {
    	rem[i-1] = result[rem_Size];
    	actual_checksum [i-1] = rem [i-1];
		rem_Size--;
    }

    rem[div_Size-1] = '\0'; //otherwise a garbage was showing
    //actual_checksum = rem.c_str();
    cout << "actual_checksum is " <<actual_checksum.c_str() << endl;
    //cout<<"\nRemainder is " << rem.c_str() << "\n";
    cout<<"Data bits after appending CRC checksum <sent frame> : \n";
    cout<<original_msg<<"\033[1;36m"<< actual_checksum.c_str() <<"\033[0m\n";

    //return msg;
    return rem;
}

//calling method for task5
string task5(string msg)
{

	//configuring msg, size of msg, divisor and size of divisor
	int msg_Size = msg.length(); //msg is the binary string representation of original msg
	cout <<"\t\t " <<msg_Size <<endl;
	vector<int> temp_vec1 = integerToArray();

	int div_Size = temp_vec1.size();
	cout <<"div_Size : " <<div_Size <<endl;
	
	int divisor[div_Size]; //divisor[] array has the generator polynomial
	
	//transferring the divisor in vector to divisor in int array
	for (int i = 0; i < temp_vec1.size(); ++i)
	{
		divisor[i] =  temp_vec1.at(i);
	}

	//frame to be sent
	string recv_rem =  checksum(msg, msg_Size, divisor, div_Size);
	cout << "recv_rem " <<recv_rem.c_str() <<endl;
	string msg_with_checksum = msg + recv_rem.c_str();
	return msg_with_checksum;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 6 : generating error
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


double randomGenerator()
{
	int id = distribution(generator);
	int num = 0;
	for (int i = 0; i < id; ++i)
	{
		num = distribution(generator);
	}

	return (1.0 * num/DIV_VAL);
}
string task6(string CRC_frame)
{
	string received_frame;
	bool *error_position = new bool[CRC_frame.length() + 10];
	received_frame = CRC_frame;
	for (int i = 0; i < CRC_frame.length(); ++i)
	{
		if(randomGenerator() < p)
		{
			received_frame[i] = (char) ((received_frame[i] - '0') ^ 1) + '0'; //toggle
			error_position[i] = true; 
		}
		else
		{

			error_position[i] = false;
		}

	}

	//cout << "Error frame : " << received_frame << endl;

	//print
	cout<< "received frame : \n";
	for (int i = 0; i < CRC_frame.length(); ++i)
	{
		if(error_position[i])
		{
			cout<<"\033[1;31m"<< received_frame[i] <<"\033[0m";
		}
		else
		{
			cout << received_frame[i];
		}
	}

	cout << "\n\n";
	return received_frame;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 7 : verify correctness
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void task7(string msg)
{
	cout<< "in task 7 " <<endl;

	//configuring msg, size of msg, divisor and size of divisor
	int msg_Size = msg.length(); //msg is the binary string representation of original msg
	cout <<msg_Size <<endl;
	vector<int> temp_vec = integerToArray();

	int div_Size = temp_vec.size();
	cout <<"div_Size : " <<div_Size <<endl;
	
	int divisor[div_Size]; //divisor[] array has the generator polynomial
	
	//transferring the divisor in vector to divisor in int array
	for (int i = 0; i < temp_vec.size(); ++i)
	{
		divisor[i] =  temp_vec.at(i);
	}

	msg = msg.substr(0, msg_Size - div_Size + 1);
	cout << msg_Size - div_Size + 1 << endl;
	int msg_Size_without_checksum = msg.length(); //msg is the binary string representation of original msg
	cout <<msg_Size_without_checksum <<endl;

	string recvd_frame_checksum = checksum(msg, msg_Size_without_checksum, divisor ,div_Size);
	cout << "recvd_frame_checksum : " << recvd_frame_checksum.c_str() << endl;



}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 8 : remove crc checksum bits; deserialize
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 9 : correction of error
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 10 : binary to ASCII 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string BinaryStringToText(string binaryString) {
    string text = "";
    stringstream sstream(binaryString);
    while (sstream.good())
    {
        bitset<8> bits;
        sstream >> bits;
        text += char(bits.to_ulong());
    }
    cout << "output frame : " << text << "\n";
    return text;
}

int main()
{
	
	data = "Hamming Code";
	m = 2;
	p = 0.05;
	generator_polynomial = 10011; //generator_polynomial is of int type, in task 5,7 it is converted to int array to get the bits separately

	
	printf("Enter data string : %s\n", data.c_str());
	//cin >> data;
	printf("enter number of data bytes in a row (m) : %d\n", m);
	//cin >> m;
	printf("enter probability (p) : %.2lf\n",p);
	//cin >> p;


	task1();
	printf("data block <ascii code of m characters per row>\n");
	string strForTask3 = task2();

	printf("string from task 2 is : %s\n",strForTask3.c_str());
	task3(strForTask3);
	//task4("010010000110000101101101011011010110100101101110011001110010000001000011011011110110010001100101",0);
	//task4(strForTask3);
	//task5(strForTask3);
	//string crc_frame = task5(strForTask3);
	//string recv_frame = task6(crc_frame);

	//BinaryStringToText("010010000110000101101101011011010110100101101110011001110010000001000011011011110110010001100101");
	//task7(recv_frame);
	return 0;
}