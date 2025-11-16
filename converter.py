"""
Convert files from a directory into a files.h simulated filesystem format.
"""

import os
import sys
from pathlib import Path

# Constants matching the C header expectations
MAX_FILE_WIDTH = 256  # Adjust if your C code uses different values
MAX_FILE_LENGTH = 1024


def escape_c_string(s):
    """Escape a string for use in C code."""
    # Replace backslash first, then other escape sequences
    s = s.replace('\\', '\\\\')
    s = s.replace('"', '\\"')
    s = s.replace('\n', '\\n')
    s = s.replace('\r', '\\r')
    s = s.replace('\t', '\\t')
    return s


def read_file_as_lines(filepath, max_width=MAX_FILE_WIDTH):
    """
    Read a file and split it into lines, ensuring each line fits within max_width.
    Returns a list of strings, each representing one line with \\n preserved.
    """
    lines = []
    
    try:
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            for line in f:
                # Keep the newline character if present
                if len(line) <= max_width:
                    lines.append(line)
                else:
                    # Split long lines into chunks
                    for i in range(0, len(line), max_width):
                        chunk = line[i:i + max_width]
                        lines.append(chunk)
    except Exception as e:
        print(f"Warning: Could not read {filepath}: {e}", file=sys.stderr)
        return []
    
    return lines


def generate_sfile_entry(filename, lines):
    """Generate a C struct initializer for one sfile entry."""
    entry_lines = []
    entry_lines.append(f'    {{.name = "{escape_c_string(filename)}",')
    entry_lines.append('        .contents = {')
    
    # Add each line as a C string
    for line in lines:
        escaped = escape_c_string(line)
        entry_lines.append(f'        "{escaped}",')
    
    entry_lines.append('        },')
    entry_lines.append(f'        .length = {len(lines)},')
    entry_lines.append('        .position = 0,')
    entry_lines.append('        .line = 0')
    entry_lines.append('    }')
    
    return '\n'.join(entry_lines)


def generate_files_h(directory, output_path='files.h', include_preferences=True):
    """
    Generate a files.h header from all files in the given directory.
    
    Args:
        directory: Path to directory containing files to include
        output_path: Where to write the output files.h
        include_preferences: Whether to include the preferences.txt entry
    """
    directory = Path(directory)
    
    if not directory.is_dir():
        print(f"Error: {directory} is not a directory", file=sys.stderr)
        return False
    
    # Collect all files (not directories)
    files_to_process = []
    for item in sorted(directory.iterdir()):
        if item.is_file():
            files_to_process.append(item)
    
    if not files_to_process:
        print(f"Warning: No files found in {directory}", file=sys.stderr)
    
    # Generate the header
    with open(output_path, 'w', encoding='utf-8') as out:
        # Write header guard and includes
        out.write('#ifndef FILES_H\n')
        out.write('#define FILES_H\n\n')
        out.write('\n')
        out.write('#include <stdarg.h>\n\n')
        
        # Write struct definition
        out.write('typedef struct {\n')
        out.write('    char name[LINE_SIZE];                               // For identification\n')
        out.write('    char contents[MAX_FILE_WIDTH][MAX_FILE_LENGTH];     // For data storage\n')
        out.write('    int line;\n')
        out.write('    int position;\n')
        out.write('    int length;\n')
        out.write('    } sfile;\n\n\n')
        
        # Start the files array
        out.write('sfile files[MAX_FILES] = {\n')
        
        # Add preferences.txt if requested
        if include_preferences and files_to_process:
            first_file = files_to_process[0].name
            out.write('    {.name = "preferences.txt",\n')
            out.write('        .contents = {\n')
            out.write(f'        "{escape_c_string(first_file)}\\n",\n')
            out.write('        "01\\n",\n')
            out.write('        "0\\n"\n')
            out.write('        },\n')
            out.write('        .length = 3,\n')
            out.write('        .position = 0,\n')
            out.write('        .line = 0\n')
            out.write('    },\n\n')
        
        # Add all files from directory
        for i, filepath in enumerate(files_to_process):
            lines = read_file_as_lines(filepath, MAX_FILE_WIDTH)
            
            if len(lines) > MAX_FILE_LENGTH:
                print(f"Warning: {filepath.name} has {len(lines)} lines, truncating to {MAX_FILE_LENGTH}", 
                      file=sys.stderr)
                lines = lines[:MAX_FILE_LENGTH]
            
            entry = generate_sfile_entry(filepath.name, lines)
            out.write(entry)
            
            # Add comma if not last entry
            if i < len(files_to_process) - 1:
                out.write(',\n\n')
            else:
                out.write('\n')
        
        out.write('};\n\n')
        
        # Write the function declarations and implementations
        out.write('''// Opens a simulated file (analogous to fopen)
void sopen(sfile** out, const char file_name[LINE_SIZE], char open_type[2]) {
    if (out == NULL) return;
    if (file_name[0] == '\\0') {
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



// Read up to max-1 characters into output from fin, stop on '\\n' (include it) or EOF.
// Returns output on success, NULL on EOF or error.
char* sgets(char* output, int max, sfile* fin) {
    if (output == NULL || max <= 0) return NULL;
    if (fin == NULL || fin->line >= fin->length) return NULL;

    // Cap max to internal line width
    if (max > MAX_FILE_WIDTH) max = MAX_FILE_WIDTH;

    int out_i = 0;         //  Character of the output string

    // While we can write at least one more char (reserve space for '\\0')
    while (out_i < max - 1 && fin->line < fin->length) {
        char c = fin->contents[fin->line][fin->position];
        output[out_i] = c;
        
        // We have to keep track of our position within the sfile & in the output string seperately
        out_i++;            //  Character of the output string
        fin->position++;    //  Character of one line of the sfile

        // If we read newline or nul, move to next stored row and stop
        if (c == '\\n' || c == '\\0') {
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

    output[out_i] = '\\0';
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
''')
    
    print(f"Successfully generated {output_path} with {len(files_to_process)} files")
    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python convert_to_filesh.py <directory> [output_path]")
        print("  directory: Directory containing files to convert")
        print("  output_path: Where to write files.h (default: files.h)")
        sys.exit(1)
    
    directory = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) > 2 else 'files.h'
    
    success = generate_files_h(directory, output_path)
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()