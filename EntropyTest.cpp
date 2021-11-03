// EntropyTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <vector>

#define distinct_cities		23
#define number_cities		106346945
#define dimension_data		2000

int frequency[distinct_cities] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char cities[distinct_cities][10];							// for frequency count
char cities_name[number_cities][4];							// each line of cities in sampleCities.txt
float data[number_cities];								// each line of data in out.txt
int cluster_node[dimension_data][distinct_cities];			// for keeping track of cluster nodes

// Variables for entropy
float entropy[dimension_data] = { 0 };
float global_entropy = 0;
int total_vectors[dimension_data] = { 0 };

// Init function for initialising variables
void Init()
{
	// Initialise cities
	char city1[] = "ARN", city2[] = "BCN", city3[] = "BER", city4[] = "DEN", city5[] = "DOH", city6[] = "FAI",
		city7[] = "HKG", city8[] = "ICN", city9[] = "IEV", city10[] = "ILR", city11[] = "KUL", city12[] = "LCY",
		city13[] = "LIS", city14[] = "NYC", city15[] = "OFF", city16[] = "SAO", city17[] = "SCL", city18[] = "SDJ",
		city19[] = "SFO", city20[] = "SGP", city21[] = "TPE", city22[] = "TYO", city23[] = "ZRH";
	strcpy(cities[0], city1); strcpy(cities[1], city2); strcpy(cities[2], city3);
	strcpy(cities[3], city4); strcpy(cities[4], city5); strcpy(cities[5], city6);
	strcpy(cities[6], city7); strcpy(cities[7], city8); strcpy(cities[8], city9);
	strcpy(cities[9], city10); strcpy(cities[10], city11); strcpy(cities[11], city12);
	strcpy(cities[12], city13); strcpy(cities[13], city14); strcpy(cities[14], city15);
	strcpy(cities[15], city16); strcpy(cities[16], city17); strcpy(cities[17], city18);
	strcpy(cities[18], city19); strcpy(cities[19], city20); strcpy(cities[20], city21);
	strcpy(cities[21], city22); strcpy(cities[22], city23);
	// Initialise node frequency count
	for (int i = 0; i < dimension_data; i++)
	{
		memcpy(cluster_node[i], frequency, distinct_cities);
	}
}

// Read from sampleCities.txt and store the cities
// Read from out.txt and store each line of values to the respective cities
void ReadInputFiles(const char* sample_cities, const char* input_name)
{
	// cities
	//FILE* cities_file = fopen(sample_cities, "r");
	FILE* cities_file;
	errno_t cityOK = fopen_s(&cities_file, sample_cities, "r");

	if (cityOK != 0)
	{
		fprintf(stderr, "Error: failed to open file %s (Hint: check your working directory)\n", sample_cities);
		exit(1);
	}

	int i = 0;
	char name[100];

	while(fgets(name, 100, cities_file))
	{
		// For extracting only the city names
		int nDigits = floor(log10(abs(i+1))) + 2;
		// Store the names in a global array
		memcpy(cities_name[i], name + nDigits, 3);
		cities_name[i][3] = '\0';
		i++;
	}
	fclose(cities_file);
	// input data
	FILE* input_file;
	errno_t dataOK = fopen_s(&input_file, input_name, "r");

	if (dataOK != 0)
	{
		fprintf(stderr, "Error: failed to open file %s (Hint: check your working directory)\n", input_name);
		exit(1);
	}

	int j = 0;
	char inputVal[dimension_data];
	while(fgets(inputVal, dimension_data, input_file))
	{
		// Sum up the values
		char* numData = inputVal;
		char* p;
		float sum = 0;
		float value;
		
		for (int n = 0; n < 150; n++) {
			value = strtod(numData, &p);
			numData = p;
			if (p)
			{				
				sum = sum + value;
			}
		}
		// Store the data in a global array
		data[j] = sum;
		j++;
	}
	fclose(input_file);
}

// Read from outresults.txt and store the cluster nodes
// for each member, find it's city and add to frequency count
void ReadKTree(const char* ktree)
{
	//FILE* ktree_file = fopen(ktree, "r");
	FILE* ktree_file;
	errno_t ktreeOK = fopen_s(&ktree_file, ktree, "r");

	if (ktreeOK != 0)
	{
		fprintf(stderr, "Error: failed to open file %s (Hint: check your working directory)\n", ktree);
		exit(1);
	}

	int i = -1;
	char node[dimension_data], real_node[dimension_data];
	char* pch;
	char* temp[2];
	bool isCluster = false;

	while(fgets(node, dimension_data, ktree_file))
	{
		memcpy(real_node, node, dimension_data);
		// Sum up the values
		char* numData = node;
		char* p;
		float sum = 0;
		float value;
		for (int n = 0; n < 2; n++) {
			value = strtod(numData, &p);
			numData = p;
		}
		for (int n = 0; n < 150; n++) {
			value = strtod(numData, &p);
			numData = p;
			if (p)
			{
				sum = sum + value;
			}
		}
		// Split line into first two values
		pch = strtok(node, " ");
		for (int j = 0; j < 2; j++)
		{
			temp[j] = pch;			
			pch = strtok(NULL, " ");
		}
		// If first value == second value, it is a cluster
		if (strcmp(temp[0],temp[1]) == 0)
		{			
			// For each element in data, find the matching value and its index
			i++;
			char temp_name[3];
			for (int k = 0; k < number_cities; k++)
			{	
				if (data[k] == sum)
				{
					// Find the corresponding city using the index
					strcpy(temp_name, cities_name[k]);
				}
			}
			// Find the frequency index in cluster node
			int city_index;
			for (int k = 0; k < 23; k++)
			{
				if (strcmp(cities[k], temp_name) == 0)
				{
					city_index = k;
				}
			}
			// Update frequency count for each city in each cluster node
			cluster_node[i][city_index]++;
		}

		// Keep track of every first value == 0 until next cluster
		if (strcmp(temp[0], "0") == 0)
		{
			char temp_name[3];
			for (int k = 0; k < number_cities; k++)
			{
				if (data[k] == sum)
				{
					// Find the corresponding city using the index
					strcpy(temp_name, cities_name[k]);
					printf("%s\n", temp_name);
				}
			}
			// Find the frequency index in cluster node
			int city_index;
			for (int k = 0; k < 23; k++)
			{
				if (strcmp(cities[k], temp_name) == 0)
				{
					city_index = k;
				}
			}
			// Update frequency count for each city in each cluster node
			cluster_node[i][city_index]++;
		}
	}
}

// Evaluate for entropy value for each cluster
float EntropyTest()
{
	int count = 0;
	// For each member in cluster node evaluate cluster size
	for (int i = 0; i < dimension_data; i++)
	{
		for (int j = 0; j < distinct_cities; j++)
		{
			total_vectors[i] = total_vectors[i] + cluster_node[i][j];
		}
		if (total_vectors[i] == 0)
		{
			count++;
			break;
		}
	}
	// For each member in cluster node evaluate for entropy value
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < distinct_cities; j++)
		{
			float p = (float)cluster_node[i][j] / (float)total_vectors[i];
			float log_p = log2(p+FLT_EPSILON);
			printf("log_p: %f\n", log_p);
			entropy[i] = entropy[i] + ((p+FLT_EPSILON)*log_p);
		}
		entropy[i] = -(entropy[i]);
	}
	// Calculaten for the global entropy
	for (int i = 0; i < count; i++)
	{
		global_entropy = global_entropy + (entropy[i] / total_vectors[i]);
	}
	return global_entropy;
}

int main()
{
	// Start timing
	auto start = std::chrono::high_resolution_clock::now();
	// Read files and store data 
	Init();
	ReadInputFiles("sampleCities.txt", "out.txt");
	ReadKTree("outresults500.txt");
	// Calculate entropy
	float entropyVal = EntropyTest();
	printf("The global entropy value is: %f\n", entropyVal);
	// End timing
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	printf("time elapsed: %f seconds\n", elapsed.count());

	return 0;
}


