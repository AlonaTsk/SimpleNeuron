
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>





#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CMAG  "\x1B[35m"
#define CCYN  "\x1B[36m"
#define CWHT  "\x1B[37m"


double sigmoid(double x) {


        double result;


        result = 1 / (1 + exp(-x));


        return result;
}




double sigmoid_derivative(double x) {
        double result;


        /* result = sigmoid(x) * (1 - sigmoid(x)); */
        result = x * (1 - x);


        return result;
}




double dot(double *v, double *u, uint32_t n) {


        double result = 0.0;
        for (uint32_t i = 0; i < n; i++) {
                result += v[i] * u[i];
        }


        return result;
}




static double synaptic_weights[3];


double analyze(double input[]) {
        double result = dot(input, synaptic_weights, 3);
        result = sigmoid(result);
        return result;
}


void think_train(double input[][3], double output[]) {
        double result1, result2, result3, result4;


        result1 = dot(&input[0][0], synaptic_weights, 3);
        result2 = dot(&input[1][0], synaptic_weights, 3);
        result3 = dot(&input[2][0], synaptic_weights, 3);
        result4 = dot(&input[3][0], synaptic_weights, 3);


        output[0] = sigmoid(result1);
        output[1] = sigmoid(result2);
        output[2] = sigmoid(result3);
        output[3] = sigmoid(result4);
}


void train(double training_set_inputs[][3], double training_set_outputs[], uint32_t n) {


        for (uint32_t i = 0; i < n; i++) {


                double error[4];
                double output[4];


                think_train(training_set_inputs, output);


                error[0] = training_set_outputs[0] - output[0];
                error[1] = training_set_outputs[1] - output[1];
                error[2] = training_set_outputs[2] - output[2];
                error[3] = training_set_outputs[3] - output[3];


                error[0] *= sigmoid_derivative(output[0]);
                error[1] *= sigmoid_derivative(output[1]);
                error[2] *= sigmoid_derivative(output[2]);
                error[3] *= sigmoid_derivative(output[3]);


                double transpose[3][4];


                transpose[0][0] = training_set_inputs[0][0];
                transpose[0][1] = training_set_inputs[1][0];
                transpose[0][2] = training_set_inputs[2][0];
                transpose[0][3] = training_set_inputs[3][0];


                transpose[1][0] = training_set_inputs[0][1];
                transpose[1][1] = training_set_inputs[1][1];
                transpose[1][2] = training_set_inputs[2][1];
                transpose[1][3] = training_set_inputs[3][1];


                transpose[2][0] = training_set_inputs[0][2];
                transpose[2][1] = training_set_inputs[1][2];
                transpose[2][2] = training_set_inputs[2][2];
                transpose[2][3] = training_set_inputs[3][2];


                double adjustments[3];


                adjustments[0] = dot(error, &transpose[0][0], 4);
                adjustments[1] = dot(error, &transpose[1][0], 4);
                adjustments[2] = dot(error, &transpose[2][0], 4);


                synaptic_weights[0] += adjustments[0];
                synaptic_weights[1] += adjustments[1];
                synaptic_weights[2] += adjustments[2];
        }


}


struct imgAttributes {
        float redAvg;
        float greenAvg;
        float blueAvg;
        float redAvg1[4];
        float greenAvg1[4];
        float blueAvg1[4];
        float redAvg2[16];
        float greenAvg2[16];
        float blueAvg2[16];
};


void createTmpBinaryFile(const char * fileName1, const char * tmpFileName) {
        FILE* inputFile;
        FILE* tmpFile;
        char c;
        int width;
        int height;
        int b;
        unsigned char c1[1];
        if ((inputFile = fopen(fileName1, "rt")) != NULL) {
                tmpFile = fopen(tmpFileName, "wb");
                fscanf(inputFile, "%c%c", &c, &c); // skip P6
                fscanf(inputFile, "%d", &width);// read width
                printf("Picture width %d\n", width);
                fscanf(inputFile, "%d", &height);
                printf("Picture height %d\n", height);
                fscanf(inputFile, "%d", &b);// skip 255
                //printf("Picture colors %d\n", b);
                fscanf(inputFile, "%c", &c);// skip end of line symbol
                long int i = 0;
                while (i < width*height * 3) {
                        fscanf(inputFile, "%c", &c1[0]);// read color
                        fwrite(c1, 1, 1, tmpFile);// write color to the temporary binary file
                        //printf("%d ", c1[0]);
                        i++;
                }


                fclose(tmpFile);
                fclose(inputFile);
        }
        else
                printf("Error open file %s", fileName1);


}


float colorAvg(FILE* file,  int i,  int j, int width, int height, int color)
{
        long int Avg = 0;
        unsigned char c[1];
        long int pos;
        for(int ii=i;ii<height; ii++)
                for( int jj=j*3+color; jj<width*3; jj+=3)
                {
                        pos = fseek(file, ii*width*3 + jj, SEEK_SET);
                        fread(c, 1, 1, file);        
                        Avg += c[0];
                }
        
        return Avg/(float)((width - j)*(height - i));
}


void calcAndSave(FILE* binFile, FILE* outputFile,  int i,  int j, int width, int height) {
        float red = colorAvg(binFile, i, j, width, height, 0);
        float green = colorAvg(binFile, i, j, width, height, 1);
        float blue = colorAvg(binFile, i, j, width, height, 2);


        


        fprintf(outputFile, "%f  ", red);
        fprintf(outputFile, "%f  ", green);
        fprintf(outputFile, "%f\n", blue);
        printf("red - %f  ", red);
        printf("green - %f  ", green);
        printf("blue - %f\n", blue);
}


void createOutputFile(const char * outputFileName, const char * binFileName) {
        FILE* binFile = fopen(binFileName, "rb");
        FILE* outputFile = fopen(outputFileName, "w");
        printf("All image\n");
        //all image
        calcAndSave(binFile, outputFile, 0, 0, 300, 400);
        //regions
        printf("Regions 150x200 pixels\n");
        for(int i=0; i<=200; i+=200)
                for(int j=0; j<=150; j+=150)
                        calcAndSave(binFile, outputFile, i, j, j+150, i+200);
        printf("Regions 75x100 pixels\n");


        for (int i = 0; i <= 300; i += 100)
                for (int j = 0; j <= 225; j += 75)
                        calcAndSave(binFile, outputFile, i, j, j+75, i+100);


        fclose(outputFile);
        fclose(binFile);
}


void loadStruct(const char * fileName, struct imgAttributes *img){
        FILE* f = fopen(fileName, "r");
        fscanf(f,"%f", &img->redAvg);
        fscanf(f, "%f", &img->greenAvg);
        fscanf(f, "%f", &img->blueAvg);
        for (int i = 0; i < 4; i++) {
                fscanf(f, "%f", &img->redAvg1[i]);
                fscanf(f, "%f", &img->greenAvg1[i]);
                fscanf(f, "%f", &img->blueAvg1[i]);
        }
        for (int i = 0; i < 16; i++) {
                fscanf(f, "%f", &img->redAvg2[i]);
                fscanf(f, "%f", &img->greenAvg2[i]);
                fscanf(f, "%f", &img->blueAvg2[i]);
        }
        fclose(f);
}


float compareRed(struct imgAttributes img1, struct imgAttributes img2) {
        float result = 0;
        result += fabs(img1.redAvg - img2.redAvg);
        for (int i = 0; i < 4; i++)
                result+= fabs(img1.redAvg1[i] - img2.redAvg1[i]);
        for (int i = 0; i < 16; i++)
                result += fabs(img1.redAvg2[i] - img2.redAvg2[i]);


        return result/21;
}


float compareGreen(struct imgAttributes img1, struct imgAttributes img2) {
        float result = 0;
        result += fabs(img1.greenAvg - img2.greenAvg);
        for (int i = 0; i < 4; i++)
                result += fabs(img1.greenAvg1[i] - img2.greenAvg1[i]);
        for (int i = 0; i < 16; i++)
                result += fabs(img1.greenAvg2[i] - img2.greenAvg2[i]);


        return result / 21;
}


float compareBlue(struct imgAttributes img1, struct imgAttributes img2) {
        float result = 0;
        result += fabs(img1.blueAvg - img2.blueAvg);
        for (int i = 0; i < 4; i++)
                result += fabs(img1.blueAvg1[i] - img2.blueAvg1[i]);
        for (int i = 0; i < 16; i++)
                result += fabs(img1.blueAvg2[i] - img2.blueAvg2[i]);


        return result / 21;
}




int main(int argc, char *argv[]) {


        
        char fileName1[20];        
        char fileName2[20];
        char *tmpFileName = "tmp.bin";
        printf("Input first file name: ");
        scanf("%s",fileName1);
        printf("Input second file name: ");
        scanf("%s", fileName2);
        
        printf("\n\nFile %s:\n", fileName1);
        createTmpBinaryFile(fileName1, tmpFileName);//create a binary data file from the source ppm file
        createOutputFile("out1.txt", "tmp.bin");


        printf("File %s:\n", fileName2);
        createTmpBinaryFile(fileName2, tmpFileName);//create a binary data file from the another ppm file
        createOutputFile("out2.txt", "tmp.bin");
        
        struct imgAttributes img1, img2;


        loadStruct("out1.txt", &img1);
        loadStruct("out2.txt", &img2);
        
        


       
        
        printf("The average value of absolute deviations of the red color is %f\n", compareRed(img1, img2));


        


        printf("The average value of absolute deviations of the red color is %f\n", compareGreen(img1, img2));


       


        printf("The average value of absolute deviations of the red color is %f\n", compareBlue(img1, img2));




    return 0;
}