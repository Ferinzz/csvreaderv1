// csvreader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <cstdio>
#include <fstream>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <charconv>
#include <string>
#include <filesystem>


//special notes. For some reason txt files in Windows adds the delimiter to the end of the line. If you have ; then the last value before newline will be ;
//if tab it will be a tab.
//When reading per byte character you will see the final added delimiter. But if you stop at \n the extra hidden delimiter is not shown and it consumes all bytes used to declare the end of line.
//3 bytes are used to declare the end of line. The delimiter, 113 and 10.

int main()
{
    std::cout << "Hello World!\n";

    int is_ok = EXIT_FAILURE;
    FILE* fp = std::fopen("C:/Users/benja/source/rando programs/csvreader/csvreader/test.txt", "r+");
    if (!fp)
    {
        std::perror("File opening failed");
        return is_ok;
    }

    int c; // Note: int, not char, required to handle EOF
    //read first line to output what the format looks like. 113 represents a newline character based on testing on Windows txt files.
    std::string firstline;
    while ((c = std::fgetc(fp)) != '\n' && c != EOF)
    {
        firstline += c;
        std::cout << char(c);
    }
    std::cout << std::endl;
    std::cout << firstline<<std::endl;

    char delimiter;
    //putting in {} because we only need the string temporarily to get the desired delimiter. Could probably figure this out programmatically but bah.
    {
        //We need to ask the user what delimiter to used. This can be determined by the header row. usually either | , ; or TAB.
        std::string delimiterString;
        std::cout << std::endl;
        std::cout << "choose the delimiter :";
        std::cout << std::endl;
        std::getline(std::cin, delimiterString);
        std::cout << std::endl;
        delimiter = delimiterString[0];
        std::cout << std::endl; }

    //get column count
    int columnCount = 1;
    for (auto i = firstline.begin(); i != firstline.end(); i++)
    {
        std::cout << *i;
        *i == delimiter ? columnCount++:NULL;
    }
    std::cout << columnCount << std::endl;

    int columnSelected = 0;
    while (columnSelected > columnCount || columnSelected < 1)
    {
        std::cout << "which column would you like to output? 1-" << columnCount <<std::endl;
        std::cin >> columnSelected;
    }

    //create the directory if not already created Also assigns the current path to the temp folder.
    //currently just using temp folder until I figure out a good way to update to a folder where exe resides.
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::create_directories("csvreaderoutputs");
    //create an output file and filestream.
    std::filesystem::path filename = "csvreaderoutputs/Testoutput.txt";
    std::ofstream ostrm(filename, std::ios::binary);
    if (!ostrm) {
        std::cerr << "Failed to open file at " << filename << std::endl;
        return 1;
    }
    //std::cout << std::filesystem::current_path();

    //because we stopped at the new line, go one more step to get past the new line value.
    //c = std::fgetc(fp);

    


    /*********
    
    Need to start finding and outputting the specific column results.
    loop through whole thing one line at a times
    Each line count the columns until you reach desired column check for "
    Do not use getline because it requires more memory manipulation
    *******************/

    std::cout << std::endl;
    while ((c = std::fgetc(fp)) != EOF) // Standard C I/O file reading loop
    {
        //std::putchar(c);
        //now for a per-line loop counting columns
        for (int i = 1; i < columnSelected; i++)
        {
            c = std::fgetc(fp);
            while(c!=delimiter && c != EOF) {
                c = std::fgetc(fp);
            }

        }

        //now we're at the desired column so we loop until the next delimiter outputing the value into the output file
        //step one more to get out of the first delimiter
        c = std::fgetc(fp);

        //need to handle quotes delimiting sections so we need to check if the first character is a ".
        //if it is, then we need to ignore the normal delimiter
        //Creating a temp delimiter so handle whether the column starts with a " or not. If there's a " then we need to ignore all delimiters until the " is escaped.
        char tempdelimiter = ' ';
        if (c == '\"') { 
            tempdelimiter = '\"';
            c = std::fgetc(fp);
        }
        else tempdelimiter = delimiter;
        while (c != tempdelimiter && c != EOF)
        {
            ostrm << char(c);
            c = std::fgetc(fp);
        }
        if (tempdelimiter != delimiter) { c = std::fgetc(fp); }
        ostrm << '\n';
        if (c != delimiter) { std::cout << "error, column did not end at end of \""; }

        //step through until the end of the line.
        /*while (c != '\n' && c != EOF)
        {
            c = std::fgetc(fp);
        }*/

    }

    if (std::ferror(fp))
        std::puts("I/O error when reading");
    else if (std::feof(fp))
    {
        std::puts("End of file reached successfully");
        is_ok = EXIT_SUCCESS;
    }

    std::fclose(fp);
    ostrm.close();
    return is_ok;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
