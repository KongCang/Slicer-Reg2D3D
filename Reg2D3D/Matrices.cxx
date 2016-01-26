#include "Matrices.h"

void Matrices::CopyMatrix(Matrices* source){

    if (this->MatrixData == NULL || (this->Column==0 && this->Row==0) ){std::cout<<"Matrix not initialized"<<std::endl; return;}
    if (this->Column != source->Column || this->Row != source->Row ){std::cout<<"Matrices must have the same size"<<std::endl; return;}

    for (int i=0;i<this->Row; i++){
        for(int j=0;j<this->Column;j++){
            this->MatrixData[i][j]=source->MatrixData[i][j];
        }
    }
}

void Matrices::SetIdentityMatrix(){

    if (this->MatrixData == NULL || (this->Column==0 && this->Row==0) ){
        std::cout<<"Matrix not initialized"<<std::endl;
        return;
    }

    for (int i=0;i<this->Row; i++){
        for(int j=0;j<this->Column;j++){
            if (j==i) {this->MatrixData[i][j] = 1;}
            else{this->MatrixData[i][j] = 0; }
        }
    }
}

void Matrices::InvertMatrix(){

        if ( this->Column!=this->Row )
        {
                std::cout<<"The matrix to be inverted must be a square one!\n";
                return ;
        }

        int n=this->Row;

        int i,j,k;
        int *is,*js;
        is=new int[n];
        js=new int[n];

        double d=0.0,p=0.0;
        for ( k=0;k<n;k++ )
        {
                //(1)
                d=0.0;
                for ( i=k;i<n;i++ )
                {
                        //(2)
                        for ( j=k;j<n;j++ )
                        {
                                //(3)
                                p=fabs (this->MatrixData[i][j] );
                                if ( p>d )
                                {
                                        //(4)
                                        d=p;
                                        is[k]=i;
                                        js[k]=j;
                                }//(4)'
                        }//(3)'
                }//(2)'
                if ( d+1.0==1.0 )
                {
                        //(2-2)
                        delete []is;
                        delete []js;
                        std::cout<<"This matrix is singular!\n";
                        return ;
                }//(2-2)'
                if ( is[k]!=k )
                {
                        //(2-3)
                        for ( j=0;j<n;j++ )
                        {
                                //(3-3)
                                p=this->MatrixData[k][j];
                                this->MatrixData[k][j]=this->MatrixData[is[k]][j];
                                this->MatrixData[is[k]][j]=p;
                        }//(3-3)'
                }//(2-3)'
                if ( js[k]!=k )
                {
                        //(2-4)
                        for ( i=0;i<n;i++ )
                        {
                                //(3-4)
                                p=this->MatrixData[i][k];
                                this->MatrixData[i][k]=this->MatrixData[i][js[k]];
                                this->MatrixData[i][js[k]]=p;
                        }//(3-4)'
                }//(2-4)'
                this->MatrixData[k][k]=1.0/this->MatrixData[k][k];
                for ( j=0;j<n;j++ )
                {
                        //(2-5)
                        if ( j!=k )
                        {
                                //(3-5)
                                this->MatrixData[k][j]=this->MatrixData[k][j]*this->MatrixData[k][k];
                        }//(3-5)'
                }//(2-5)'
                for ( i=0;i<n;i++ )
                {
                        //(2-6)
                        if ( i!=k )
                        {
                                //(3-6)
                                for ( j=0;j<n;j++ )
                                {
                                        //(4-6)
                                        if ( j!=k )
                                        {
                                                //(5-6)
                                                this->MatrixData[i][j]=this->MatrixData[i][j] -
                                                                              this->MatrixData[i][k]*this->MatrixData[k][j];
                                        }//(5-6)'
                                }//(4-6)'
                        }//(3-6)
                }
                for ( i=0;i<n;i++ )
                {
                        //(2-7)
                        if ( i!=k )
                        {
                                //(3-7)
                                this->MatrixData[i][k]=- ( this->MatrixData[i][k] ) *this->MatrixData[k][k];
                        }//(3-7)'
                }//(2-7)'
        }//(1)'
        for ( k=n-1;k>=0;k-- )
        {
                //(1-b)
                if ( js[k]!=k )
                {
                        //(2-b)
                        for ( j=0;j<n;j++ )
                        {
                                //(3-b)
                                p=this->MatrixData[k][j];
                                this->MatrixData[k][j]=this->MatrixData[js[k]][j];
                                this->MatrixData[js[k]][j]=p;
                        }//(3-b)'
                }//(2-b)'
                if ( is[k]!=k )
                {
                        //(2-b-2)
                        for ( i=0;i<n;i++ )
                        {
                                //(3-b-2)
                                p=this->MatrixData[i][k];
                                this->MatrixData[i][k]=this->MatrixData[i][is[k]];
                                this->MatrixData[i][is[k]]=p;
                        }//(3-b-2)'
                }//(2-b-2)'
        }//(1-b)'
        delete []is;
        delete []js;
}


void Matrices::MultiplyMatrix_right(Matrices* B){
    if ( this->Column!=B->Row )
        {
                std::cout<<"These two matrix do not satisfy the qualification of matrices multiply!\n";
                return ;
        }
        int m=this->Row;
        int n=this->Column;
        int l=B->Column;
        int i,j,k;
        Matrices tempMatrix(m,l);

        for ( k=0;k<l;k++ )
        {
                for ( i=0;i<m;i++ )
                {
                        tempMatrix.MatrixData[i][k]=0;
                        for ( j=0;j<n;j++ )
                                tempMatrix.MatrixData[i][k] += ( this->MatrixData[i][j] ) * ( B->MatrixData[j][k] );
                }
        }

        this->CopyMatrix(&tempMatrix);
}

void Matrices::MultiplyMatrix(Matrices *A, Matrices*B){
  if ( A->Column!=B->Row )
        {
                std::cout<<"These two matrix do not satisfy the qualification of matrices multiply!\n";
                return ;
        }

  if(this->Row != A->Row && this->Column != B->Column) { this->SetRowColumn(A->Row,B->Column);}
        int m=A->Row;
        int n=A->Column;
        int l=B->Column;
        int i,j,k;

        for ( k=0;k<l;k++ )
        {
                for ( i=0;i<m;i++ )
                {
                        this->MatrixData[i][k]=0;
                        for ( j=0;j<n;j++ )
                                this->MatrixData[i][k] += ( A->MatrixData[i][j] ) * ( B->MatrixData[j][k] );
                }
        }
}

void Matrices:: RotateMatrix(double angle, Rot_Axis axis){

        Matrices RotateMatrix(4,4);
        RotateMatrix.SetIdentityMatrix();

        angle=angle*(double)PI/(double)180.0; //change angle in radian

        if (this->Column!=4 )
        {
                std::cout<<"Error not a 4 columns matrix!\n";
                return;
        }

        switch ( axis )
        {
                case Rot_Axis_X:
                {
                        RotateMatrix.MatrixData[1][1]=cos ( angle );
                        RotateMatrix.MatrixData[1][2]=-sin ( angle );
                        RotateMatrix.MatrixData[2][1]=-RotateMatrix.MatrixData[1][2];
                        RotateMatrix.MatrixData[2][2]=RotateMatrix.MatrixData[1][1];
                        break;
                }
                case Rot_Axis_Y:
                {
                        RotateMatrix.MatrixData[0][0]=cos ( angle );
                        RotateMatrix.MatrixData[0][2]=sin ( angle );
                        RotateMatrix.MatrixData[2][0]=-RotateMatrix.MatrixData[0][2];
                        RotateMatrix.MatrixData[2][2]=RotateMatrix.MatrixData[0][0];
                        break;
                }
                case Rot_Axis_Z:
                {
                        RotateMatrix.MatrixData[0][0]=cos ( angle );
                        RotateMatrix.MatrixData[0][1]=-sin ( angle );
                        RotateMatrix.MatrixData[1][0]=-RotateMatrix.MatrixData[0][1];
                        RotateMatrix.MatrixData[1][1]=RotateMatrix.MatrixData[0][0];
                        break;
                }
        }

        this->MultiplyMatrix_right(&RotateMatrix);
}


void Matrices::TranslateMatrix(double displacement_along_X,double displacement_along_Y, double displacement_along_Z){

        if ( this->Column!=4 )
        {
                std::cout<<"Error not a 4 columns matrix!\n";
                return ;
        }
        Matrices TranslateMatrix(4,4);
        TranslateMatrix.SetIdentityMatrix();

        TranslateMatrix.MatrixData[0][3]=displacement_along_X;
        TranslateMatrix.MatrixData[1][3]=displacement_along_Y;
        TranslateMatrix.MatrixData[2][3]=displacement_along_Z;

        this->MultiplyMatrix_right(&TranslateMatrix);

}


