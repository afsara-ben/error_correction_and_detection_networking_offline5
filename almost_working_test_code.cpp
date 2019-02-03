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

int checksum_Size; //needed for task 8
int extra_bit_count; //passed to task 4
bool *check_bit_positions = new bool [1000]; //for task9


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
    //cout<< num_row <<endl;
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


string task3(string str)
{
	
    int str_len;
  	str_len = str.length();

    int extra_bits = log2(m * 8) + 1; //finding max possible power of 2 possible for given bits
    extra_bit_count = extra_bits;

  	//cout << "No of data bits to be added " << extra_bits << " : Total Bits :" << str_len + extra_bits << endl;

    int row_len = m*8;
  	int num_row = str.length() / row_len;
	//cout << " row_len : " << row_len<<endl;

	//cout << endl << "Data bits entered : " << str << "\n";

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

            check_bit_positions[offset+base - 1] = true;

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

	return str;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//							task 4 : Serialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool *error_position_col_major;
bool *error_position; //initialized in task 6
string task4(string str, int extra) //str is binary of original msg; extra is extra bits from task3
{
	char temp_arr[num_row][m*8 + extra];
	
	error_position_col_major = new bool[str.length() + 10];
	
	
	int itr = 0;

	for (int j = 0; j < num_row; ++j)
	{
		for (int i = 0; i < m*8 + extra; ++i)
		{
			//if(error_position[itr]) error_position_col_major[ i+j*(m*8+extra) ] = true;
			//else error_position_col_major[ i+j*(m*8+extra) ] = false;

			temp_arr[j][i] = str[itr];
			itr++;
		}
		
	}

	//print original matrix
	/*for (int j = 0; j < num_row; ++j)
	{
		for (int i = 0; i < m*8 + extra; ++i)
		{
			cout << temp_arr[j][i];
		}
		cout<<endl;
		
	}*/
  
  	// accessing element column wise 
  	string column_str;
  	//cout<< "\ndata bits after column wise Serialization : \n";
  	for (int i = 0; i < m*8 + extra; i++) { 
    	for (int j = 0; j < num_row; j++) 
    	{ 
    		column_str += temp_arr[j][i];
      		cout << temp_arr[j][i]; 
    	} 
  	} 
  	cout<<endl;

	/*for (int i = 0; i < column_str.length(); ++i)
	{
		if(i%(m*8 + extra_bit_count - 1) == 0) cout << "\n";
		cout << column_str[i];
	}
	cout << "\n";*/
	return column_str;
 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 5 : CRC checksum calculation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//helper functions
//converts the int generator polynomial to an int vector
vector <int> integerToArray()
{
	vector <int> resultArray;
	//fill(resultArray.begin(), resultArray.end(), 0);
	
	int generator_polynomial_tmp = generator_polynomial;
	while (true)
	{
		resultArray.insert(resultArray.begin(), generator_polynomial_tmp%10);
		generator_polynomial_tmp /= 10;
		if(generator_polynomial_tmp == 0)
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

	//cout<<"Message string : "<<msg<<endl;

	msg_Size = (msg_Size + div_Size) - 1; //new size after appending (divisor_size - 1) zeros at end

	//cout<<"\nNew message bit is " << msg << "\n";

	rem_Size = msg_Size;

	result = msg; //copy message string to result string

	//cout<<"\nResult string : "<<result<<endl;
	
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

    //cout<<"\nCRC code that is generated is " << msg << "\n";

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
    //cout << "actual_checksum is " <<actual_checksum.c_str() << endl;
    //cout<<"\nRemainder is " << rem.c_str() << "\n";
    //cout<<"\nData bits after appending CRC checksum <sent frame> : \n";
    //cout<<original_msg<<"\033[1;36m"<< actual_checksum.c_str() <<"\033[0m\n";
    
    string tmp_actual_checksum = actual_checksum.c_str();
    checksum_Size = tmp_actual_checksum.length();
    //cout << "actual_checksum.length : " << tmp_actual_checksum.length() <<endl;
	
    //cout << "checksum_Size : " << checksum_Size <<endl;
	
	return rem;
}

string original_recv_rem;
//calling method for task5
string task5(string msg)
{

	//configuring msg, size of msg, divisor and size of divisor
	int msg_Size = msg.length(); //msg is the binary string representation of original msg

	vector<int> temp_vec1 = integerToArray();

	int div_Size = temp_vec1.size();
	
	int divisor[div_Size]; //divisor[] array has the generator polynomial
	
	//transferring the divisor in vector to divisor in int array
	for (int i = 0; i < temp_vec1.size(); ++i)
	{
		divisor[i] =  temp_vec1.at(i);
	}

	//frame to be sent
	string recv_rem =  checksum(msg, msg_Size, divisor, div_Size);
	original_recv_rem = recv_rem.c_str();

	string msg_with_checksum = msg + recv_rem.c_str();
	cout<<"\nData bits after appending CRC checksum <sent frame> : \n";
    cout<<msg<<"\033[1;36m"<< recv_rem.c_str() <<"\033[0m\n";
    
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
	error_position = new bool[CRC_frame.length() + 10];
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
	cout<< "\nreceived frame : \n";
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


int my_strcmp(string s, string t) 
{
    int i;
    //cout << "s.size : " << s.length() << "\nt.size : " << t.length() <<endl;
    for (i = 0; s[i] == t[i]; i++)
        if (s[i] == '\0')
            return 0;
    return s[i] - t[i];
}

void task7(string msg)
{

	//configuring msg, size of msg, divisor and size of divisor
	int msg_Size = msg.length(); //msg is the binary string representation of original msg

	vector<int> temp_vec = integerToArray();
	
	int div_Size = temp_vec.size();
	
	int divisor[div_Size]; //divisor[] array has the generator polynomial
	
	//transferring the divisor in vector to divisor in int array
	for (int i = 0; i < temp_vec.size(); ++i)
	{
		divisor[i] =  temp_vec.at(i);
	}

	msg = msg.substr(0, msg_Size - div_Size + 1);
	int msg_Size_without_checksum = msg.length(); //msg is the binary string representation of original msg

	string recvd_frame_checksum = checksum(msg, msg_Size_without_checksum, divisor ,div_Size);
	cout << "recvd_frame_checksum : " << recvd_frame_checksum.c_str() << endl;
	cout << "original_recv_rem : " << original_recv_rem.c_str() <<endl;

	cout<<"\nresult of crc checksum matching : ";
	if(my_strcmp(recvd_frame_checksum.c_str(), original_recv_rem) != 0)
	{
		cout << "error detected \n";
	}
	else	cout << "no error detected\n";


}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 8 : remove crc checksum bits; deserialize
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string task8(string str)
{
	
	int str_Size = str.length();
	str = str.substr(0,str_Size - checksum_Size);
	
	string col_major_str = task4(str, extra_bit_count + 1); // checksum removed
	cout<<"\n~~~ data block after removing crc checksum bits : ~~~";
	for (int i = 0; i < col_major_str.length(); ++i)
	{
		//if(i%(m*8 + extra_bit_count ) == 0) cout << "\n";
		if(i%20 == 0) cout << "\n";
		cout << col_major_str[i];
	}
	cout << "\n";
	return col_major_str;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//						task 9 : correction of error
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string task9(string str)
{
	cout << "data block after removing check bits\n";
	string new_str;
	for (int i = 0; i < str.length(); ++i)
	{
		if(!check_bit_positions[i]) new_str += str[i];
	}
	
	for (int i = 0; i < new_str.length(); ++i)
	{
		if(i%(m*8) == 0) cout << "\n";
		cout << new_str[i];
	}
	cout << endl;
	return new_str;
}

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

void task10(string binary_Str)
{
	cout << "binary string :\n";
	cout << binary_Str <<endl;
	BinaryStringToText(binary_Str);
}

int main()
{
	
	/*data = "Hamming Code";
	m = 2;
	p = 0.05;
	generator_polynomial = 10101; //generator_polynomial is of int type, in task 5,7 it is converted to int array to get the bits separately
*/
	data = "a";
	m = 1;
	p = 0.05;
	generator_polynomial = 101;
	
	printf("Enter data string : %s\n", data.c_str());
	printf("enter number of data bytes in a row (m) : %d\n", m);
	printf("enter probability (p) : %.2lf\n",p);
	printf("enter generator polynomial : %d\n",generator_polynomial);


	//simple test
	task1();
	printf("data block <ascii code of m characters per row>\n");
	string strForTask3 = task2();

	string str_with_checkbits = task3(strForTask3);
	//task4(str_with_checkbits, extra_bit_count);
	
	cout<< "\ndata bits after column wise Serialization : \n";
	task4("110111010001",4); //takes as input the data bit with checksum
	
	string crc_frame = task5("110111010001");
	string recv_frame = task6(crc_frame);

	task7(recv_frame);
	string from8 = task8(recv_frame);
	string from9 = task9(from8);
	task10(from9);
	

	/*task1();
	printf("data block <ascii code of m characters per row>\n");
	string strForTask3 = task2();

	string str_with_checkbits = task3(strForTask3);
	
	//
	str_with_checkbits = "000010011000011100001100011011101011101101010011001001011101110100011000111001000000110010000011011001111010111010100011000101" ;

	cout<< "\ndata bits after column wise Serialization : \n";
	string from4 = task4(str_with_checkbits, extra_bit_count);
	//cout<< "returned from 4 : " << from4 << endl;
	//task4("110111010001",4); //takes as input the data bit with checksum
	
	string crc_frame = task5(from4);
	//string crc_frame = task5(strForTask3);
	//string crc_frame = task5("110111010001");
	string recv_frame = task6(crc_frame);

	task7(recv_frame);
	string from8 = task8(recv_frame);
	string from9 = task9(from8);
	task10(from9);*/


	
	return 0;
}