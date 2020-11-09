// Copyright 2020 Alexandru Necula 312CD

#include <stdio.h>
#include <stdlib.h>

#include "./bmp_header.h"

#define NAME 30

// dezaloca o matrice de tip unsigned char
void freeMatrix(unsigned char** matrix, int rows) {
  for (int i = 0; i < rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

// aloca o matrice de tip unsigned char
unsigned char** allocMatrix(int rows, int cols) {
  unsigned char** matrix;
  matrix = (unsigned char**)calloc(rows, sizeof(unsigned char*));
  if (!matrix) {
    return NULL;
  }
  for (int i = 0; i < rows; i++) {
    matrix[i] = (unsigned char*)calloc(cols, sizeof(unsigned char));
    if (!matrix[i]) {
      freeMatrix(matrix, i);
      return NULL;
    }
  }
  return matrix;
}

// dezaloca o matrice de tip int
void freeMatrix_int(int** matrix, int rows) {
  for (int i = 0; i < rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

// aloca o matrice de tip int
int** allocMatrix_int(int rows, int cols) {
  int** matrix = (int**)calloc(rows, sizeof(int*));
  if (!matrix) {
    return NULL;
  }
  for (int i = 0; i < rows; i++) {
    matrix[i] = (int*)calloc(cols, sizeof(int));
    if (!matrix[i]) {
      freeMatrix_int(matrix, i);
      return NULL;
    }
  }
  return matrix;
}

// citeste imaginea din fisier, conform formatului BMP
unsigned char** readImage(char* inputFileName, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader) {
  FILE* inputBMP = fopen(inputFileName, "rb");
  if (!inputBMP) {
    return NULL;
  }
  fread(fileHeader, sizeof(bmp_fileheader), 1, inputBMP);
  fread(infoHeader, sizeof(bmp_infoheader), 1, inputBMP);
  fseek(inputBMP, fileHeader->imageDataOffset, SEEK_SET);
  int pixelBytesPerRow = (infoHeader->width) * 3;
  int paddingBytesPerRow = (4 - (pixelBytesPerRow % 4)) % 4;
  unsigned char** image = allocMatrix(infoHeader->height, pixelBytesPerRow);
  if (!image) {
    fclose(inputBMP);
    return NULL;
  }
  for (int i = 0; i < infoHeader->height; i++) {
    for (int j = 0; j < pixelBytesPerRow; j++) {
      fread(&image[i][j], 1, 1, inputBMP);
    }
    fseek(inputBMP, paddingBytesPerRow, SEEK_CUR);
  }
  fclose(inputBMP);
  return image;
}

// scrie imaginea in fisier, conform formatului BMP
void writeImage(unsigned char** image, char* outputFileName,
  bmp_infoheader infoHeader, bmp_fileheader fileHeader) {
  FILE* outputBMP = fopen(outputFileName, "wb");
  if (!outputBMP) {
    return;
  }
  unsigned char zero = 0;
  int pixelBytesPerRow = (infoHeader.width) * 3;
  int paddingBytesPerRow = (4 - (pixelBytesPerRow % 4)) % 4;
  fwrite(&fileHeader, sizeof(bmp_fileheader), 1, outputBMP);
  fwrite(&infoHeader, sizeof(bmp_infoheader), 1, outputBMP);
  fseek(outputBMP, fileHeader.imageDataOffset, SEEK_SET);
  for (int i = 0; i < infoHeader.height; i++) {
    for (int j = 0; j < pixelBytesPerRow; j++) {
      fwrite(&image[i][j], 1, 1, outputBMP);
    }
    for (int j = 0; j < paddingBytesPerRow; j++) {
      fwrite(&zero, 1, 1, outputBMP);
    }
  }
  fclose(outputBMP);
}

void Task1(unsigned char** image, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader, char* inputFileName) {
  unsigned char s;
  char* outputFileName = (char*)calloc(NAME, sizeof(char));
  if (!outputFileName) {
    return;
  }
  unsigned char** bw = allocMatrix(infoHeader->height, (infoHeader->width) * 3);
  if (!bw) {
    free(outputFileName);
    return;
  }
  for (int i = 0; i < infoHeader->height; i++) {
    for (int j = 0; j < infoHeader->width * 3; j += 3) {
      s = (image[i][j] + image[i][j + 1] + image[i][j + 2]) / 3;
      bw[i][j] = s;
      bw[i][j + 1] = s;
      bw[i][j + 2] = s;
    }
  }
  sprintf(outputFileName, "%s_black_white.bmp", inputFileName);
  writeImage(bw, outputFileName, *infoHeader, *fileHeader);
  free(outputFileName);
  freeMatrix(bw, infoHeader->height);
}

void fillRows(unsigned char** nocrop, unsigned char** image,
  int up, int down, int max) {
  for (int i = 0; i < up; i++) {
    for (int j = 0; j < max * 3; j++) {
      nocrop[i][j] = 255;
    }
  }
  for (int i = up; i < max - down; i++) {
    for (int j = 0; j < max * 3; j++) {
      nocrop[i][j] = image[i - up][j];
    }
  }
  for (int i = max - down; i < max; i++) {
    for (int j = 0; j < max * 3; j++) {
      nocrop[i][j] = 255;
    }
  }
}

void fillCols(unsigned char** nocrop, unsigned char** image,
  int left, int right, int max) {
  for (int j = 0; j < left * 3; j++) {
    for (int i = 0; i < max; i++) {
      nocrop[i][j] = 255;
    }
  }
  for (int j = left * 3; j < (max - right) * 3; j++) {
    for (int i = 0; i < max; i++) {
      nocrop[i][j] = image[i][j - left * 3];
    }
  }
  for (int j = (max - right) * 3; j < max * 3; j++) {
    for (int i = 0; i < max; i++) {
      nocrop[i][j] = 255;
    }
  }
}

/** fill retine zona care trebuie completata cu alb (stanga-dreapta/sus-jos)
 *  up/down/left/right retin numarul de linii/coloane care trebuie completate
 */
void Task2(unsigned char** image, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader, char* inputFileName) {
  char* outputFileName = (char*)calloc(NAME, sizeof(char));
  if (!outputFileName) {
    return;
  }
  bmp_infoheader infoHeader_new = *infoHeader;
  unsigned char** nocrop;
  int fill, max, up, down, left, right;
  if (infoHeader->height <= infoHeader->width) {
    fill = infoHeader->height;
    max = infoHeader->width;
    infoHeader_new.height = max;
    nocrop = allocMatrix(max, max * 3);
    if (!nocrop) {
      free(outputFileName);
      return;
    }
    down = (max - fill) / 2;
    if ((max - fill) % 2 == 0) {
      up = down;
    } else {
      up = down + 1;
    }
    fillRows(nocrop, image, up, down, max);
  } else {
    fill = infoHeader->width;
    max = infoHeader->height;
    infoHeader_new.width = max;
    nocrop = allocMatrix(max, max * 3);
    if (!nocrop) {
      free(outputFileName);
      return;
    }
    left = (max - fill) / 2;
    if ((max - fill) % 2 == 0) {
      right = left;
    } else {
      right = left + 1;
    }
    fillCols(nocrop, image, left, right, max);
  }
  sprintf(outputFileName, "%s_nocrop.bmp", inputFileName);
  writeImage(nocrop, outputFileName, infoHeader_new, *fileHeader);
  free(outputFileName);
  freeMatrix(nocrop, max);
}

void swap(unsigned char* a, unsigned char* b) {
  unsigned char temp = *a;
  *a = *b;
  *b = temp;
}

void flipMatrix(unsigned char** matrix, int rows, int cols) {
  for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows / 2; i++) {
      swap(&matrix[i][j], &matrix[rows - i - 1][j]);
    }
  }
}

void applyFilter(unsigned char** image, unsigned char** editedImage,
  int** filter, int filterSize, int width, int height) {
  int s;
  for (int ipixel = 0; ipixel < height; ipixel++) {
    for (int jpixel = 0; jpixel < width * 3; jpixel++) {
      s = 0;
      for (int i = -(filterSize / 2); i <= filterSize / 2; i++) {
        for (int j = -(filterSize / 2); j <= filterSize / 2; j++) {
          // verificam daca vecinii se afla in afara imaginii
          if (((ipixel + i) >= 0) && ((jpixel / 3 + j) >= 0)
            && ((ipixel + i) < height) && (((jpixel / 3 + j) < width))) {
            s += image[ipixel + i][jpixel + j * 3] *
              filter[i + filterSize / 2][j + filterSize / 2];
          }
        }
      }
      if (s < 0) {
        editedImage[ipixel][jpixel] = 0;
      } else if (s > 255) {
        editedImage[ipixel][jpixel] = 255;
      } else {
        editedImage[ipixel][jpixel] = s;
      }
    }
  }
}

// creeaza o noua matrice cu liniile inversate
unsigned char** createFlippedImage(unsigned char** image,
  bmp_infoheader* infoHeader) {
  unsigned char** flippedImage;
  flippedImage = allocMatrix(infoHeader->height, (infoHeader->width) * 3);
  if (!flippedImage) {
    return NULL;
  }
  for (int i = 0; i < infoHeader->height; i++) {
    for (int j = 0; j < (infoHeader->width) * 3; j++) {
      flippedImage[i][j] = image[(infoHeader->height) - i - 1][j];
    }
  }
  return flippedImage;
}

void Task3(unsigned char** image, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader, char* inputFileName, char* filterPath) {
  int filterSize;
  FILE* filterFile = fopen(filterPath, "r");
  if (!filterFile) {
    return;
  }
  char* outputFileName = (char*)calloc(NAME, sizeof(char));
  if (!outputFileName) {
    fclose(filterFile);
    return;
  }
  fscanf(filterFile, "%d", &filterSize);
  int** filter = allocMatrix_int(filterSize, filterSize);
  if (!filter) {
    fclose(filterFile);
    free(outputFileName);
    return;
  }
  unsigned char** editedImage;
  editedImage = allocMatrix(infoHeader->height, (infoHeader->width) * 3);
  if (!editedImage) {
    freeMatrix_int(filter, filterSize);
    fclose(filterFile);
    free(outputFileName);
    return;
  }
  for (int i = 0; i < filterSize; i++) {
    for (int j = 0; j < filterSize; j++) {
      fscanf(filterFile, "%d", &filter[i][j]);
    }
  }
  applyFilter(image, editedImage, filter, filterSize,
    infoHeader->width, infoHeader->height);
  flipMatrix(editedImage, infoHeader->height, (infoHeader->width) * 3);
  sprintf(outputFileName, "%s_filter.bmp", inputFileName);
  writeImage(editedImage, outputFileName, *infoHeader, *fileHeader);
  free(outputFileName);
  freeMatrix(editedImage, infoHeader->height);
  freeMatrix_int(filter, filterSize);
  fclose(filterFile);
}

unsigned char findMin(unsigned char pixel1, unsigned char pixel2) {
  if (pixel1 < pixel2) {
    return pixel1;
  } else {
    return pixel2;
  }
}

unsigned char findMax(unsigned char pixel1, unsigned char pixel2) {
  if (pixel1 > pixel2) {
    return pixel1;
  } else {
    return pixel2;
  }
}

void applyPooling(unsigned char** image, unsigned char** editedImage,
  int filterSize, int width, int height, char type) {
  unsigned char newPixel;
  for (int ipixel = 0; ipixel < height; ipixel++) {
    for (int jpixel = 0; jpixel < width * 3; jpixel++) {
      newPixel = image[ipixel][jpixel];
      for (int i = -(filterSize / 2); i <= filterSize / 2; i++) {
        for (int j = -(filterSize / 2); j <= filterSize / 2; j++) {
          // verificam daca vecinii se afla in afara imaginii
          if (((ipixel + i) < 0) || ((jpixel / 3 + j) < 0)
            || ((ipixel + i) >= height) || (((jpixel / 3 + j) >= width))) {
            if (type == 'm') {
              newPixel = findMin(newPixel, 0);
            } else if (type == 'M') {
              newPixel = findMax(newPixel, 0);
            }
          } else {
            if (type == 'm') {
              newPixel = findMin(newPixel, image[ipixel + i][jpixel + j * 3]);
            } else if (type == 'M') {
              newPixel = findMax(newPixel, image[ipixel + i][jpixel + j * 3]);
            }
          }
        }
      }
      editedImage[ipixel][jpixel] = newPixel;
    }
  }
}

void Task4(unsigned char** image, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader, char* inputFileName, char* poolingPath) {
  int filterSize;
  char type;
  FILE* poolingFile = fopen(poolingPath, "r");
  if (!poolingFile) {
    return;
  }
  char* outputFileName = (char*)calloc(NAME, sizeof(char));
  if (!outputFileName) {
    fclose(poolingFile);
    return;
  }
  unsigned char** editedImage;
  editedImage = allocMatrix(infoHeader->height, (infoHeader->width) * 3);
  if (!editedImage) {
    fclose(poolingFile);
    free(outputFileName);
    return;
  }
  fscanf(poolingFile, "%c %d", &type, &filterSize);
  applyPooling(image, editedImage, filterSize,
    infoHeader->width, infoHeader->height, type);
  flipMatrix(editedImage, infoHeader->height, (infoHeader->width) * 3);
  sprintf(outputFileName, "%s_pooling.bmp", inputFileName);
  writeImage(editedImage, outputFileName, *infoHeader, *fileHeader);
  free(outputFileName);
  freeMatrix(editedImage, infoHeader->height);
  fclose(poolingFile);
}

int check(unsigned char** image, int r1, int c1,
  int r2, int c2, int threshold) {
  if ((abs(image[r1][c1] - image[r2][c2]) +
    abs(image[r1][c1 + 1] - image[r2][c2 + 1]) +
    abs(image[r1][c1 + 2] - image[r2][c2 + 2])) <= threshold) {
    return 1;
  } else {
    return 0;
  }
}

/** determina zonele conform cerintei si le salveaza in matricea zone
 *  rowUp/colLeft determina daca algoritmul trebuie
 * sa revina la o linie/coloana anterioara
 */
int createZone(unsigned char** image, int** zone, int width, int height,
  int threshold) {
  int zoneNo = 1, rowUp, colLeft;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (zone[i][j] == 0) {
        zone[i][j] = zoneNo;
        for (int r = 0; r < height; r++) {
          for (int c = 0; c < width; c++) {
            rowUp = 0;
            colLeft = 0;
            if (zone[r][c] == zoneNo) {
              // verificam pixelul de sus
              if ((r - 1 >= 0) &&
                (check(image, i, j * 3, r - 1, c * 3, threshold) == 1) &&
                (zone[r - 1][c] == 0)) {
                zone[r - 1][c] = zoneNo;
                rowUp = 1;
              }
              // verificam pixelul de jos
              if ((r + 1 < height) &&
                (check(image, i, j * 3, r + 1, c * 3, threshold) == 1)
                && (zone[r + 1][c] == 0)) {
                zone[r + 1][c] = zoneNo;
              }
              // verificam pixelul din stanga
              if ((c - 1 >= 0) &&
                (check(image, i, j * 3, r, (c - 1) * 3, threshold) == 1) &&
                (zone[r][c - 1] == 0)) {
                zone[r][c - 1] = zoneNo;
                colLeft = 1;
              }
              // verificam pixelul din dreapta
              if ((c + 1 < width) &&
                (check(image, i, j * 3, r, (c + 1) * 3, threshold) == 1) &&
                (zone[r][c + 1] == 0)) {
                zone[r][c + 1] = zoneNo;
              }
            }
            if ((rowUp == 1) || (colLeft == 1)) {
              switch (r) {
              case 0:
                break;
              case 1:
                r--;
                break;
              default:
                r -= 2;
              }
              switch (c) {
              case 0:
                break;
              case 1:
                c--;
                break;
              default:
                c -= 2;
              }
            }
          }
        }
        zoneNo++;
      }
    }
  }
  return zoneNo;
}

/** newPixelSum este o matrice care retine suma fiecarui pixel din zona
 *  in ordinea BGR (coloanele 0,1,2)
 *  numarul pixelilor din zona este retinut in coloana 3
 * numarul liniei reprezinta indicele zonei
 */
void replacePixels(unsigned char** image, unsigned char** editedImage,
  int** zone, int** newPixelSum, int width, int height, int zoneNo) {
  int average;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      for (int k = 0; k < 3; k++) {
        newPixelSum[zone[i][j]][k] += image[i][j * 3 + k];
      }
      newPixelSum[zone[i][j]][3]++;
    }
  }
  for (int k = 1; k <= zoneNo; k++) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (zone[i][j] == k) {
          for (int x = 0; x < 3; x++) {
            average = newPixelSum[k][x] / newPixelSum[k][3];
            editedImage[i][j * 3 + x] = (unsigned char)average;
          }
        }
      }
    }
  }
}

void cluster(unsigned char** image, unsigned char** editedImage,
  int width, int height, int threshold) {
  int** zone = allocMatrix_int(height, width);
  if (!zone) {
    return;
  }
  int zoneNo;
  zoneNo = createZone(image, zone, width, height, threshold);
  int** newPixelSum = allocMatrix_int(zoneNo + 1, 4);
  if (!newPixelSum) {
    freeMatrix_int(zone, height);
    return;
  }
  replacePixels(image, editedImage, zone, newPixelSum, width, height, zoneNo);
  freeMatrix_int(newPixelSum, zoneNo + 1);
  freeMatrix_int(zone, height);
}

void Task5(unsigned char** image, bmp_infoheader* infoHeader,
  bmp_fileheader* fileHeader, char* inputFileName, char* clusterPath) {
  int threshold;
  FILE* clusterFile = fopen(clusterPath, "r");
  if (!clusterFile) {
    return;
  }
  char* outputFileName = (char*)calloc(NAME, sizeof(char));
  if (!outputFileName) {
    fclose(clusterFile);
    return;
  }
  unsigned char** editedImage;
  editedImage = allocMatrix(infoHeader->height, (infoHeader->width) * 3);
  if (!editedImage) {
    fclose(clusterFile);
    free(outputFileName);
    return;
  }
  fscanf(clusterFile, "%d", &threshold);
  cluster(image, editedImage, infoHeader->width, infoHeader->height, threshold);
  flipMatrix(editedImage, infoHeader->height, (infoHeader->width) * 3);
  sprintf(outputFileName, "%s_clustered.bmp", inputFileName);
  writeImage(editedImage, outputFileName, *infoHeader, *fileHeader);
  free(outputFileName);
  freeMatrix(editedImage, infoHeader->height);
  fclose(clusterFile);
}

int main() {
  FILE* inputFile = fopen("input.txt", "r");
  if (!inputFile) {
    return -1;
  }
  char* inputFileName = (char*)calloc(NAME, sizeof(char));
  if (!inputFileName) {
    fclose(inputFile);
    return -1;
  }
  char* path = (char*)calloc(NAME, sizeof(char));
  if (!path) {
    fclose(inputFile);
    free(inputFileName);
    return -1;
  }
  bmp_infoheader infoHeader;
  bmp_fileheader fileHeader;
  unsigned char** image, ** flippedImage;
  fscanf(inputFile, "%s", inputFileName);
  image = readImage(inputFileName, &infoHeader, &fileHeader);
  flippedImage = createFlippedImage(image, &infoHeader);
  inputFileName[5] = '\0'; // sterge extensia
  Task1(image, &infoHeader, &fileHeader, inputFileName);
  Task2(image, &infoHeader, &fileHeader, inputFileName);
  fscanf(inputFile, "%s", path);
  Task3(flippedImage, &infoHeader, &fileHeader, inputFileName, path);
  fscanf(inputFile, "%s", path);
  Task4(flippedImage, &infoHeader, &fileHeader, inputFileName, path);
  fscanf(inputFile, "%s", path);
  Task5(flippedImage, &infoHeader, &fileHeader, inputFileName, path);
  free(inputFileName);
  free(path);
  freeMatrix(image, infoHeader.height);
  freeMatrix(flippedImage, infoHeader.height);
  fclose(inputFile);
  return 0;
}
