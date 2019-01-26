/*
author: Ömer Seyrekbasan 15011029
OS: Windows 10 64 bit
Compiler: GCC 8.1.0 
contact:seyrekbasanomer@gmail.com
*/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

//PGM dosya tipinin header bilgileri için oluşturulan veri tipi
typedef struct HEADER {
    int width;
    int height;
    int max_val;
} HEADER;



HEADER read_header(FILE*);
int** read_data(FILE* , HEADER); 
void write_image(FILE* ,  int**, HEADER);
int** convolution(int **, HEADER, double **, int);
int** normalize_sobel(int **, int** , HEADER);
double** calculate_gauss(double );
void normalize_laplacian(int**, HEADER);

int main(int argc, char const *argv[])
{
    //fi = okunan resim
    FILE *fi;
    //fo = okunan resim aynı şekilde bastırılır
    FILE *fo;
    // fg =  gauss filtresi ile elde edilen görüntü bastırılır
    FILE *fg;
    // fs = sobel filtresi ile elde edilen görüntü bastırılır
    FILE *fs;
    // fl = laplacian filtresi ile elde edilen görüntü bastırılır.
    FILE *fl;
 
    // okunan resim burada tutulur
    int ** original_image;
    //gauss filtresinden çıkan resim burada tutulur
    int ** output_gaussian;
    //laplacian filtresinden çıkan resim burada tutulur
    int ** output_laplacian;

    //sobelx filtresinden çıkan resim burada tutulur
    int **output_sobelx;
    //sobelx filtresinden çıkan resim burada tutulur
    int ** output_sobely;
    //sobel filtresinin gradientinden  çıkan resim burada tutulur
    int ** output_sobel;

    int i;
    HEADER image_header;
  
    //filtreler burada tutulur
    double ** gauss_kernel;
    double ** sobel_kernelx;
    double ** sobel_kernely;
    double ** laplacian_kernel;

    //filtreler için memory açılır.
    //genelde filtreler 3x3 olduğu için heap alanına grek yoktur ama
    //daha büyük filtreler içinde kullanışlı olması için böyle yazıldı.
    sobel_kernelx = (double**) malloc(3*sizeof(double*));
    for(i = 0; i < 3; i++)
        sobel_kernelx[i] = (double*)calloc(3, sizeof(double));

    sobel_kernely = (double**) malloc(3*sizeof(double*));
    for(i = 0; i < 3; i++)
        sobel_kernely[i] = (double*)calloc(3, sizeof(double));

    laplacian_kernel = (double**) malloc(3*sizeof(double*));
    for(i = 0; i < 3; i++)
        laplacian_kernel[i] = (double*)calloc(3, sizeof(double));

    if (sobel_kernelx == NULL || sobel_kernely == NULL || laplacian_kernel == NULL) {
        printf("Not Enough Memory!");
        exit(0);
    } else {
        printf("Successfully Allocated Memory!\n");
    }
      
    //sobelx filtresi tanımlaması
    sobel_kernelx[0][0] = -1;
    sobel_kernelx[0][1] = 0;
    sobel_kernelx[0][2] = 1;
    sobel_kernelx[1][0] = -2;
    sobel_kernelx[1][1] = 0;
    sobel_kernelx[1][2] = 2;
    sobel_kernelx[2][0] = -1;
    sobel_kernelx[2][1] = 0;
    sobel_kernelx[2][2] = 1;

    //sobely filtresi tanımlaması
    sobel_kernely[0][0] = 1;
    sobel_kernely[0][1] = 2;
    sobel_kernely[0][2] = 1;
    sobel_kernely[1][0] = 0;
    sobel_kernely[1][1] = 0;
    sobel_kernely[1][2] = 0;
    sobel_kernely[2][0] = -1;
    sobel_kernely[2][1] = -2;
    sobel_kernely[2][2] = -1;

    //laplacian filtresi tanımlaması
    laplacian_kernel[0][1] = 0;
    laplacian_kernel[0][0] = 1;
    laplacian_kernel[0][2] = 0;
    laplacian_kernel[1][0] = 1;
    laplacian_kernel[1][1] = -4;
    laplacian_kernel[1][2] = 1;
    laplacian_kernel[2][0] = 0;
    laplacian_kernel[2][1] = 1;
    laplacian_kernel[2][2] = 0;
    
    //Laplacian of gaussian filtresi
    // laplacian_kernel[0][0] = 0;
    // laplacian_kernel[0][1] = 0;
    // laplacian_kernel[0][2] = 1;
    // laplacian_kernel[0][3] = 0;
    // laplacian_kernel[0][4] = 0;
    // laplacian_kernel[1][0] = 0;
    // laplacian_kernel[1][1] = 1;
    // laplacian_kernel[1][2] = 2;
    // laplacian_kernel[1][3] = 1;
    // laplacian_kernel[1][4] = 0;
    // laplacian_kernel[2][0] = 1;
    // laplacian_kernel[2][1] = 2;
    // laplacian_kernel[2][2] = -16;
    // laplacian_kernel[2][3] = 2;
    // laplacian_kernel[2][4] = 1;
    // laplacian_kernel[3][0] = 0;
    // laplacian_kernel[3][1] = 1;
    // laplacian_kernel[3][2] = 2;
    // laplacian_kernel[3][3] = 1;
    // laplacian_kernel[3][4] = 0;
    // laplacian_kernel[4][0] = 0;
    // laplacian_kernel[4][1] = 0;
    // laplacian_kernel[4][2] = 1;
    // laplacian_kernel[4][3] = 0;
    // laplacian_kernel[4][4] = 0;
    

    // yazım işlemleri için ayrı ayrı dosyalar açılır
	fi = fopen("images/pentagone.1024.pgm","rb");
	fo = fopen("images/originalpentagone.pgm","wb+");
	fg = fopen("images/gausspentagone.pgm","wb+");
	fs = fopen("images/sobelpentagone.pgm","wb+");
	fl = fopen("images/laplacianpentagone.pgm","wb+");
	
    if (fi == NULL) {
        printf("Error Opening Input File!\n");
        printf("Exiting...\n");
        exit(0);
    } 
       
    if (fg == NULL || fs == NULL || fo == NULL) {
        printf("Error Opening Output File!\n");
        printf("Exiting...\n");
        exit(0);
    } 
    
    //bu fonksiyondan image headeri okunur.
    image_header = read_header(fi);

    printf("width = %d\n", image_header.width);
    printf("height = %d\n", image_header.height);
    printf("max value = %d\n", image_header.max_val);
    
    //bu fonksiyondan pixel bilgileri okunur
    original_image = read_data(fi, image_header);
    fclose(fi);

    //bu fonksiyon verilen pointerı ve headerı kurallara uygun olarak file'a yazar
    write_image(fo, original_image, image_header);
	fclose(fo);

    //konvolüsyon işlemi gerçekleştirilir
    output_laplacian = convolution(original_image, image_header, laplacian_kernel, 3);
    normalize_laplacian(output_laplacian, image_header);
    write_image(fl, output_laplacian, image_header);

	fclose(fl);
    free(laplacian_kernel);
    free(output_laplacian);


    //gauss filtresi oluşturulur
    gauss_kernel = calculate_gauss(1.0);

    //konvolüsyon işlemi gerçekleştirilir
    output_gaussian = convolution(original_image, image_header, gauss_kernel, 5);
    write_image(fg, output_gaussian, image_header);
	fclose(fg);
    free(original_image);
    free(gauss_kernel);

    //konvolüsyon işlemi gerçekleştirilir
    output_sobelx = convolution(output_gaussian, image_header, sobel_kernelx, 3);
    output_sobely = convolution(output_gaussian, image_header, sobel_kernely, 3);
    
    free(output_gaussian);
    
    output_sobel = normalize_sobel(output_sobelx, output_sobely, image_header);

    free(output_sobelx);
    free(output_sobely);

    write_image(fs, output_sobel, image_header);
	fclose(fs);

   
    free(output_sobel);

    return 0;
}
 
//Dosyanın header bilgisinin okunması için yazılmış fonksiyon.
HEADER read_header(FILE *fi) {
    HEADER image_header;
    
    char c = 0;
    int tmp = 0;
    int cont = 0;
    char buffer[25];
    


        fscanf(fi, "%s", buffer);
        if ( buffer[0] == 'P' && buffer[1] == '5' )
            printf("PGM File Format\n");
        else {
            printf("No PGM File Format.\n Exiting...\n");
            exit(0);
        }
        do {
            c = getc(fi);
            //boşluklar ve # comment satırları geçilir
            if (c != '#' && c != '\n') {
                fseek(fi, -1, SEEK_CUR);
                fscanf(fi, "%d", &tmp);
                switch (cont) {
                    case 0:
                        image_header.width = tmp;
                        break;
                    case 1:
                        image_header.height = tmp;
                        break;
                    case 2:
                        image_header.max_val = tmp;
                        break;
                }
                cont++;
            } else {    
                //boşluklar ve # comment satırları geçilir         
                while((c != '\n')) {
                    c = getc(fi);
                }                  
            }
        } while (cont != 3);
    return image_header;

}

//file dan pixel değerlerini okur
int** read_data(FILE* fi, HEADER image_header) {
    char * buffer;
    int **image;
    int i = 0;
    int j = 0;
    int k = 0;
    char c = 0;
    int tmp = 0;
    int size = 0;



    buffer = (char*) malloc(100000000*sizeof(char));

    if (buffer == NULL) {
        printf("Not Enough Memory!");
    } else {
        printf("Successfully Allocated Memory!\n");
    }

    //hem okuyup hem yazma işlemi yerine önce bir buffer'a okunma işlemi daha stabil çalışmakta
    i = 0;
    while (!feof(fi)) {
        c = getc(fi);
        buffer[i] = c;
        i++;
    }
    size = i;

    printf("buffer size = %d\n", size);

    //resim için yer açılır
    image = (int**) malloc(image_header.height*sizeof(int*));
    for (i = 0; i < image_header.width; i++)
        image[i] = (int*)calloc(image_header.width, sizeof(int));

    if (image == NULL) {
        printf("Not Enough Memory\nExiting...\n");
        exit(0);
    }

    k = 0;
    //eğer max_val 256 dan büyük ise bir byte okumak yetmemektedir.
    if (image_header.max_val < 256) { 
        i = 0;      
        while (i < image_header.height) {
            j = 0;
            while (j < image_header.width) {
                //burada 1 byte okumak için 1 byte büyüklüğünde olan unsigned char tipi kullanılmıştır
                tmp = (unsigned char) buffer[k];
                image[j][i] = tmp;
                j++;
                k++;
            }
            i++;
        }     
    }  
    free(buffer);
    return image;
} 


//bu fonksiyon verilen resmi kurallara uygun olarak file'a yazar
void write_image(FILE* fo,  int** image, HEADER image_header) {
    int i = 0;
    int j = 0;
    char c = 0;
    fprintf(fo,"P5\n");
    fprintf(fo,"%d %d\n", image_header.width, image_header.height);
    fprintf(fo,"%d\n", image_header.max_val);

        for (i = 0; i < image_header.height; i++) {
        for (j = 0; j < image_header.width; j++) {
  	        c = (unsigned char) image[j][i];
            putc(c,fo);
        }
    }
}

//bu fonksiyon verilen resim üzerinde verilen filtreyi geçirir
int** convolution(int **image, HEADER image_header, double **kernel, int kernel_size) {
    
    int **filtered_image;
    int i = 0;
    int j = 0;
    int k = 0;
    int t = 0;
    int row = 0;
    int col = 0;
    int tmp;
    //overhead değişkeni kenarlarda kalacak olan boşluktur.
    int overhead = (kernel_size / 2);
    
    //orijinal resim korunmaktadır.
    filtered_image = (int**) malloc(image_header.height*sizeof(int*));      
    for (i = 0; i < image_header.width; i++)
        filtered_image[i] = (int*)calloc(image_header.width, sizeof(int));

    if (filtered_image == NULL)  {
        printf("Not Enough Memory!\n");
    } else {
        printf("Successfully Allocated Memory!\n");
    }

    //filtre resim üzerinde bir bir kaydırılırs
    for (i = overhead; i < image_header.height -overhead; i++) {
        for (j = overhead; j < image_header.width -overhead; j++) {
            tmp = 0 ;
            row = i+overhead;
            col = j+overhead;
            //filtrenin sağ alt köşesinden sol üst köşesine doğru okuma vardır
            for (k = 0; k < kernel_size; k++) {
                for (t = 0; t <kernel_size; t++) {
                    //çarpım işlemi yapılır
                    tmp += kernel[kernel_size-1-k][kernel_size-1-t] * image[row-k][col-t]; 
                }                
            }

            filtered_image[i][j] = tmp;
        }
    }
 
    return filtered_image;

}

//sobelx ve sobely gradienti hesaplanır
int** normalize_sobel(int **sobelx, int** sobely,HEADER image_header) {
    int i;
    int j;
    int tmp;
    int ** return_image;


    return_image = (int**) malloc(image_header.height*sizeof(int*));

    for (i = 0; i < image_header.width; i++)
        return_image[i] = (int*)calloc(image_header.width, sizeof(int));

    if (return_image == NULL) {
        printf("Not Enough Memory\nExiting...\n");
        exit(0);
    }
    printf("Successfully Allocated Memory!\n");

    for (i = 2; i < image_header.height - 2; i++) {
        for ( j = 2; j < image_header.width - 2; j++) {
            tmp = sqrt((sobelx[i][j] * sobelx[i][j]  + sobely[i][j] * sobelx[i][j]));

            //burada çok basit bir normalizsyon vardır.
            if (tmp > 255)
                return_image[i][j] = 255;
            else if (tmp < 0)
                return_image[i][j] = 0;
            else 
                return_image[i][j] = (int) tmp;
            
        }
    }

    return return_image;

}

//verilen standar sapma için gauss filtresi hesaplanır
double** calculate_gauss(double sd) {

    double **gauss_kernel;
    int i;
    int j;
    double r, s = 2.0 * sd * sd; 
    double sum = 0.0; 

    gauss_kernel = (double**) malloc(5*sizeof(double*));
    for(i = 0; i < 5; i++)
        gauss_kernel[i] = (double*)calloc(5, sizeof(double));

    if (gauss_kernel == NULL) {
        printf("Not Enough Memory!");
        exit(0);
    }
        

    printf("Successfully Allocated Memory!\n");

    for (i = -2; i <= 2; i++) { 
        for (j = -2; j <= 2; j++) { 
            r = sqrt(i * i + j * j); 
            gauss_kernel[i + 2][j + 2] = (exp(-(r * r) / s)) / (3.14 * s); 
            sum += gauss_kernel[i + 2][j + 2]; 
        } 
    } 

    for (i = 0; i < 5; ++i) 
        for (j = 0; j < 5; ++j) 
            gauss_kernel[i][j] /= sum; 

    for (i = 0; i < 5; ++i) {
        for (j = 0; j < 5; ++j)
            printf(" %2lf ", gauss_kernel[i][j]);
        printf("\n");
    }
        
    return gauss_kernel;

}

//laplacian filtresi üzerinde çok basit bir normalizasyon yapılır
//bu konvolüsyon işlemi sırasında da yapılabilir fakat diğer filtreleri etkilememesi için ayrı bi şekilde yaptım
void normalize_laplacian(int** image, HEADER image_header ) {
    int i;
    int j;

    for(i = 0; i < image_header.height; i++) {
        for(j = 0; j < image_header.width; j++) {
            if(image[i][j] > 255)
                image[i][j] = 255;
            else if(image[i][j] < 0)
                image[i][j] = 0;
        }
    }
}