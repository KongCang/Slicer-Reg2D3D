#ifndef MATRICES_H
#define MATRICES_H


#include <iostream>
#include <stdio.h>
#include <string>
#include "math.h"
#define PI 3.1415926535897932384626433832795

enum Rot_Axis{Rot_Axis_X,Rot_Axis_Y,Rot_Axis_Z};//rotation center

class Matrices
{
private:
        int Row;//size of row
        int Column;//size of column
        double **MatrixData;//matrix data

public:

        Matrices(const Matrices &other)
        {
            this->Row = other.Row;
            this->Column = other.Column;

            this->MatrixData = new double *[this->Row];
            for (int i=0; i<this->Row; i++){
                 this->MatrixData[i] = new double [this->Column];
             }

            CopyMatrix((Matrices *)&other);
        }

        void operator = (const Matrices &other)
        {
            if(this->MatrixData!=NULL)
                { for (int i=0; i<this->Row; i++) {delete [] this->MatrixData[i];}
                    delete [] this->MatrixData;}

            this->Row = other.Row;
            this->Column = other.Column;

            this->MatrixData = new double *[this->Row];
            for (int i=0; i<this->Row; i++){
                 this->MatrixData[i] = new double [this->Column];
             }

            CopyMatrix((Matrices *)&other);
        }

        /*************************/
        //constructor, destructor
        /*************************/
        Matrices(){this->Row = 0; this->Column = 0; this->MatrixData = NULL;}
        ~Matrices(){
            if(this->MatrixData!=NULL)
                { for (int i=0; i<this->Row; i++) {delete [] this->MatrixData[i];}
                    delete [] this->MatrixData;}
            this->Row = 0;
            this->Column = 0;}

        Matrices(int row, int column){
            this->Row = row;
            this->Column = column;

            //allocate array memory
            this->MatrixData = new double *[this->Row];
            for (int i=0; i<this->Row; i++){
                 this->MatrixData[i] = new double [this->Column];
            }
        }

        /*************************/
        //Set and get methods
        /*************************/
        void SetRowColumn(int r, int c){

            this->Row = r;
            this->Column = c;
            //deallocate memory
            if(this->MatrixData!=NULL) { for (int i=0; i<this->Row; i++) {delete [] this->MatrixData[i];}
                    delete [] this->MatrixData;};
             //reallocate memory
            this->MatrixData = new double *[this->Row];
            for (int i=0; i<this->Row; i++){
                 this->MatrixData[i] = new double [this->Column];
            }

        }

        void SetMatrixData(int r, int c, double value)
        {
            if(this->MatrixData != NULL) {
                this->MatrixData[r][c]=value;
            }
        }

        double GetMatrixData(int r, int c){
            if(this->MatrixData != NULL) {
                return this->MatrixData[r][c];
            }
            return 0.0;
        }


        /*************************/
        //Methods
        /*************************/

        //copy matrix
        void CopyMatrix(Matrices* source);

        //set as an identity
        void SetIdentityMatrix();

        //invert matrix in Gauss-Jordan method
        void InvertMatrix();

        //multiply matrices
        void MultiplyMatrix_right(Matrices* B); //with B the right number, i.e. this*B
       // void MultiplyMatrix_left(Matrices* B); //with B the left number i.e. B*this    //might be useful in the future
        void MultiplyMatrix(Matrices *A, Matrices*B); // this = A*B

        //rotate matrix, angle in degree
        void RotateMatrix(double angle,Rot_Axis axis);

        //translate matrix
        void TranslateMatrix(double displacement_along_X,double displacement_along_Y, double displacement_along_Z);

};


#endif // MATRICES_H
