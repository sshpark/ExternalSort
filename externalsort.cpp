#include <iostream>
#include <algorithm>
#include <string.h>
#include <stdio.h>
using namespace std;
#define MAX 0x7fffffff
#define MIN -1

class ExternSort {

public:
    void sort() {
        time_t start = time(NULL);

        K = memory_sort();

        ls = new int[K];
        buf = new int[K+1];

        k_merge();

        delete[] ls;
        delete[] buf;

        time_t end = time(NULL);
        printf("total time: %f\n", (end-start)*1000.0 / CLOCKS_PER_SEC);
    }

    ExternSort(const char* input_file, const char* out_file, int count)
    {
        arrlen = count;
        input_file_path = new char[strlen(input_file) + 1];
        strcpy(input_file_path, input_file);
        output_file_path = new char[strlen(out_file) + 1];
        strcpy(output_file_path, out_file);
 
    }
 
    virtual ~ExternSort()
    {
        delete[] input_file_path;
        delete[] output_file_path;
    }


private:
    char* input_file_path;
    char* output_file_path;

    int arrlen;
    int K;
    int* ls;
    int* buf;

protected:
    int read_data(FILE* file, int a[], int n) {
        int i = 0;

        while (i < n && (fscanf(file, "%d", &a[i])) != EOF)
            i++;

        printf("Read %d integer\n", i);
        return i;
    }

    void write_data(FILE* file, int a[], int n) {
        for (int i = 0; i < n; i++)
            fprintf(file, "%d ", a[i]);

        fprintf(file, "%d", MAX);
    }

    char* get_temp_filename(int index) {
        char *tempname = new char[105];
        sprintf(tempname, "temp%d.txt", index);
        return tempname;
    }

    int memory_sort() {
        FILE* fin = fopen(input_file_path, "rt");
        int n = 0, file_count = 0;
        int* array = new int[arrlen];

        while ((n = read_data(fin, array, arrlen)) > 0) {
            std::sort(array, array+arrlen);

            char* filename = get_temp_filename(file_count++);
            FILE* tempfile = fopen(filename, "w");

            free(filename);
            write_data(tempfile, array, n);
            fclose(tempfile);
        }

        delete[] array;
        fclose(fin);

        return file_count;
    }

    void k_merge() {
        FILE* fout = fopen(output_file_path, "wt");
        FILE** farray = new FILE*[K];

        for (int i = 0; i < K; i++) {
            char* filename = get_temp_filename(i);
            farray[i] = fopen(filename, "rt");
            free(filename);
        }

        for (int i = 0; i < K; i++) {
            if (fscanf(farray[i], "%d", &buf[i]) == EOF) {
                printf("There is no %d file to merge\n", K);
            }
        }

        create_loser_of_tree();

        int tmp;

        while (buf[ls[0]] != MAX) {
            tmp = ls[0];
            fprintf(fout, "%d ", buf[tmp]);
            fscanf(farray[tmp], "%d", &buf[tmp]);
            adjust(tmp);
        }

        fprintf(fout, "%d ", buf[ls[0]]);

        for (int i = 0; i < K; i++) {
            fclose(farray[i]);
        }

        delete[] farray;
        fclose(fout);
    }

    void create_loser_of_tree() {
        buf[K] = MIN;
        for (int i = 0; i < K; i++)
            ls[i] = K;
        for (int i = 0; i < K; i++)
            adjust(i);
    }

    void adjust(int s) {
        int t = (s+K)>>1;

        while (t > 0) {
            if (buf[s] > buf[ls[t]]) {
                s ^= ls[t];
                ls[t] ^= s;
                s ^= ls[t];
            }
            t >>= 1;
        }

        ls[0] = s;
    }

    

};


const unsigned int count1 = 50000;
const unsigned int number_to_sort = 1566; //在内存中一次排序的数量  
const char* unsort_file = "unsort_data.txt"; //原始未排序的文件名 
const char* sort_file = "sort_data.txt";//已排序的文件名



void init_data(unsigned int num)
{
    FILE* f = fopen(unsort_file, "wt");
    for (int i = 0; i < num; i++)
        fprintf(f, "%d ", rand());
    fclose(f);
}


int main(int argc, char const *argv[])
{
    srand((unsigned)time(NULL));
    init_data(count1);
    ExternSort extSort(unsort_file, sort_file, number_to_sort);
    extSort.sort();
 
    return 0;
}