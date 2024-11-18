// indexing_file.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include <cstring>


const char filename[] = "data.txt";
const char index_filename[] = "index";
const char filler = ' ';


int main() {
    // Create index-file
    {
        char* str;  // string from file
        FILE* input = fopen(filename, "r");
        fseek(input, 0, SEEK_END);
        long file_size = ftell(input);
        rewind(input);
        str = new char[file_size + 1];

        fread(str, sizeof(char), file_size, input);
        str[file_size] = '\0';

        fclose(input);
        char* beg = str;
        char* end = str + 1;

        FILE* findex = fopen(index_filename, "wb");

        while (*end) {
            while ((*(++end) != '#') && (*end));
            fwrite(beg, sizeof(char), 2, findex); // name
            long pos = beg - str + 2, len = end - (beg + 2);
            fwrite(&pos, sizeof(long), 1, findex); // position
            fwrite(&len, sizeof(long), 1, findex); // length
            beg = end + 1;
            end = beg + 1;
        }
        fclose(findex);
    }
    // Find and replace string, using indexing file
    {
        char name[3] = {};
        char cur_name[3] = {};
        printf("Sentence name: ");
        fgets(name, 3, stdin);
        name[2] = '\0';
        getchar();
        long pos, len;
        
        //Search in index file
        FILE* findex = fopen(index_filename, "rb");
        printf("%s\n", name);
        while ((fread(cur_name, sizeof(char), 2, findex), strncmp(cur_name, name, 2)) && !feof(findex))
        {
            fseek(findex, 2 * sizeof(long), SEEK_CUR);
        }
        if (feof(findex))
        {
            printf("No sentence with such name");
            return -1;
        }
        //Read information about sentence
        fread(&pos, sizeof(long), 1, findex);
        fread(&len, sizeof(long), 1, findex);
        fclose(findex);

        //Find and replace sentence in file
        char* sentence = new char[len + 1];
        char* new_sentence = new char[len + 1];
        FILE* file = fopen(filename, "r+");
        
        //Find sentence
        fseek(file, pos, SEEK_SET);
        fread(sentence, sizeof(char), len, file);
        sentence[len] = '\0';
        printf("%s\n", sentence);

        //Replace sentence
        printf("New sentence:\n");
        fgets(new_sentence, len + 1, stdin);
        // last char in string can be '\n'
        size_t new_len = strlen(new_sentence);
        if (new_sentence[new_len - 1] == '\n')
            new_sentence[new_len - 1] = '\0';

        fseek(file, pos, SEEK_SET);
        fprintf(file, "%*s", -len, new_sentence);

        fclose(file);
    }
    return 0;
}
