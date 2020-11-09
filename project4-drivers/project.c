// Copyright 2019 Alexandru Necula 312CD

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "include/project.h"

int *arrayProduct(int *array, int arrayLength) {
  int *products = (int*)calloc(arrayLength, sizeof(int));
  if (!products) {
    return NULL;
  }
  int p;
  for (int i = 0; i < arrayLength ; i++) {
    p = 1;
    for (int j = 0; j < arrayLength ; j++) {
      if (i != j) {
        p *= array[j];
      }
    }
    products[i] = p;
  }
  return products;
}

int **rotateMatrix(int **matrix, int matrixSize) {
  int **rotatedMatrix = (int**)calloc(matrixSize, sizeof(int *));
  if (!rotatedMatrix) {
    return NULL;
  }
  for (int i = 0; i < matrixSize; i++) {
    rotatedMatrix[i] = calloc(matrixSize, sizeof(int));
    if (!rotatedMatrix[i]) {
      return NULL;
    }
  }
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize ; j++) {
      rotatedMatrix[i][j] = matrix[j][matrixSize - i - 1];
    }
  }
  return rotatedMatrix;
}

int *subMatrixesSums(int **matrix, int queriesNo, int *queries) {
  int *sums = (int*)calloc(queriesNo, sizeof(int));
  if (!sums) {
    return NULL;
  }
  int No = 0;
  for (int k = 0 ; k < queriesNo ; k++) {
    sums[k] = 0;
    for (int i = queries[No + 0]; i <= queries[No + 2]; i++) {
      for (int j = queries[No + 1]; j <= queries[No + 3] ; j++) {
        sums[k] += matrix[i][j];
      }
    }
    No += 4;
  }
  return sums;
}

TDriver *allocDriver(int ridesNo) {
  TDriver* driver = (TDriver*)calloc(1, sizeof(TDriver));
  if (!driver) {
    return NULL;
  }
  driver->rides = (TRide*)calloc(ridesNo, sizeof(TRide));
  driver->ridesNo = ridesNo;
  return driver;
}

TDriver **allocDrivers(int driversNo, int *driversRidesNo) {
  TDriver** drivers = (TDriver**)calloc(driversNo, sizeof(TDriver*));
  if (!drivers) {
    return NULL;
  }
  for (int i = 0 ; i < driversNo ; i++) {
  drivers[i] = allocDriver(driversRidesNo[i]);
  }
  return drivers;
}

TDriver** readDrivers(FILE* inputFile, int* driversNo) {
  fread(driversNo, sizeof(int), 1, inputFile);
  int* driversRidesNo = (int*)calloc(*driversNo, sizeof(int));
  if (!driversRidesNo) {
    return NULL;
  }
  fread(driversRidesNo, sizeof(int), *driversNo, inputFile);
  TDriver** drivers = allocDrivers(*driversNo, driversRidesNo);
  if (!drivers) {
    free(driversRidesNo);
    return NULL;
  }
  for (int i = 0; i < *driversNo; i++) {
    fread(drivers[i]->name, 20, 1, inputFile);
    fread(drivers[i]->regNo, 8, 1, inputFile);
    fread(&drivers[i]->lat, sizeof(double), 1, inputFile);
    fread(&drivers[i]->lon, sizeof(double), 1, inputFile);
    for (int j = 0; j < driversRidesNo[i]; j++) {
      fread(&drivers[i]->rides[j], sizeof(TRide), 1, inputFile);
    }
  }
  free(driversRidesNo);
  return drivers;
}

void printDrivers(FILE* outputFile, TDriver** drivers, int driversNo) {
  for (int i = 0; i < driversNo; i++) {
    fwrite(drivers[i]->name, 20, 1, outputFile);
    fwrite(drivers[i]->regNo, 8, 1, outputFile);
    fwrite(&drivers[i]->lat, sizeof(double), 1, outputFile);
    fwrite(&drivers[i]->lon, sizeof(double), 1, outputFile);
    for (unsigned int j = 0; j < drivers[i]->ridesNo; j++) {
      fwrite(&drivers[i]->rides[j], sizeof(TRide), 1, outputFile);
    }
  }
}

char *maxRatingDriverName(TDriver **drivers, int driversNo) {
  unsigned char state;
  int completedRidesNo, stars, driverNo;
  float* rating = (float*)calloc(driversNo, sizeof(float));
  if (!rating) {
    return NULL;
  }
  float maxRating;
  for (int i = 0; i < driversNo ; i++) {
    stars = 0;
    completedRidesNo = drivers[i]->ridesNo;
    for (unsigned int j = 0; j < drivers[i]->ridesNo; j++) {
      state = drivers[i]->rides[j].state;
      if ((CANCELED_RIDE(state)) || (ONGOING_RIDE(state))) {
        completedRidesNo--;
      } else {
        stars += GET_STARS(state);
      }
    }
    if (completedRidesNo) {
      rating[i] = (float) stars / completedRidesNo;
    } else {
      rating[i] = 0;
    }
  }
  maxRating = rating[0];
  driverNo = 0;
  for (int i = 1; i < driversNo; i++) {
    if (rating[i] > maxRating) {
      maxRating = rating[i];
      driverNo = i;
    }
  }
  free(rating);
  return drivers[driverNo]->name;
}

void swap_double(double *x, double *y) {
  double temp = *x;
  *x = *y;
  *y = temp;
}

void swap_int(int *x, int *y) {
  int temp = *x;
  *x = *y;
  *y = temp;
}

TDriver **getClosestDrivers(TDriver **drivers, int driversNo, double desiredX,
  double desiredY, int resultsNo) {
  TDriver** closeDrivers = (TDriver**) calloc(resultsNo, sizeof(TDriver*));
  if (!closeDrivers) {
    return NULL;
  }
  double* distance = (double*) calloc(driversNo, sizeof(double));
  if (!distance) {
    free(closeDrivers);
    return NULL;
  }
  int* driverNo = (int*) calloc(driversNo, sizeof(int));
  if (!driverNo) {
    free(closeDrivers);
    free(distance);
    return NULL;
  }
  for (int i = 0; i < driversNo; i++) {
    distance[i] = sqrt(pow(drivers[i]->lat - desiredX, 2)
          + pow(drivers[i]->lon - desiredY, 2));
    driverNo[i] = i;
  }
  for (int i = 0; i < driversNo-1; i++)  {
    for (int j = 0; j < driversNo-i-1; j++)  {
      if ((distance[j] > distance[j+1]) ||
        ((distance[j] == distance[j+1]) &&
        (strcmp(drivers[driverNo[j]]->name, drivers[driverNo[j+1]]->name)>0))) {
        swap_double(&distance[j], &distance[j+1]);
        swap_int(&driverNo[j], &driverNo[j+1]);
      }
    }
  }
  for (int i = 0; i < resultsNo; i++) {
    closeDrivers[i] = drivers[driverNo[i]];
  }
  free(distance);
  free(driverNo);
  return closeDrivers;
}

void freeDriver(TDriver *driver) {
  free(driver->rides);
  free(driver);
}

void freeDrivers(TDriver **drivers, int driversNo) {
  for (int i = 0; i < driversNo; i++) {
    freeDriver(drivers[i]);
  }
  free(drivers);
}
