/*I've implemented this program to solve another program. I've been asked to write a multithreading program performing some 
mathematical operations on a set of input data passed through a binary file to the main function. One of the threads was supposed
to compute, for each input number, the overall product of the numbers received by that time.

I realized that, by simply passing a set of numbers which progressively grew from 1 to something more than 20  (even 21) I was 
not able to represent the result of the multiplication even on a LONGLONG data type (which is the biggest available).

So, I've decided to implement a digit by digit multiplication which basically simply applies the multiplication algorithm and 
saves each digit in a cell of an array. The result is then printed out, by scanning the array.

The program is not optimized, since I wrote it in a kind of hurry and there could be some mistakes. Moreover, it is implemented
in a way which is somehow functional to my initial assignment. But, the important thing is the algorithm I've used, which, 
I think could be interesting.
*/
#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

/*Maximum number of digits of the res of our multiplication*/
#define MAX 50

DWORD multiply(DWORD, DWORD*, DWORD);
DWORD parseInput(DWORD, DWORD *);

int _tmain(DWORD argc, LPTSTR argv[]) {
	DWORD i, nIn, buffer, res_size, res[MAX];	//res will store the overall product at each time
	HANDLE hIn;

	if (argc != 2) {
		_ftprintf(stderr, _T("Missing input file\r\n")); return 1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		_ftprintf(stderr, _T("Invalid handle value\r\n")); return 2;
	}


	/*I copy the first number directly inside the res array so that I don't have to care about the first multiplication*/
	if (ReadFile(hIn, &buffer, sizeof(buffer), &nIn, NULL) && nIn) {
		_ftprintf(stdout, _T("Input: %d. "), buffer);
		res_size = parseInput(buffer, res);
	}else {
		_ftprintf(stderr, _T("Can't read: no input or error in reading\r\n")); return 3;
	}
	
	_ftprintf(stdout, _T("Overall product: "));
	for (i = res_size; i > 0; i--)
		_ftprintf(stdout, _T("%d"), res[i-1]);
	_ftprintf(stdout, _T("\r\n"));
	
	/*I start from the second integer*/
	while (ReadFile(hIn, &buffer, sizeof(buffer), &nIn, NULL) && nIn > 0) {
		_ftprintf(stdout, _T("Input: %d. "), buffer);
		res_size = multiply(buffer, res, res_size);
		_ftprintf(stdout, _T("Overall product: "));
		for (i = res_size; i > 0; i--)
			_ftprintf(stdout, _T("%d"), res[i-1]);
		_ftprintf(stdout, _T("\r\n"));
	}

	CloseHandle(hIn);
	return 0;
}

DWORD parseInput(DWORD n, DWORD *input){
	DWORD i = 0;

	while (n) {
		input[i] = n % 10;	//last digit of n in the rightmost free position of the array
		n = n / 10;			//I consider next digit
		i++;
	}

	return i;
}

DWORD multiply(DWORD n, DWORD *res, DWORD res_size) {
	DWORD prod, new_size, carry, carry1, i, j, digitsN, input[MAX], tmp[MAX];		//the input num is gonna be parsed so i can multiply each digit by each digit
												// of the overall product

	digitsN = parseInput(n, input);

	/*i need a tmp array to store intermediate results*/
	for (i = 0; i < MAX; i++)
		tmp[i] = 0;

	i = 0;
	while (i < digitsN) {
		j = 0;
		carry = (carry1 = 0);
		/*I multiply each digit of the current tmp res by each digit of the  input value
		 and I sum all tmp results up digit by digit. Remember that for each shift of one tens on the left on the input value
		 I need to multiply by ten the current tmp res. Ex:
		 44*4 ->

		 1) 4*4 = 12 (m.1)
		 2) 4*4 = 12 (m.2)

		 res =  12*10 (one shift left) + 12 = 132

		*/
		DWORD val;

		while (j < res_size) {
			prod = res[j] * input[i] + carry;
			carry = prod / 10;
			val =  (tmp[i + j] + carry1 + prod % 10) % 10;
			/*I can produce a carry also when summing up tmp res and I must consider it*/
			carry1 = (tmp[i + j] + carry1 + prod % 10) / 10;
			tmp[i + j] = val;
			j++;
		}
		/*last position where I wrote + 1*/
		new_size = i + j;
		/*saving last carry*/
		tmp[i + j] = carry + carry1; //i save it only if it is = 0 because at second next cycle on i it has to have a valid value
		if (tmp[i + j] != 0)
			new_size++;
		i++;
	}


	for (i = 0; i < new_size; i++)
		res[i] = tmp[i];

	return new_size;
}
