#include "CodeGenerator.hpp"

#include <stdio.h>

#ifdef WIN32
	#include <conio.h>
#else
	inline int getch() {return fgetc(stdin);}
#endif

using namespace SoftWire;

void testHelloWorld()
{
	printf("HelloWorld is a function which uses printf to print a message.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling HelloWorld.asm...\n\n");
	
	ASM_EXPORT(printf);
	Assembler x86("HelloWorld.asm");

	void (*helloWorld)() = (void(*)())x86.callable();

	if(helloWorld)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			printf("output: ");
			helloWorld();
			printf("\n\n");
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testSetBits()
{
	printf("SetBits is a function which sets a number of bits in a buffer starting from a given bit. In this example it starts at bit 5 and sets 44 bits (viewed right to left).\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling SetBits.asm...\n\n");

	Assembler x86("SetBits.asm");

	void (*setBits)(unsigned int*, int, int) = (void(*)(unsigned int*, int, int))x86.callable();

	if(setBits)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			unsigned int bitBuffer[] = {0x00000000, 0x00000000};

			setBits(bitBuffer, 5, 44);

			printf("output: %.8X %.8X\n\n", bitBuffer[1], bitBuffer[0]);
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testCrossProduct()
{
	printf("CrossProduct is a function which computes the cross product of two vectors. In this example it computes (1, 0, 0) x (0, 1, 0).\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling CrossProduct.asm...\n\n");

	Assembler x86("CrossProduct.asm");

	void (*crossProduct)(float*, float*, float*) = (void(*)(float*, float*, float*))x86.callable();

	if(crossProduct)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			float V0[3] = {1, 0, 0};
			float V1[3] = {0, 1, 0};
			float V2[3];

			crossProduct(V0, V1, V2);

			printf("output: (%.3f, %.3f, %.3f)\n\n", V2[0], V2[1], V2[2]);
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testAlphaBlend()
{
	printf("AlphaBlend is a function which blends two RGBA colors. SoftWire will conditionally compile for Katmai instructions. In this example, 0x00FF00FF is blended with 0x7F007F00 with a blending factor of 64 / 256.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling AlphaBlend.asm...\n\n");

	bool katmai;

	printf("Do you have a Katmai compatible processor (y/n)?\n\n");

	int c;
	do
	{
		c = getch();
	}
	while(c != 'y' && c != 'n');

	if(c == 'y')
	{
		katmai = true;
	}
	else
	{
		katmai = false;
	}

	ASM_DEFINE(katmai);
	Assembler x86("AlphaBlend.asm");

	int (*alphaBlend)(int, int, int) = (int(*)(int, int, int))x86.callable();

	if(alphaBlend)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			int x = alphaBlend(0x00FF00FF, 0x7F007F00, 64);

			printf("output: %.8X\n\n", x);
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testFactorial()
{
	printf("Factorial is a function which computes the factorial of an integer using recursion. In this example, 5! is computed.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling Factorial.asm...\n\n");

	Assembler x86("Factorial.asm");

	int (*factorial)(int) = (int(*)(int))x86.callable();

	if(factorial)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			int x = factorial(5);

			printf("output: %d\n\n", x);
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testMandelbrot()
{
	printf("Mandelbrot is a function which draws the Mandelbrot fractal.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();
	printf("Assembling Mandelbrot.asm...\n\n");

	ASM_EXPORT(printf);

	Assembler x86("Mandelbrot.asm");

	int (*mandelbrot)() = (int(*)())x86.callable();

	if(mandelbrot)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			mandelbrot();
		}

		printf("\n\n");
	}
	else
	{
		printf(x86.getErrors());
	}
}

void testIntrinsics()
{
	printf("Testing run-time intrinsics.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();

	Assembler x86;

	static char string[] = "All working!";

	x86.push((int)string);
	x86.call((int)printf);
	x86.add(esp, 4);
	x86.ret();

	void (*emulator)() = (void(*)())x86.callable();

	if(emulator)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			printf("output: ");
			emulator();
			printf("\n\n");
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

class TestRegisterAllocator : public CodeGenerator
{
public:
	TestRegisterAllocator()
	{
		x1 = 1;
		x2 = 2;
		x3 = 3;
		x4 = 4;
		x5 = 5;
		x6 = 6;
		x7 = 7;
		x8 = 8;
		x9 = 9;

		static int t1 = 0;
		static int t2 = 0;
		static int t3 = 0;
		static int t4 = 0;
		static int t5 = 0;
		static int t6 = 0;
		static int t7 = 0;
		static int t8 = 0;
		static int t9 = 0;

		pushad();

		mov(x32(&t1), m32(&x1));
		mov(x32(&t2), m32(&x2));
		mov(x32(&t3), m32(&x3));
		mov(x32(&t4), m32(&x4));
		mov(x32(&t5), m32(&x5));
		mov(x32(&t6), m32(&x6));
		mov(x32(&t7), m32(&x7));
		mov(x32(&t8), m32(&x8));
		mov(x32(&t9), m32(&x9));

		mov(m32(&x1), r32(&t9));
		mov(m32(&x2), r32(&t8));
		mov(m32(&x3), r32(&t7));
		mov(m32(&x4), r32(&t6));
		mov(m32(&x5), r32(&t5));
		mov(m32(&x6), r32(&t4));
		mov(m32(&x7), r32(&t3));
		mov(m32(&x8), r32(&t2));
		mov(m32(&x9), r32(&t1));

		popad();
		ret();
	}

	int x1;
	int x2;
	int x3;
	int x4;
	int x5;
	int x6;
	int x7;
	int x8;
	int x9;
};

void testRegisterAllocator()
{
	printf("Testing register allocator. SoftWire will swap nine numbers using nine virtual general-purpose registers.\n\n");
	printf("Press any key to start assembling\n\n");
	getch();

	TestRegisterAllocator x86;

	void (*script)() = (void(*)())x86.callable();

	if(script)
	{
		printf("%s\n\n", x86.getListing());
		printf("Execute code (y/n)?\n\n");

		int c;
		do
		{
			c = getch();
		}
		while(c != 'y' && c != 'n');

		if(c == 'y')
		{
			printf("Input:  %d %d %d %d %d %d %d %d %d\n", x86.x1, x86.x2, x86.x3, x86.x4, x86.x5, x86.x6, x86.x7, x86.x8, x86.x9);
			script();
			printf("output: %d %d %d %d %d %d %d %d %d\n", x86.x1, x86.x2, x86.x3, x86.x4, x86.x5, x86.x6, x86.x7, x86.x8, x86.x9);
			printf("\n");
		}
	}
	else
	{
		printf(x86.getErrors());
	}
}

int main()
{
	testHelloWorld();
	testSetBits();
	testCrossProduct();
	testAlphaBlend();
	testFactorial();
	testMandelbrot();
	testIntrinsics();
	testRegisterAllocator();

	printf("Press any key to continue\n");
	getch();
	
	return 0;
}
