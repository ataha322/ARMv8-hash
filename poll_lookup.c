/*
 * CSE30 WI22 HW6
 * CSE30 username: cs30wi22gx (TODO: Fill in)
 */

#include "poll_lookup.h"

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * main
 *
 * Arguments: argc, argv
 *
 * Operation: Main driver for the program, calls other funttions to:
 *            parse the options, allocate the hash table, load the table, print
 *out the table stats
 *            and make print population stats of the desired city/state
 * Returns:   EXIT_SUCCESS if all ok, EXIT_FAILURE otherwise
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
int main(int argc, char *argv[]) {
  node **table;
  unsigned long size = TABLE_SIZE;
  // name of csv file
  char *filename;
  int info = 0;

  // Indicates days we want stats for/to remove
  char *date = NULL;
  char *del_date = NULL;

  // Parse options
  if (!parse_opts(argc, argv, &filename, &size, &info, &date, &del_date)) {
    return EXIT_FAILURE;
  }

  // Allocate space for table
  if ((table = calloc(size, sizeof(node *))) == NULL) {
    fprintf(stderr, "%s: Unable to allocate space for hash table\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Load records from file
  if (load_table(table, size, filename)) {
    return EXIT_FAILURE;
  }

  // Delete data first
  if (del_date) {
    char *stripped_date = strip_date(del_date);
    if (stripped_date) { // no malloc fail
      delete_date(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Produce data for a single date
  if (date) {
    char *stripped_date = strip_date(date);
    if (stripped_date) { // no malloc fail
      print_date_stats(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Print metadata
  if (info)
    print_info(table, size);

  // Clean up
  delete_table(table, size);

  return EXIT_SUCCESS;
}

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * hash
 *
 * Arguments: a null terminated string
 *
 * Operation: calculates a hash value for the string
 *
 * returns:   the hash value
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
unsigned long hash(char *str) {
  unsigned long hash = 0;
  unsigned int c;
#ifdef C_HASH
  while ((c = (unsigned char)*str++) != '\0') {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
#else
  while ((c = (unsigned char)*str++) != '\0') {
    hash = hashFun((unsigned long)c, hash);
  }
#endif
  return hash;
}

// helper function to compare dates
int nodeCompare(node* current, const int year, const int month, const int day, const int hour) {
  if (current->year == year &&
      current->month == month &&
      current->day == day &&
      current->hour == hour)
      return 1;
  else
      return 0;
}
/*
 * node_lookup
 * Searches for the first node with this year, month, day, and hour
 * in this chain, and returns a pointer to it.
 *
 * Arguments: linked list chain pointer head, year, month, day, hour
 */
node *node_lookup(node *front, int year, int month, int day, int hour) {
  // TODO: Implement node_lookup
  if(front == NULL) {
    return NULL;
  }
  node* tmp = front;
  while (tmp != NULL) {
    if (nodeCompare(tmp, year, month, day, hour))
      return tmp;
    else
      tmp = tmp->next;
  }
  return NULL;
}

/*
 * add_node
 * Adds a node with the provided data to the end of the given linked list.
 *
 * Arguments: linked list pointer head, year, month, day, hour, pm25, temp, iws
 * Return:  NULL    if there was an error
 *          the head of the chain otherwise
 */
node *add_node(node *front, int year, int month, int day, int hour, int pm25,
               int temp, float iws) {
  // TODO: Implement add_node
  node* newNode = (node*) malloc(sizeof(node));
  if (!newNode)
    return NULL;
  newNode->year = year;
  newNode->month = month;
  newNode->day = day;
  newNode->hour = hour;
  newNode->pm25 = pm25;
  newNode->temp = temp;
  newNode->iws = iws;
  newNode->next = NULL;

  if (front == NULL) {
    front = newNode;
    return front;
  }

  node* end = front;
  while(end->next != NULL) {
    end = end->next;
  }
  end->next = newNode;
  return front;
}

int compareDates(int* dates, int year, int month, int day) {
  if (dates[0] == year &&
      dates[1] == month &&
      dates[2] == day) {
        return 1;
      } else {
        return 0;
      }
}

/*
 * print_date_stats
 * Print the average stats for this date
 *
 * Arguments: pointer to hash table, hash table size, date as a string in the
 *form YYYY-MM-DD
 */
void print_date_stats(node **table, unsigned long size, char *datestr) {
  // TODO: Implement print_data_stats
  char datecopy[MAX_SIZE_DATESTR];
  strcpy(datecopy, datestr);
  unsigned long index = hash(datestr);
  index = index % size;
  node* current = table[index];
  int dates[3];
  const char split[] = "-";
  char *token = strtok(datestr, split);
  // Year, month, day
  int c = 0;
  while (token != NULL) {
    if (c >= 3) {
      fprintf(stderr, "Incorrect date format.\n");
      break;
    }
    dates[c] = atoi(token);
    token = strtok(NULL, split);
    c++;
  }


  int minPm = INT_MAX;
  int maxPm = INT_MIN;
  int sumPm = 0;
  int minTemp = INT_MAX;
  int maxTemp = INT_MIN;
  int sumTemp = 0;
  float minIWS = FLT_MAX;
  float maxIWS = FLT_MIN;
  float sumIWS = 0;

  

  int countNodes = 0;
  while (current != NULL) {
    if (compareDates(dates, current->year, current->month, current->day)) {
      countNodes++;
      sumPm += current->pm25;
      sumTemp += current->temp;
      sumIWS += current->iws;

      if (current->pm25 < minPm)
        minPm = current->pm25;
      if (current->pm25 > maxPm)
        maxPm = current->pm25;
      if (current->temp < minTemp)
        minTemp = current->temp;
      if (current->temp > maxTemp)
        maxTemp = current->temp;
      if (current->iws < minIWS)
        minIWS = current->iws;
      if (current->iws > maxIWS)
        maxIWS = current->iws;
    }
    current = current->next;

  }

  if (countNodes == 0) {
    printf("Unable to find any data for the date %s.\n", datecopy);
    return;
  }

  int pmAvg = sumPm / countNodes;
  int tmpAvg = sumTemp / countNodes;
  float iwsAvg = sumIWS / countNodes;
  
  
  // Use the following formatting strings to print messages.
  
  printf("Minimum pm2.5: %d\tMaximum pm2.5: %d\tAverage pm2.5: %d\n",
         minPm, maxPm, pmAvg);
  printf("Minimum temp: %d\tMaximum temp: %d\tAverage temp: %d\n",
         minTemp, maxTemp, tmpAvg);
  printf("Minimum iws: %f\tMaximum iws: %f\tAverage iws: %f\n",
         minIWS, maxIWS, iwsAvg);
}

/*
 * load_table
 * Allocate memory for the hash table of node*s
 *
 * Arguments: pointer to hash table, hash table size, file name
 */
int load_table(node **table, unsigned long size, char *filename) {
  // TODO: Implement load_table
  int year, month, day, hour, pm25, TEMP;
  float iws;
  const char* delim = ",";
  char line[LINE_SIZE]; 
  
  char readbuf[7][8];

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("load_table filename open");
    return 1;
  }

  char* token;
  int c = 0;
  while(fgets(line, LINE_SIZE, fp)) {
    token = strtok(line, delim);
    c = 0;
    while (token != NULL) {
      if (c > 6) {
        perror("line read error");
        return 1;
      }
      if(strcmp(token, "NA") == 0)
        strcpy(readbuf[c], "0");
      else 
        strcpy(readbuf[c], token);
      token = strtok(NULL, delim);
      c++;
    }
    year = atoi(readbuf[COL_YEAR]);
    month = atoi(readbuf[COL_MONTH]);
    day = atoi(readbuf[COL_DAY]);
    hour = atoi(readbuf[COL_HOUR]);
    pm25 = atoi(readbuf[COL_PM25]);
    TEMP = atoi(readbuf[COL_TEMP]);
    iws = atof(readbuf[COL_IWS]);

    char date[MAX_SIZE_DATESTR];
    snprintf(date, MAX_SIZE_DATESTR, "%d-%d-%d", year, month, day);
    unsigned long hashVal = hash(date);
    unsigned long hashIndex = hashVal % size;
    node* tmp = node_lookup(table[hashIndex], year, month, day, hour);
    if (tmp != NULL) {
      fprintf(stderr, "load_table duplicate entry: %d-%d-%d %d\n", year, month, day, hour);
      continue;
    } else {
      node* newNode = add_node(table[hashIndex], year, month, day, hour, pm25, TEMP, iws);
      table[hashIndex] = newNode;
      if (!newNode) {
        fprintf(stderr, "load_table clould not add %s\n", line);
      }
    }
  }


  fclose(fp);
  return 0;
}

/*
 * delete_date
 * Delete all nodes associated with a given date of form YYYY-MM-DD
 * All leading zeros have already been removed in the date string,
 * so you do not have to worry about 2010-01-01 versus 2010-1-1.
 */
void delete_date(node **table, unsigned long size, char *datestr) {
  // TODO: Implement delete_date
  unsigned long hashVal = hash(datestr);
  hashVal = hashVal % size;
  node* tmp1 = table[hashVal];
  if (tmp1 == NULL)
    return;
  node* tmp2 = tmp1->next;

  int dates[3];
  const char split[] = "-"; 
  char *token = strtok(datestr, split);
  // Year, month, day
  int c = 0;
  while (token != NULL) {
    if (c >= 3) {
      fprintf(stderr, "Incorrect date format.\n");
      break;
    }
    dates[c] = atoi(token);
    token = strtok(NULL, split);
    c++;
  }

  while (compareDates(dates, tmp1->year, tmp1->month, tmp1->day)) {
    table[hashVal] = tmp2;
    free(tmp1);
    tmp1 = tmp2;
    if(tmp2 != NULL)
      tmp2 = tmp2->next;
    else
      break;
  }
  
  while (tmp2 != NULL) {
    if (compareDates(dates, tmp2->year, tmp2->month, tmp2->day)) {
      tmp1->next = tmp2->next;
      free(tmp2);
      tmp2 = tmp1->next;
    } else {
      tmp1 = tmp2;
      tmp2 = tmp2->next;
    }
  }

  
}

/*
 * print_info
 *
 * Arguments: pointer to a hash table, number of elements
 */
void print_info(node **table, unsigned long size) {
  // TODO: Implement print_info
  unsigned long totalNodes = 0;
  unsigned long longestChain = 0;
  unsigned long shortestChain = INT_MAX;
  unsigned long empty = 0;
  unsigned long currentChain = 0;
  node* tmp = NULL;
  
  for (unsigned long i = 0; i < size; i++) {
    tmp = table[i];
    if (tmp == NULL)
      empty++;
    else {
      while(tmp != NULL) {
        currentChain++;
        totalNodes++;
        tmp = tmp->next;
      }
      if (currentChain > longestChain)
        longestChain = currentChain;
      if (currentChain < shortestChain)
        shortestChain = currentChain;
      currentChain = 0;
    }
  }
  printf("Table size: %lu\n", size);
  printf("Total entries: %lu\n", totalNodes);
  printf("Longest chain: %lu\n", longestChain);
  printf("Shortest chain: %lu\n", shortestChain);
  printf("Empty buckets: %lu\n", empty);
}

/*
 * delete_table
 *
 * Arguments: pointer to hash table, hash table array size
 */
void delete_table(node **table, unsigned long size) {
  // TODO: Implement delete_table
  node* tmp1 = NULL;
  node* tmp2 = NULL;

  for(unsigned long int i = 0; i < size; i++) {
    if(table[i] != NULL) {
      tmp1 = table[i];
      tmp2 = tmp1->next;
      while(tmp2 != NULL) {
        free(tmp1);
        tmp1 = tmp2;
        tmp2 = tmp2->next;
      }
      free(tmp1);
    }
  }
  free(table);
}
