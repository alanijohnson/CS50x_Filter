#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <cs50.h>

typedef struct
{
    int  rgbtBlue;
    int  rgbtGreen;
    int  rgbtRed;
}
RGBINTERMEDIATE;

void copyImage(int height, int width, RGBTRIPLE image[height][width], RGBTRIPLE copy[height][width]);
RGBTRIPLE average(RGBTRIPLE pixels[], int len);
void sumG(RGBINTERMEDIATE g[3][3], int sum[3]);
RGBTRIPLE sobelPixel(RGBTRIPLE pixels[3][3]);
RGBINTERMEDIATE multiplyG(RGBTRIPLE pixel, int coeff);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{

    BYTE rgbAvg;

    // iterate through each row of image
    for(int i = 0; i < height; i++){
        // iterate through each pixel in row
        for(int j=0; j < width; j++){

            // calculate what the greyscale picture should be.
            // pixel color is determined by setting each RGB value to the average of the
            rgbAvg = round((image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0);
            image[i][j].rgbtBlue = rgbAvg;
            image[i][j].rgbtGreen = rgbAvg;
            image[i][j].rgbtRed = rgbAvg;

        }
    }

    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // copy image as temp to save pixels.
    RGBTRIPLE copy[height][width];
    copyImage(height, width, image, copy);

    for(int i=0; i<height; i++){
        for( int j=0; j<width; j++){

            // copy over pixels
            image[i][j] = copy[i][width - 1 - j];

        }
    }


    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{

    // copy image to save pixels from original
    RGBTRIPLE copy[height][width];
    copyImage(height, width, image, copy);

    // helper variables
    RGBTRIPLE pixels[9];
    int count = 0;
    int xtemp;
    int ytemp;

    // iterate through each row of image
    for(int i = 0; i < height; i++){
        // iterate through each pixel in row
        for(int j=0; j < width; j++){

            // calculate what the blurred picture pixels should be.
            // pixel is determined to be the average of all surrounding pixels (including the pixel itself)

            // create an array of all surrounding pixels & count the number of surrounding pixels (including the pixel itself)
            for(int ymove = -1; ymove <=1; ymove++){
                for(int xmove = -1; xmove <=1; xmove++){

                    xtemp = i + xmove;
                    ytemp = j + ymove;
                    if( xtemp >=0 && xtemp < height && ytemp>=0 && ytemp < width ){
                        //if the pixel is in the limits calculate the
                        pixels[count] = copy[xtemp][ytemp];
                        count++;
                    }
                }
            }

            // update pixel color; reset count
            image[i][j] = average(pixels,count);
            count = 0;

        }
    }

    return;
}

// Detect edges
// add function to provide a pixel and then calculate the pixel.
void edges(int height, int width, RGBTRIPLE image[height][width])
{

    // copy image to save pixels from original image
    RGBTRIPLE copy[height][width];
    copyImage(height, width, image, copy);

    // helper variable
    RGBTRIPLE pixels[3][3];
    RGBTRIPLE black;
    black.rgbtBlue = 0;
    black.rgbtGreen = 0;
    black.rgbtRed = 0;
    int xtemp;
    int ytemp;


    // iterate through each row of image
    for(int i = 0; i < height; i++){
        // iterate through each pixel in row
        for(int j=0; j < width; j++){

            // calculate what the edge picture pixels should be.

            // pixel is determined to be an edge if there is a great change in color moving left to right (gx) or top to bottom (gy)

            // multipy 3x3 pixel matrix by gx and gy
            // combine gx & gy by taking squared sum
            // round integer to the nearest int & cap at 255.

            // for each pixel, create a matrix of the surrounding pixels
            // missing pixels should be treated as black pixels. rgb values = 0x0.
            for(int y=0; y<3; y++){
                for(int x=0; x<3; x++){

                    ytemp = i + y - 1;
                    xtemp = j + x - 1;

                    if( xtemp >=0 && xtemp < width && ytemp>=0 && ytemp < height ){
                        // if the pixel is in the limits calculate the pixel
                        pixels[y][x] = copy[ytemp][xtemp];
                    } else {
                        pixels[y][x] = black;
                    }

                }
            }


            // determine the pixel coloration.
            image[i][j] = sobelPixel(pixels);

        }
    }


    return;
}

// Copy fresh image
void copyImage(int height, int width, RGBTRIPLE image[height][width], RGBTRIPLE copy[height][width]){

    // iterate through rows of pixels
    for (int i = 0; i < height; i++){
        // iterate through each pixel in each row
        for(int j = 0; j < width; j++ ){
            copy[i][j] = image[i][j];
        }

    }
    return;
}

// average an array of pixels
RGBTRIPLE average(RGBTRIPLE pixels[], int len){

    WORD sumGreen = 0x0;
    WORD sumRed = 0x0;
    WORD sumBlue = 0x0;
    RGBTRIPLE newPixel;

    for(int i=0; i<len; i++){

        sumGreen += pixels[i].rgbtGreen;
        sumBlue += pixels[i].rgbtBlue;
        sumRed += pixels[i].rgbtRed;

    }

    newPixel.rgbtBlue = round(sumBlue / (float)len);
    newPixel.rgbtGreen = round(sumGreen / (float)len);
    newPixel.rgbtRed = round(sumRed / (float)len);

    return newPixel;


}

RGBTRIPLE sobelPixel(RGBTRIPLE pixels[3][3]){


    // define g multipliers
    int gx[3][3] = {
        { -1, 0, 1},
        { -2, 0, 2},
        { -1, 0, 1}
    };

    int gy[3][3] = {
        { -1, -2, -1},
        { 0, 0, 0},
        { 1, 2, 1}
    };

    RGBINTERMEDIATE tempgx[3][3];
    RGBINTERMEDIATE tempgy[3][3];

    // calculate the gx gy matrix for each color
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){

            tempgx[i][j] = multiplyG(pixels[i][j], gx[i][j]);
            tempgy[i][j] = multiplyG(pixels[i][j], gy[i][j]);

        }
    }

    // sum gx and gy matrix
    int sumx[3];
    int sumy[3];
    sumG(tempgx,sumx);
    sumG(tempgy,sumy);
    int sum[3];

    // iterate through RGB and calculate the final pixel
    for(int i=0; i<3; i++){
        sum[i] = round(sqrt(pow(sumx[i],2) + pow(sumy[i],2)));
        if(sum[i]>255){
            sum[i] = 255;
        }

    }

    RGBTRIPLE result;
    result.rgbtRed = sum[0];
    result.rgbtGreen = sum[1];
    result.rgbtBlue = sum[2];


    return result;

}


RGBINTERMEDIATE multiplyG(RGBTRIPLE pixel, int coeff){
    RGBINTERMEDIATE RGBValues;
    RGBValues.rgbtBlue = pixel.rgbtBlue * coeff;
    RGBValues.rgbtGreen = pixel.rgbtGreen * coeff;
    RGBValues.rgbtRed = pixel.rgbtRed * coeff;

    return RGBValues;
}




void sumG(RGBINTERMEDIATE g[3][3], int sum[3]){

    int sumRed = 0;
    int sumGreen = 0;
    int sumBlue = 0;

    for(int i=0; i<3; i++){
        for( int j=0; j<3; j++){
            sumRed += g[i][j].rgbtRed;
            sumGreen += g[i][j].rgbtGreen;
            sumBlue += g[i][j].rgbtBlue;
        }
    }

    sum[0] = sumRed;
    sum[1] = sumGreen;
    sum[2] = sumBlue;

}



