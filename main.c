
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/random.h>

#define SIZE 100
#define FCFS 1
#define LCFSNP 2
#define LCFSP 3
#define RR 4
#define SJF 5


struct Stack 
{
	int top;

	unsigned capacity;

	int* array;
};

struct Stack* createStack(unsigned capacity)
{
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));

	stack->capacity = capacity;

	stack->top = -1;

	stack->array = (int*)malloc(stack->capacity * sizeof(int));

	return stack;
}

int isEmpty(struct Stack* stack) 
{ 
	return stack->top == -1;
}

void push(struct Stack* stack, int item)
{ 
	stack->array[++stack->top] = item;
}

int pop(struct Stack* stack)
{
	if (isEmpty(stack))
		return -1;

	return stack->array[stack->top--];
}

void swap(int* num1, int* num2)
{
	int temp = *num1;

	*num1 = *num2;

	*num2 = temp;
}

int readLine(char* buffer1, char* buffer2, int i)
{
	int count = 0;

	while (buffer1[i] != '\n')
		buffer2[count++] = buffer1[i++];

	buffer2[count++] = buffer1[i++];

	return i;
}


void SaveData(int index, char* file, int* insertionTime, int* executionTime, int size)
{
	for (int i = 0; i < size; i++)
	{
		char line[SIZE];

		index = readLine(file, line, index);

		char buffer1[SIZE];

		char buffer2[SIZE];

		int j = 0;

		while (line[j] != ',')
		{
			buffer1[j] = line[j];

			j++;
		}

		j++;

		int k = 0;

		while (line[j] != '\n')
		{
			buffer2[k] = line[j];

			j++;

			k++;
		}

		insertionTime[i] = atoi(buffer1);

		executionTime[i] = atoi(buffer2);
	}
}

void bSort(int* arr1, int* arr2, int size)
{
	int i, j;

	for (i = 0; i < size - 1; i++)
		for (j = 0; j < size - i - 1; j++)
			if (arr1[j] > arr1[j + 1])
			{
				swap(&arr1[j], &arr1[j + 1]);

				swap(&arr2[j], &arr2[j + 1]);
			}
}


void FindWaitingTimeFCFS(int* insertionTime, int size, int* executionTime, int waitingTimeArr[])
{
	waitingTimeArr[0] = 0;

	for (int i = 1; i < size; i++)
	{
		if (waitingTimeArr[i - 1] + executionTime[i - 1] + insertionTime[i - 1] < insertionTime[i])
			waitingTimeArr[i] = 0;

		else
			waitingTimeArr[i] = executionTime[i - 1] + waitingTimeArr[i - 1] - (insertionTime[i] - insertionTime[i - 1]);
	}
}


void FindWaitingTimeLCFSNP(int* insertionTime, int size, int* executionTime, int waitingTimeArr[])
{
	waitingTimeArr[0] = 0;

	int arrInsertion[size];

	int arrExecution[size];

	int index = 1;

	int indexInsertion = 1;

	int indexWaiting = 1;

	struct Stack* data1 = createStack(size);

	struct Stack* data2 = createStack(size);

	arrInsertion[0] = insertionTime[0];

	arrExecution[0] = executionTime[0];

	while (index < size - 1)
	{
		if (indexInsertion < size)
		{
			if (insertionTime[indexInsertion] <= waitingTimeArr[indexWaiting - 1] + arrExecution[index - 1] + arrInsertion[index - 1])
			{
				push(data1, insertionTime[indexInsertion]);

				push(data2, executionTime[indexInsertion]);

				indexInsertion++;
			}
			else
			{
				if (isEmpty(data1))
				{
					arrInsertion[index] = insertionTime[indexInsertion];

					arrExecution[index] = executionTime[indexInsertion];

					index++;

					indexInsertion++;
				}
				else
				{
					arrInsertion[index] = pop(data1);

					arrExecution[index] = pop(data2);

					waitingTimeArr[index] = waitingTimeArr[index - 1] + arrExecution[index - 1] + arrInsertion[index - 1] - arrInsertion[index];

					if (waitingTimeArr[index] <= 0)
						waitingTimeArr[index] = 0;

					index++;

				}
			}
		}
		else
		{
			while (!isEmpty(data1))
			{
				arrInsertion[index] = pop(data1);

				arrExecution[index] = pop(data2);

				waitingTimeArr[index] = waitingTimeArr[index - 1] + arrExecution[index - 1] + arrInsertion[index - 1] - arrInsertion[index];

				if (waitingTimeArr[index] <= 0)
					waitingTimeArr[index] = 0;

				index++;
			}
		}
	}
}


void FindWaitingTimeLCFSP(int* insertionTime, int size, int* executionTime, int waitingTimeArr[])
{
	waitingTimeArr[size - 1] = 0;

	for (int i = size - 2; i >= 0; i--)
	{
		if (insertionTime[i] + executionTime[i] < insertionTime[i + 1])
			waitingTimeArr[i] = 0;

		else
			waitingTimeArr[i] = waitingTimeArr[i + 1] + executionTime[i + 1];

	}
}

void FindWaitingTimeRR(int* insertionTime, int size, int* executionTime, int waitingTimeArr[])
{
	int quantunSize = 2;

	int i = 0;

	int remainProcess = size;

	int remainTime[size];

	int index = 0;

	int flag1 = 0;

	int flag2 = 0;

	int sum1 = 0;

	int sum2 = 0;

	for (int j = 0; j < size; j++)
		remainTime[j] = executionTime[j];

	while (remainTime[index] == 0)
	{
		if (sum1 == 0)
			sum1++;

		sum1 += insertionTime[index + 1] - insertionTime[index];

		index++;

		remainProcess--;

		waitingTimeArr[index] = 0;
	}

	for (i = index; remainProcess != 0;)
	{
		flag2 = 0;

		if (remainTime[i] != 0)
		{
			if (remainTime[i] <= quantunSize && remainTime[i] > 0)
			{
				sum1 += remainTime[i];

				remainTime[i] = 0;

				flag1 = 1;

				flag2 = 1;
			}

			else if (remainTime[i] > 0)
			{
				remainTime[i] -= quantunSize;

				sum1 += quantunSize;

				flag2 = 1;
			}

			if (flag1 == 1 && remainTime[i] == 0)
			{
				if (sum1 - insertionTime[i] - executionTime[i] <= 0)
				{
					waitingTimeArr[i] = 0;

					sum2 += 0;
				}

				else
				{
					waitingTimeArr[i] = sum1 - insertionTime[i] - executionTime[i];

					sum2 += sum1 - insertionTime[i] - executionTime[i];
				}

				flag1 = 0;

				remainProcess--;
			}
		}

		if (i == size - 1)
			i = 0;

		else if (insertionTime[i + 1] <= sum1)
			i++;

		else if (flag2 == 0)
			sum1 += 1;

		else
			i = 0;
	}
}

void FindWaitingTimeSJF(int* insertionTime, int size, int* executionTime, int waitingTimeArr[])
{
	int small;

	int count = 0;

	int time = 0;

	int finish;

	int remainTime[size + 1];

	for (int i = 0; i < size; i++)
		remainTime[i] = executionTime[i];

	remainTime[size] = 9999;

	while (executionTime[time] == 0)
	{
		count++;

		time++;
	}

	for (time; count != size; time++)
	{
		small = size;

		for (int i = 0; i < size; i++)
		{
			if (insertionTime[i] <= time && remainTime[i] < remainTime[small] && remainTime[i] > 0)
				small = i;
		}

		remainTime[small]--;

		if (remainTime[small] == 0)
		{
			count++;

			finish = time + 1;

			waitingTimeArr[small] = finish - insertionTime[small] - executionTime[small];
		}
	}

}

void FindTurnAroundTime(int* insertionTime, int size, int* executionTime, int waitingTimeArr[], int turnAroundTimeArr[])
{
	for (int i = 0; i < size; i++)
		turnAroundTimeArr[i] = executionTime[i] + waitingTimeArr[i];
}

void FindTurnAroundTimeRR(int* insertionTime, int size, int* executionTime, int waitingTimeArr[], int turnAroundTimeArr[])
{
	int sumWaitTime = 0;

	int sumExecutionTime = 0;

	for (int i = 0; i < size; i++)
	{
		sumWaitTime += waitingTimeArr[i];

		sumExecutionTime += executionTime[i];
	}

	turnAroundTimeArr[0] = sumExecutionTime + sumWaitTime;

}

float FindAvg(int* turnAroundTimeArr, int size)
{
	float numberOfProcess = 0;

	for (int i = 0; i < size; i++)
		numberOfProcess += turnAroundTimeArr[i];

	float avg = (float)numberOfProcess / (float)size;

	return avg;
}

void RunCpu(int* insertionTime, int* executionTime, int size, int scheduling)
{
	int waitingTimeArr[size];

	int turnAroundTimeArr[size];

	if (scheduling == FCFS)
	{
		FindWaitingTimeFCFS(insertionTime, size, executionTime, waitingTimeArr);

		FindTurnAroundTime(insertionTime, size, executionTime, waitingTimeArr, turnAroundTimeArr);

		float avg = FindAvg(turnAroundTimeArr, size);

		printf("FCFS: mean turnaround = %.3f\n", avg);
	}

	if (scheduling == LCFSNP)
	{
		FindWaitingTimeLCFSNP(insertionTime, size, executionTime, waitingTimeArr);

		FindTurnAroundTime(insertionTime, size, executionTime, waitingTimeArr, turnAroundTimeArr);

		float avg = FindAvg(turnAroundTimeArr, size);

		printf("LCFS (NP): mean turnaround = %.3f\n", avg);
	}

	if (scheduling == LCFSP)
	{
		FindWaitingTimeLCFSP(insertionTime, size, executionTime, waitingTimeArr);

		FindTurnAroundTime(insertionTime, size, executionTime, waitingTimeArr, turnAroundTimeArr);

		float avg = FindAvg(turnAroundTimeArr, size);

		printf("LCFS (P): mean turnaround = %.3f\n", avg);
	}

	if (scheduling == RR)
	{
		FindWaitingTimeRR(insertionTime, size, executionTime, waitingTimeArr);

		FindTurnAroundTimeRR(insertionTime, size, executionTime, waitingTimeArr, turnAroundTimeArr);

		float avg = (float)turnAroundTimeArr[0] / (float)size;

		printf("RR: mean turnaround = %.3f\n", avg);
	}

	if (scheduling == SJF)
	{
		FindWaitingTimeSJF(insertionTime, size, executionTime, waitingTimeArr);

		FindTurnAroundTime(insertionTime, size, executionTime, waitingTimeArr, turnAroundTimeArr);

		float avg = FindAvg(turnAroundTimeArr, size);

		printf("SJF: mean turnaround = %.3f\n", avg);
	}
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Worng number of arguments\n");

		exit(-1);
	}

	int filefd = open(argv[1], O_RDONLY);

	if (filefd < 0)
	{
		printf("Can't open the file\n");

		exit(-1);
	}

	char data[1000];

	char line[1000];

	read(filefd, data, 1000);

	close(filefd);

	int i = readLine(data, line, 0);

	int size = atoi(line);

	int* insertionTimeArr = (int*)malloc(size * sizeof(int));

	int* executionTimeArr = (int*)malloc(size * sizeof(int));

	if (insertionTimeArr == NULL || executionTimeArr == NULL)
	{
		printf("Can't allocate memory\n");

		exit(-1);
	}

	SaveData(i, data, insertionTimeArr, executionTimeArr, size);

	bSort(insertionTimeArr, executionTimeArr, size);

	printf("******************************************************\n");

	RunCpu(insertionTimeArr, executionTimeArr, size, FCFS);

	RunCpu(insertionTimeArr, executionTimeArr, size, LCFSNP);

	RunCpu(insertionTimeArr, executionTimeArr, size, LCFSP);

	RunCpu(insertionTimeArr, executionTimeArr, size, RR);

	RunCpu(insertionTimeArr, executionTimeArr, size, SJF);

	printf("******************************************************\n");

	free(insertionTimeArr);

	free(executionTimeArr);

}