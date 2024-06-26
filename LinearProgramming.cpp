#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

#define M 20
#define N 20
#define MAX 10e8

static const double epsilon = 1.0e-8;
int equal(double a, double b) { return fabs(a-b) < epsilon; }
typedef struct {
  int m, n; // m=rows, n=columns, linear[m x n]
  double linear[M][N];
  int sign[M];
} Tableau;

//Ham gioi thieu
void infor();
//Ham menu yeu cau dau vao
bool menuin();
//Ham menu yeu cau file muon nhap
void menuFILE(Tableau *tab);
// Return row position now
int getCurrentY();
// go to xy position
void gotoxy(int x, int y);
// Enter by keyboard
void islet(Tableau *tab);
// file _ in
void filein(float *a, int &n, int v);
// print problem
void print_problem(Tableau *tab);
// 
void print_tableau(Tableau *tab, const char* mes);
// read file
void read_tableau(Tableau *tab, const char * filename);
void pivot_on(Tableau *tab, int row, int col);
// find colum has most negative number
int find_pivot_column(Tableau *tab);
// find row has a/b is smallest
int find_pivot_row(Tableau *tab, int pivot_col);
// 
void add_slack_variables(Tableau *tab);
// add big M (-1 variable)
void big_M(Tableau* tab);
int find_basis_variable(Tableau *tab, int col);
// print and write file
void print_optimal_vector(Tableau *tab, char *message, float matrix[]);
// main function
void simplex(Tableau *tab);
void nl(int k);


//Tableau tab  = { 5, 3, {                     
//    {  0.0 , -3, -4,   },  
//    { 600.0 ,  2.0 ,  1.0 ,   }, 
//    { 225 , 1.0 , 1.0 ,   }, 
//    {1000.0, 5.0,4.0},
//    {150.0,1.0,2.0},
//    {0.0,1.0,1.0}
//  },
//  { 0 , -1 , -1, -1,1,1 }
////  0 : equal; -1 : smaller or equal; 1 : bigger or equal
//};

int main(int argc, char *argv[]){
  int t=1;
  while (t){
    Tableau tab = {0};
    if (t==1) infor();
    getchar(); 
    int a = menuin();
    if(a) menuFILE(&tab); 
    else if (!a) {  islet(&tab); }

    simplex(&tab);
    system("pause");
    system("cls");
    getchar();
    t++;
  } 
  return 0;
} 

void nl(int k){ int j; for(j=0;j<k;j++) putchar('-'); putchar('\n'); }

void islet(Tableau *tab) {
    system("cls");
    int i, j;
    gotoxy(5,2);
    // Enter row m, col n
    printf("Nhap so hang m: ");
    scanf("%d", &tab->m);
    gotoxy(5,getCurrentY());
    printf("Nhap so cot n: ");
    scanf("%d", &tab->n);
    gotoxy(5,getCurrentY());
    // NEnter matrix size m x n
    printf("Nhap ham muc tieu: ");
    for (int i = 1; i < tab->n; i++){
      scanf("%lf", &tab->linear[0][i]);
    }
    gotoxy(5,getCurrentY());
    printf("Nhap dieu kien rang buoc:\n");
    for (i = 1; i < tab->m; i++) {
        gotoxy(8,getCurrentY());
        printf("Nhap rang buoc thu %d: ", i);
        for (j = 0; j < tab->n; j++) {
            scanf("%lf", &tab->linear[i][j]);
        }
    }
    // Enter array size n
    gotoxy(5,getCurrentY());
    printf("Nhap dau cua cac rang buoc\n\t-1: <=\n\t0: =\n\t1: >=\n");
    tab->sign[0] = 0;
    for (i = 1; i < tab->m; i++) {
        gotoxy(5,getCurrentY());
        printf("Nhap dau cua rang thu %d: ", i);
        scanf(" %d", &tab->sign[i]);
    }
}

void print_tableau(Tableau *tab, const char* mes) {
    static int counter = 0;
    int i, j;

    // Print the header
    printf("\n%d. Tableau %s:\n", ++counter, mes);
    printf("--------------");
    for (j = 0; j < tab->n; j++) {
        printf("-------------");
    }
    printf("\n");

    printf("| %-10s |", "Row");
    for (j = 0; j < tab->n; j++) {
        if (j == 0)
            printf(" %-10s |", "Z");
        else
            printf(" x%-9d |", j);
    }
    printf("\n--------------");
    for (j = 0; j < tab->n; j++) {
        printf("-------------");
    }
    printf("\n");

    // Print each row of the tableau
    for (i = 0; i < tab->m; i++) {
        if (i == 0) {
            printf("| %-10s |", "max");
        } else {
            printf("| b%-9d |", i);
        }
        for (j = 0; j < tab->n; j++) {
            // Check if the value is an integer
            if (equal((int)tab->linear[i][j], tab->linear[i][j])) {
                printf(" %10d |", (int)tab->linear[i][j]);
            } else {
                // Print floating point number with scientific notation for large numbers
                printf(" %10.2e |", tab->linear[i][j]);
            }
        }
        printf("\n--------------");
        for (j = 0; j < tab->n-1; j++) {
            printf("-------------");
        }
        printf("-------------\n");
    }
}

/* Example input file for read_tableau:
     4 5
      0   -0.5  -3 -1  -4 
     40    1     1  1   1 
     10   -2    -1  1   1 
     10    0     1  0  -1  
     0	   -1    0  1   0
     0 0 0 0 
*/
void read_tableau(Tableau *tab, const char * filename) {
  int err, i, j;
  FILE * fp;

  fp  = fopen(filename, "r" );
  if( !fp ) {
    printf("Cannot read %s\n", filename); return;;
  }
  memset(tab, 0, sizeof(*tab));
  err = fscanf(fp, "%d %d", &tab->m, &tab->n);
  if (err == 0 || err == EOF) {
    printf("Cannot read m or n\n"); return;;
  }
  for(i=0;i<tab->m; i++) {
    for(j=0;j<tab->n; j++) {
      err = fscanf(fp, "%lf", &tab->linear[i][j]);
      if (err == 0 || err == EOF) {
        printf("Cannot read \n", i, j); return;;
      }
    }
  }
    for(i=0;i<tab->m; i++) {
      err = fscanf(fp, "%d", &tab->sign[i]);
      if (err == 0 || err == EOF) {
        printf("Cannot read\n", i, j); return;;
      } 
  }
  fclose(fp);
}

void pivot_on(Tableau *tab, int row, int col) {
  int i, j;
  double pivot;

  pivot = tab->linear[row][col];
  assert(pivot>0);
  for(j=0;j<tab->n;j++)
    tab->linear[row][j] /= pivot;
  assert( equal(tab->linear[row][col], 1. ));

  for(i=0; i<tab->m; i++) { // foreach remaining row i do
    double multiplier = tab->linear[i][col];
    if(i==row) continue;
    for(j=0; j<tab->n; j++) { // r[i] = r[i] - z * r[row];
      tab->linear[i][j] -= multiplier * tab->linear[row][j];
    }
  }
}

int find_pivot_column(Tableau *tab) {
  int j, pivot_col = 1;
  double lowest = tab->linear[0][pivot_col];
  for(j=1; j<tab->n; j++) {
    if (tab->linear[0][j] < lowest) {
      lowest = tab->linear[0][j];
      pivot_col = j;
    }
  }
  if( lowest >= 0 ) {
    return -1; 
  }
  return pivot_col;
}

int find_pivot_row(Tableau *tab, int pivot_col) {
  int i, pivot_row = 0;
  double min_ratio = -1;
  for(i=1;i<tab->m;i++){

    double ratio = tab->linear[i][0] / tab->linear[i][pivot_col];
    if ( (ratio > 0  && ratio < min_ratio ) || min_ratio < 0 ) {
      min_ratio = ratio;
      pivot_row = i;
    }
  }
  if (min_ratio == -1)
    return -1; // Unbounded.
  return pivot_row;
}

void add_slack_variables(Tableau *tab) {
    int i, j;
    int row, count = 0;
    long long max = -MAX;
    for (i=1; i<=tab->m; i++ ){
    	if (tab->sign[i] == 1){
    		count++;
    		if(tab->linear[i][0] > max){
    			max = tab->linear[i][0];
    			row = i;
			}
		}
	}
    for(i=1; i<tab->m; i++) {
        for(j=1; j<tab->m; j++)	{
    	   if (tab->sign[i] == 0) continue;
		   if (tab->sign[i] == 1 && (i==j)) tab->linear[0][j+tab->n-1] = MAX;
		   tab->linear[i][j + tab->n -1] = (i==j);
	    }
    }
	count > 0? tab->n += tab->m : tab->n += tab->m -1;
    if (count > 0) for (i = 1; i < tab->m; i++){
    	if (i!=row) tab->linear[i][tab->n-1] = 0;
    	else if (row == i) tab->linear[i][tab->n-1] = -1;
	}
}

void big_M(Tableau* tab){
	int count = 0 ;
	for (int i = 1; i < tab->m; i++){if (tab->sign[i] == 1) count++;};
	if (count == 0) return;
	for (int i = tab->n-tab->m; i < tab->n-1; i++){
		if (tab->linear[0][i] == MAX){
			for (int j = 1; j < tab->m; j++){
				if (tab->linear[j][i]==1){
					for (int k = 0; k<tab->n; k++){
						tab->linear[0][k] += (-1)*MAX*tab->linear[j][k];
					}
				}
			}
		}
	
  }
  print_tableau(tab,"a");
}
	



void check_b_positive(Tableau *tab) {
  int i;
  for(i=1; i<tab->m; i++)
    assert(tab->linear[i][0] >= 0);
}

// Given a column of identity linearrix, find the row containing 1.
// return -1, if the column as not from an identity linearrix.
int find_basis_variable(Tableau *tab, int col) {
  int i, xi=-1;
  for(i=1; i < tab->m; i++) {
    if (equal( tab->linear[i][col],1) ) {
      if (xi == -1)
        xi=i;   // found first '1', save this row number.
      else
        return -1; // found second '1', not an identity linearrix.

    } else if (!equal( tab->linear[i][col],0) ) {
      return -1; // not an identity linearrix column.
    }
  }
  return xi;
}

void print_optimal_vector(Tableau *tab, char *message, float matrix[]) {
    FILE * fp;
    fp  = fopen("Optimal.txt", "w" );
    if( !fp ) {
    printf("Cannot write \n"); 
    exit(1);
    }
  // int length = sizeof(tab->sign) / sizeof(int);
  int cnt = 0, a;
  for (int i = 0; i<(sizeof(tab->sign) / sizeof(int)); i++){
    if (tab->sign[i] != 0) cnt++;
    if (tab->sign[i] == 1) a = 1;
  }
  if (a == 1) cnt++;
  cnt = tab->n - cnt;
  int j, xi;
  double S;
  printf("%s la [", message);
  fprintf(fp, "%s la [", message);
  for(j=1;j<tab->n;j++) { // for each column.
    xi = find_basis_variable(tab, j);
    if (xi != -1){
      if ( j < tab->n - 1) {
	       printf("x%d=%3.2lf, ", j, tab->linear[xi][0] );
	       fprintf(fp,"x%d=%3.2lf, ", j, tab->linear[xi][0]  );
	   }
      else {
	       printf("x%d=%3.2lf ] ", j, tab->linear[xi][0] );
	       fprintf(fp,"x%d=%3.2lf ] ", j, tab->linear[xi][0] );
	   }
      if (j<=cnt) S += -tab->linear[xi][0]*matrix[j];
      }
    else
      if ( j < tab->n - 1) {
	      printf("x%d=0, ", j);
	      fprintf(fp, "x%d=0, ", j);
	  }
      else {
	      printf(" x%d = 0 ]", j);
	      fprintf(fp," x%d = 0 ]", j);
	}
  }
  printf("\nGia tri lon nhat: %.2lf\n", S);
  fprintf(fp, "\nGia tri lon nhat: %.2lf\n", S);
  fclose(fp);
    gotoxy(5,getCurrentY());
    printf("Ban co muon tiep tuc (Y/N) ? ");
    char choice;
    gotoxy(5,getCurrentY());
    scanf("%c", &choice);
    if (choice == 'N' || choice == 'n') {
      exit(1);
    }
} 

void simplex(Tableau *tab) {
  system("cls");
  float matrix[10];
  print_problem(tab);
  for (int i = 1; i <= tab->n; i++){
    matrix[i] = tab->linear[0][i]; 
  }
  print_tableau(tab,"Khoi tao");
  int loop=0;
  add_slack_variables(tab);
  check_b_positive(tab);
  print_tableau(tab,"Them bien");
  big_M(tab);
  while( ++loop ) {
    int pivot_col, pivot_row;

    pivot_col = find_pivot_column(tab);
    if( pivot_col < 0 ) {
    print_optimal_vector(tab, "Gia tri toi uu ", matrix);
      break;
    }
    pivot_row = find_pivot_row(tab, pivot_col);
    if (pivot_row < 0) {
      printf("unbounded (no pivot_row).\n");
      break;
    }
    pivot_on(tab, pivot_row, pivot_col);
    print_tableau(tab," ");
    if(loop > 20) {
      printf("Qua nhieu vong lap > %d.\n", loop);
      break;
    }
  }
   
}

//Ham menu yeu cau file muon nhap
void menuFILE(Tableau *tab){
    system("cls");
    int v;
    printf("\n\t\t+------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                        |\n");
    printf("\t\t|                    %c   CHON FILE DE LAY DU LIEU    %c                     |\n", 16, 17);
    printf("\t\t|                                                                        |\n");
    printf("\t\t|             Nhan 1 : TEST1                 Nhan 2: TEST2               |\n");
    printf("\t\t|             Nhan 3 : TEST3                 Nhan 4: TEST4               |\n");
    printf("\t\t|             Nhan 5 : TEST5                 Nhan 6: TEST6               |\n");
    printf("\t\t|             Nhan 7 : TEST7                 Nhan 8: TEST8               |\n");
    printf("\t\t|             Nhan 9 : TEST9                 Nhan 10: TEST10             |\n");
    printf("\t\t|                                                                        |\n");
    printf("\t\t+------------------------------------------------------------------------+\n");
    printf("\n\t\t          Nhap file test (1-10): ");
	do{
		scanf("%d", &v);
		if(v!=1 && v!=2 && v!=3 && v!=4 && v!=5 && v!=6 && v!=7 && v!=8 && v!=9 && v!=10) printf("\nMoi ban nhap lai so thu tu file: ");
	}while(v!=1 && v!=2 && v!=3 && v!=4 && v!=5 && v!=6 && v!=7 && v!=8 && v!=9 && v!=10);
    if(v==1) read_tableau(tab, "TEST1.txt");
    if(v==2) read_tableau(tab, "TEST2.txt");
	if(v==3) read_tableau(tab, "TEST3.txt");
	if(v==4) read_tableau(tab, "TEST4.txt");
	if(v==5) read_tableau(tab, "TEST5.txt");
	if(v==6) read_tableau(tab, "TEST6.txt");
	if(v==7) read_tableau(tab, "TEST7.txt");
	if(v==8) read_tableau(tab, "TEST8.txt");
	if(v==9) read_tableau(tab, "TEST9.txt");
	if(v==10) read_tableau(tab, "TEST10.txt");
}


void infor(){
 	int i;
	// setcolor(0,9);
 	printf("\n%9c",201);
 	for (i=0;i<=85;i++) printf("%c",205);
 	printf("%c",187);
 	printf("\n        %c %86c",186,186);
 	printf("\n        %c                             DO AN LAP TRINH TINH TOAN                                %c",186,186);
 	printf("\n        %c                     DE TAI: Toi uu tuyen tinh Simplex - Big_M                        %c",186,186);
 	printf("\n        %c %86c",186,186);
 	printf("\n        %c           Sinh vien thuc hien:                   Giao vien huong dan:                %c",186,186);
 	printf("\n        %c           %c Nguyen Dinh Duy                       %c Nguyen Van Hieu                  %c",186,45,45,186);
 	printf("\n        %c           %c Tran Van Minh Tri  %55c",186,45,186);
 	printf("\n        %c %86c",186,186);
 	printf("\n        %c",200);
 	for (i=0;i<=85;i++) printf("%c",205);
 	printf("%c\n",188);
}

bool menuin() {
    char com;
    do {
    system("cls");
    printf("\n                                  %c   CHON CACH NHAP DU LIEU   %c\n", 16, 17);
    printf("\n                                        Nhap 1: Tu ban phim");
    printf("\n                                        Nhap 2: Tu file");
    printf("\n                                             -------\n");
    printf("\n          Nhap yeu cau: ");     
        scanf("%c", &com);
        if (com != '1' && com != '2') printf("\n                   Moi ban nhap lai: ");
    } while (com != '1' && com != '2');
    if (com == '1') return false; // from keyboard 0
    return true; // from file 1
}

void print_problem(Tableau* tab) {
        printf("+------------------------------------------------------------+\n");   
    // Objective Function
    printf("| Cuc dai : ");
    for (int j = 1; j < tab->n; ++j) {
        if (j == 1) {
            printf("%.2fx%d ", tab->linear[0][j], j);
            continue;
        }
        if (j > 1) {
            printf(tab->linear[0][j] > 0 ? " + " : " - ");
        }
        printf("%.2fx%d ", fabs(tab->linear[0][j]), j);
    }
    gotoxy(61, getCurrentY());
    printf("|\n+------------------------------------------------------------+\n| Conditions:");
    gotoxy(61, getCurrentY());
    printf("|\n");
    
    // Constraints
    for (int i = 1; i < tab->m; ++i) {
        printf("| ");
        for (int j = 1; j < tab->n; ++j) {
            if (j > 1) {
                printf(tab->linear[i][j] > 0 ? " + " : " - ");
                printf("%.2fx%d ", fabs(tab->linear[i][j]), j);
                continue;
            }
            else printf("%.2fx%d ", tab->linear[i][j], j);
        }
        switch (tab->sign[i]) {
            case 0:
                printf("= %.2f", tab->linear[i][0]);
                break;
            case -1:
                printf("<= %.2f ", tab->linear[i][0]);
                break;
            case 1:
                printf(">= %.2f ", tab->linear[i][0]);
                break;
        }
        gotoxy(61, getCurrentY());
        printf("|\n");
        printf("+------------------------------------------------------------+\n");
        
    }
    printf("\n");
    printf("Giai : \n");
    for (int i = 1; i <= tab->m; i++){
        tab->linear[0][i] = -tab->linear[0][i];
    }
}


int getCurrentY() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

