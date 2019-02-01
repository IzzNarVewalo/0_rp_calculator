#include <stdio.h>
#include <stdlib.h>

typedef struct {
	double euro;
	int rp;
} purchase_t;

typedef struct {
	char *name;
	int cost;
} sink_t;

purchase_t options[] = {
	{2.5,   350},
	{  5,   750},
	{ 10,  1580},
	{ 20,  3250},
	{ 25,  4100},
	{ 35,  5725},
	{ 50,  8250},
	{100,  17250},
};

sink_t sink[] = {
	{"Gem",				   5000},
	{"Legendary Skin",	   1820},
	{"1350 Skin",		   1350},
	{"975 sale",			487},
	{"1350 on sale",		675},
	{"975 Skin",			975},
	{"Orb",					250},
	{"mchst+key",			225},
	{"chest+key",			195},
	{"880 sale",			440},
	{"880 Champ",			880},
	{"750 Skin",			750},
	{"520 Skin",			520},
	{"Emote",				350},
	{"chest/key",			125},
	{"mchst",				165},
};

int options_c = sizeof(options) / sizeof(purchase_t);
int sink_c = sizeof(sink) / sizeof(sink_t);


void usage(){
	printf("0rp CURRENTRP LIMITINCURRENCY\n");
}

int *spendRP(int currentRP) {
	int *lookupTable, **tablePtr, count, value, *s;
	lookupTable = calloc((sink_c + 1) * (currentRP + 1), sizeof(int));
	tablePtr    = malloc((sink_c + 1) * sizeof(int*));
	tablePtr[0] = lookupTable;
	for (int i = 1; i <= sink_c; i++){
		tablePtr[i] = &lookupTable[i * (currentRP + 1)];
		for (int j = 0; j <= currentRP; j++){
			tablePtr[i][j] = tablePtr[i - 1][j];
			count = j/sink[i - 1].cost;
			value = tablePtr[i - 1][j - count * sink[i - 1].cost] + count * sink[i - 1].cost;
			if (value > tablePtr[i][j]){
				tablePtr[i][j] = value;
			}
		}
	}

	s = calloc(sink_c, sizeof(int));
	// cant bring RP down to 0
	if (tablePtr[sink_c][currentRP] != currentRP){
		s[0] = -1;
		return s; 
	}
	for (int i = sink_c, j = currentRP; i > 0; i--){
		value = tablePtr[i][j];
		for (count = 0; value != tablePtr[i - 1][j] + count * sink[i - 1].cost; count++){
			s[i - 1]++;
			j -= sink[i - 1].cost;
		}
	}

	free(lookupTable);
	free(tablePtr);
	return s;
}

void buyRP(double money, int **configPtr, int *currentConfig, int i, int *j){
	if (money < 0){
	return;	
	}
	if (money == 0){
		for (int k = 0; k < options_c; k++){
			configPtr[(*j)][k] = currentConfig[k];
		}
		j[0]++;
		return;
	}

	while (i < options_c && options[i].euro <= money){
		currentConfig[i]++;
		buyRP(money - options[i].euro, configPtr, currentConfig, i, j);
		i++;
		currentConfig[i - 1]--;
	}
}

int *buyRPHelper(double money, int rp){
	int count, *countAddr, *ret, *currentConfig, *possibleConfigs, **configPtr;
	int totalRpBought = 0, currentRP = rp;
	double  totalCost = 0;
	count = 0;
	countAddr = &count;
	ret 			= calloc(sink_c, sizeof(int));
	currentConfig   = calloc(options_c, sizeof(int));
	possibleConfigs = calloc((options_c) * 1024, sizeof(int));
	configPtr       = malloc(1024 * sizeof(int*));
	ret[0] = -1;

	configPtr[0] = possibleConfigs;
	for(int i = 0; i < 1024; i++){
		configPtr[i] = &possibleConfigs[i * options_c];
		for (int j = 0; j < options_c; j++)
			configPtr[i][j] = 0;
	}
	buyRP(money, configPtr, currentConfig, 0, countAddr);
	 for (int i = count; i > 0; i--){
	 	for (int j = 0; j < options_c; j++){
	 		rp += configPtr[i - 1][j] * options[j].rp;
	 			currentConfig[j] = configPtr[i - 1][j];
	 	}
	 	if (rp >= 0){
	 		ret = spendRP(rp);
	 		if(ret[0] >= 0){
	 			printf("new total rp: %d\n", rp);
	 			for (int j = 0; j < options_c; j++){
	 				if (currentConfig[j]){
	 					printf("%4dx %5d RP, each %3.2f€\n", currentConfig[j], options[j].rp, options[j].euro);
	 					totalRpBought += currentConfig[j] * options[j].rp;
	 					totalCost += currentConfig[j] * options[j].euro;
	 				}
	 			}
	 			printf("for a total of %6d RP for %3.2f€\n", totalRpBought, totalCost);
	 			break;
	 		}
	 	}
	 	// reset
	 	rp = currentRP;
	}
	free(currentConfig);
	free(possibleConfigs);
	free(configPtr);
	return ret;
}

int main(int argc, char *argv[]){
if (argc != 3)
		usage();
	int RP = atoi(argv[1]), totalRpBought, *sinked, *rpConfig;
	double euroAvailable = atof(argv[2]), euroSpent = 0;

	printf("current RP: %i\nmax money spending: %5.2f\n\n", RP, euroAvailable);
	if (RP >= 0){
		sinked = spendRP(RP);
	} else {
		sinked = malloc(1 * sizeof(int*));
		sinked[0] = -1;
	}
	
	while (sinked[0] < 0 && euroSpent + 2.50 <= euroAvailable){
		euroSpent += 2.50;
		sinked = buyRPHelper(euroSpent, RP);
	}

  if (sinked[0] < 0){
  	printf("sorry impossible without spending more money on RP.\n");
		return 0;
	}

	for (int i = 0; i < sink_c; i++){
		if (sinked[i]){
			printf("%-16s %5d %5d %5d\n", sink[i].name, sinked[i], sink[i].cost, sinked[i] * sink[i].cost);
			totalRpBought += 0;
		}
  }
	return 0;
}



