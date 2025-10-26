#ifndef FILES_H
#define FILES_H


#include <stdarg.h>

typedef struct {
    char name[LINE_SIZE];                               // For identification
    char contents[MAX_FILE_WIDTH][MAX_FILE_LENGTH];     // For data storage
    int line;
    int position;
    int length;
    } sfile;


sfile files[MAX_FILES] = {
    {.name = "preferences.txt",
        .contents = {
        "CiancioTest11.asm\n",
        "01\n",
        "0\n"
        },
        .length = 3,
        .position = 0,
        .line = 0
    },

    {.name = "CiancioTest11.asm",
        .contents = {
        "; Program to take values and then print them backwards (testing recursive functions)\n",
        "; Get the number of values to enter.\n",
        "get\n",
        "; Check size(for memory safety!)\n",
        "cmp ax 18\n",
        "ja [EH]\n",
        "mov [MV] ax\n",
        "; Get& print all values\n",
        "fun [FN] 1 [MV]\n",
        "; Put the number of values, just so the lists are perfect opposite.\n",
        "put\n",
        "; Early Halt(for memory safety)\n",
        "EH\n",
        "halt\n",
        "; Main(like main() function) variable\n",
        "MV\n",
        "\n",
        "\n",
        "; Function parameter\n",
        "FP\n",
        "\n",
        "; Function\n",
        "FN\n",
        "; get the pointer to the pointer to the value we want.\n",
        "mov bx [FP]\n",
        "; get the pointer to the value we want\n",
        "mov bx [bx+1]\n",
        "; get the value we want(the counter)\n",
        "mov cx [bx]\n",
        "get\n",
        "; DEC counter\n",
        "add cx -1\n",
        "; Store counter to pass forward\n",
        "mov [VR] cx\n",
        "cmp cx 0\n",
        "JBE [EN]\n",
        "fun [FN] 1 [VR]\n",
        "; END(of function)\n",
        "EN\n",
        "put\n",
        "ret\n",
        "\n",
        "; Variable for storage of the counter between functions\n",
        "VR\n",
        "\n"
        },
        .length = 44,
        .position = 0,
        .line = 0
    }
};

// Opens a simulated file (analogous to fopen)
void sopen(sfile** out, const char file_name[LINE_SIZE], char open_type[2]) {
    if (out == NULL) return;
    if (file_name[0] == '\0') {
        *out = NULL;
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(files[i].name, file_name) == 0) {
            *out = &files[i];
            (*out)->line = 0;
            (*out)->position = 0;
            return;
        }
    }
    *out = NULL;
    return;
}

// Closes a simulated file (analogous to fclose)
void sclose(sfile* fin) {
    // We don't actually need to do anything.
    // Just for API Compatibility
}



// Read up to max-1 characters into output from fin, stop on '\n' (include it) or EOF.
// Returns output on success, NULL on EOF or error.
char* sgets(char* output, int max, sfile* fin) {
    if (output == NULL || max <= 0) return NULL;
    if (fin == NULL || fin->line >= fin->length) return NULL;

    // Cap max to internal line width
    if (max > MAX_FILE_WIDTH) max = MAX_FILE_WIDTH;

    int out_i = 0;         //  Character of the output string

    // While we can write at least one more char (reserve space for '\0')
    while (out_i < max - 1 && fin->line < fin->length) {
        char c = fin->contents[fin->line][fin->position];
        output[out_i] = c;
        
        // We have to keep track of our position within the sfile & in the output string seperately
        out_i++;            //  Character of the output string
        fin->position++;    //  Character of one line of the sfile

        // If we read newline or nul, move to next stored row and stop
        if (c == '\n' || c == '\0') {
            // move to next stored row for next call
            fin->line++;
            fin->position = 0;
            break;
        }

        // If we've reached the end of the stored chunk row, advance to next stored row
        // so subsequent calls continue reading the logical next part.
        if (fin->position >= MAX_FILE_WIDTH) {
            fin->line++;
            fin->position = 0;
            break; // return a partial "fgets"-style chunk
        }
    }

    output[out_i] = '\0';
    return output;
}

bool seof(sfile* fin) {
    if (fin == NULL) return true;
    return (fin->line >= fin->length);
}

// Formatted print to simulated file (analogous to fprintf)
int sfprintf(sfile* fout, const char* format, ...) {
    if (fout == NULL || fout->length >= MAX_FILE_LENGTH) {
        return -1;
    }

    va_list args;
    va_start(args, format);
    int result = vsnprintf(fout->contents[fout->length], MAX_FILE_WIDTH, format, args);
    va_end(args);

    if (result > 0) {
        fout->length++;
    }

    return result;
}

#endif